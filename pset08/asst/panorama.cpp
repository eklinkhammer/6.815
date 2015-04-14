#include "panorama.h"
#include "matrix.h"
#include <unistd.h>
#include <ctime>
#include "blending.h"

using namespace std;

// Pset 07. Compute xx/xy/yy Tensor of an image.
Image computeTensor(const Image &im, float sigmaG, float factorSigma) {
  int width = im.width();
  int height = im.height();

  Image lumiBlurred = getBlurredLumi(im, sigmaG);

  Image gradX = gradientX(lumiBlurred);
  Image gradY = gradientY(lumiBlurred);

  Image M(width, height, 3);

  float gradx, grady;
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      gradx = gradX(i,j);
      grady = gradY(i,j);
      M(i,j,0) = gradx*gradx;
      M(i,j,1) = gradx*grady;
      M(i,j,2) = grady*grady;
    }
  }
  Image blurredM = gaussianBlur_seperable(M, sigmaG*factorSigma);
  return blurredM;

}

// Pset07. Compute Harris Corners.
vector<Point> HarrisCorners(const Image &im, float k, float sigmaG, float factorSigma, float maxiDiam, float boundarySize) {
  vector<Point> points;
  Image R = cornerResponse(im, k, sigmaG, factorSigma);
  excludeCorners(R, boundarySize);

  Image max = maximum_filter(R,maxiDiam);
  for (int i = 0; i < R.width(); i++) {
    for (int j = 0; j < R.height(); j++) {
      if (R(i,j,0) > 0 && R(i,j,0) == max(i,j,0)) {
        Point p(i,j);
        points.push_back(p);
      }
    }
  }
  return points;
}

void excludeCorners(Image &im, float boundarySize) {
  for (int i = 0; i < im.width(); i++) {
    for (int j = 0; j < im.height(); j++) {
      if (i < boundarySize || j < boundarySize || j + boundarySize > im.height() ||
          i + boundarySize > im.width()) {
        for (int k = 0; k < im.channels(); k++) {
          im(i,j,k) = -1;
        }
      }
    }
  }
}

// Pset07. Compute response = det(M) - k*[(trace(M)^2)]
Image cornerResponse(const Image &im, float k, float sigmaG, float factorSigma) {
  int width = im.width();
  int height = im.height();
  Image M = computeTensor(im, sigmaG, factorSigma);
  Image R(width, height,1);
  float det;
  float trace, val;
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      det = M(i,j,0) * M(i,j,2) - M(i,j,1)*M(i,j,1);
      trace = M(i,j,0) + M(i,j,2);
      val = det - k*trace*trace;
      R(i,j,0) = val;
    }
  }
  return R;
}

// Pset07. Descriptor helper function
Image descriptor(const Image &blurredIm, Point p, float radiusDescriptor) {
  Image output(radiusDescriptor*2+1,radiusDescriptor*2+1,1);
  int centerX = p.x;
  int centerY = p.y;
  int descriptX = 0, descriptY = 0;

  
  

  for (int i = centerX - radiusDescriptor; i < centerX + radiusDescriptor + 1; i++) {
    descriptY = 0;
    for (int j = centerY - radiusDescriptor; j < centerY + radiusDescriptor + 1; j++) {
      if (pixelInFrame(blurredIm, i, j)) {
        output(descriptX,descriptY,0) = blurredIm(i,j,0);
      }
      descriptY++;
    }
    descriptX++;
  }

  float variance = output.var();
  float mean = output.mean();
  float std = sqrt(variance);

  for (int i = 0; i < output.width(); i++) {
    for (int j = 0; j < output.height(); j++) {
        output(i,j,0) = (output(i,j,0) - mean) / std;
    }
  }
  return output;
}

// Pset07. obtain corner features from a list of corners
vector <Feature> computeFeatures(const Image &im, vector<Point> cornersL,
  float sigmaBlurDescriptor, float radiusDescriptor) {

  Image lumiBlurred = getBlurredLumi(im, sigmaBlurDescriptor);
  vector<Feature> features;
  for (int i = 0; i < (int) cornersL.size(); i++) {
      Point p = cornersL[i];
      Image descript = descriptor(lumiBlurred, p, radiusDescriptor);
      Feature f(p, descript);
      features.push_back(f);
  }
  return features;
}

// Pset07. SSD difference between features
float l2Features(Feature &f1, Feature &f2) {
  Image im1 = f1.desc();
  Image im2 = f2.desc();
  float val = 0, diff;
  for (int i = 0; i < im1.width(); i++) {
    for (int j = 0; j < im1.height(); j++) {
      diff = im1(i,j,0) - im2(i,j,0);
      diff *= diff;
      val += diff;
    }
  }
  return val;
}

vector <Correspondance> findCorrespondences(vector<Feature> listFeatures1, vector<Feature> listFeatures2, float threshold) {
    vector<Correspondance> cors;
    //Feature f1, f2, fbest;
    float min, secondMin, temp, score;
    float square_threshold = threshold*threshold;
    int list1Length = listFeatures1.size();
    int list2Length = listFeatures2.size();
    for(int i = 0; i < list1Length; i++) {
      Feature f1 = listFeatures1[i];
      Feature f2 = listFeatures2[0];
      min = l2Features(f1, f2);
      secondMin = min;
      Feature fbest = f2;
      for (int j = 1; j < list2Length; j++) {
        f2 = listFeatures2[j];
        temp = l2Features(f1, f2);
        if (temp < min) {
          secondMin = min;
          min = temp;
          fbest = f2;
        } else if (temp < secondMin) {
          secondMin = temp;
        }
      }
      score = secondMin / min;
      if (score > square_threshold) {
        Correspondance c(f1, fbest);
        cors.push_back(c);
      }
    }

  return cors;
}

// Pset07: Implement as part of RANSAC
// return a vector of bools the same size as listOfCorrespondences indicating
//  whether each correspondance is an inlier according to the homography H and threshold epsilon
vector<bool> inliers(Matrix H, vector <Correspondance> listOfCorrespondences, float epsilon) {
  vector<bool> bools;
  float points[2][3];
  float norm, diffx, diffy;
  float squareEpsilon = pow(epsilon, 2);
  int length = listOfCorrespondences.size();
  for (int i = 0; i < length; i++) {
    listOfCorrespondences[i].toListOfPairs(points);
    Matrix point1(1,3,points[0]);
    Matrix point2(1,3,points[1]);
    point1 = H.multiply(point1);
    point1 = homogenize(point1);
    // Norm
    diffx = (point1(0,0) - point2(0,0));
    diffy = (point1(0,1) - point2(0,1));
    norm = pow(diffx,2) + pow(diffy, 2);

    if (norm < squareEpsilon) {
      bools.push_back(true);
    } else {
      bools.push_back(false);
    }
  }
  return bools;
}

Matrix RANSAC(vector <Correspondance> listOfCorrespondences, int Niter, float epsilon) {
  float listOfPairs[4][2][3]; // keep this at the top of your code.
  float identity[] = {1, 0, 0,
                      0, 1, 0,
                      0, 0, 1};
  Matrix I(3,3,identity);
  int maxInliers = 0;
  vector<bool> inliersMax;
  Matrix HMax = I;
  for (int i = 0; i < Niter; i++) {
    vector<Correspondance> corrs = sampleCorrespondances(listOfCorrespondences);
    getListOfPairs(corrs, listOfPairs);
    Matrix H = I;
    try {
      H = computeHomography(listOfPairs);
    } catch(...) {
      cout << "Exception thrown." << endl;
      H = I;
    }
    vector<bool> inlies = inliers(H, listOfCorrespondences, epsilon);

    int countInliers = countBoolVec(inlies);
    if (countInliers > maxInliers) {
      maxInliers = countInliers;
      HMax = H;
      inliersMax = inlies;
    }
  }

  //cout << "The Homography that has the best match is: \n";
  //

   // for (int i = 0; i < (int) inliersMax.size(); i++) {
   //   cout << inliersMax[i] << endl;
   // }

  float listOfAllPairs[maxInliers][2][3];
  int index = 0;
  for (int i = 0; i < (int) inliersMax.size(); i++) {
    if (inliersMax[i]) {
      listOfCorrespondences[i].toListOfPairs(listOfAllPairs[index]);
      index++;
    }
  }
  
  // For the matrix system Ax = 0, x = H
  //  A^TA h = 0
  // H is the eigenvector with an eigenvalue of 0
  // When doing a least squares estimation, H is the eigenvector with the 
  // smallest eigenvalue
  // Alternatively, H is the eigenvector with the largest eigenvalue of the 
  // matrix (A^TA)^(-1)
 //  Matrix systm(9, 2*maxInliers);
 //  for (int i = 0; i < maxInliers; i++) {
 //    addConstraint9(systm, 2*i, listOfAllPairs[i]);
 //  }

 //  //cout << "The system that is constructed is:\n";
 // // systm.print();

 //  Matrix A = (systm.transpose()).multiply(systm);

 //  Matrix ATAInverse = A.inverse();

 //  // Get eigenvector corresponding to largest eigenvector
 //  // That is the homography matrix
 //  //ATAInverse.print();

 //  Matrix homoColumn(1,9);
 //  homoColumn(0,0) = HMax(0,0);
 //  homoColumn(0,1) = HMax(0,1);
 //  homoColumn(0,2) = HMax(0,2);

 //  homoColumn(0,3) = HMax(1,0);
 //  homoColumn(0,4) = HMax(1,1);
 //  homoColumn(0,5) = HMax(1,2);

 //  homoColumn(0,6) = HMax(2,0);
 //  homoColumn(0,7) = HMax(2,1);
 //  homoColumn(0,8) = HMax(2,2);
 //  Matrix x = leastSquares(ATAInverse, homoColumn);
 //  //x.print();
 //  Matrix result(3,3);
 //  result(0,0) = x(0,0) / x(0,8);
 //  result(1,0) = x(0,1)/ x(0,8);
 //  result(2,0) = x(0,2)/ x(0,8);
 //  result(0,1) = x(0,3)/ x(0,8);
 //  result(1,1) = x(0,4)/ x(0,8);
 //  result(2,1) = x(0,5)/ x(0,8);
 //  result(0,2) = x(0,6)/ x(0,8);
 //  result(1,2) = x(0,7)/ x(0,8);
 //  result(2,2) = x(0,8)/ x(0,8);

 //  result.print();
  return HMax;
}

Matrix leastSquares(Matrix A, Matrix b) {
  int NUM_SIMULATIONS = 10;
  //Matrix b(1, A.rows());

  // // init b to 1's
  // for (int i = 0; i < A.rows(); i++) {
  //   b(0, i) = 1;
  // }

  //b.print();

  for (int i = 0; i < NUM_SIMULATIONS; i++) {
    Matrix nextB = A.multiply(b);

    float norm = 0;
    for (int j = 0; j < A.rows(); j++) {
      norm += pow(nextB(0,j), 2);
    }
    norm = sqrt(norm);

    for (int j = 0; j < A.rows(); j++) {
      b(0,j) = nextB(0,j) / norm;
    }

  }

  for (int i = 0; i < A.rows(); i++) {
    b(0,i) = 1 / b(0,i);
  }
  return b;
}
Image autostitch(Image &im1, Image &im2, float blurDescriptor, float radiusDescriptor) {
  Matrix H = getH(im1, im2, blurDescriptor, radiusDescriptor);
  return stitch(im1, im2, H);
}

/******************************************************************************
 * Helpful optional functions to implement
 *****************************************************************************/

void addConstraint9(Matrix &systm,  int i, const float constr[2][3]) {
  float x, y, xp, yp;
  x = constr[0][0];
  xp = constr[1][0];
  y = constr[0][1];
  yp = constr[1][1];
  systm(0,i)   = x;
  systm(1,i)   = y;
  systm(2,i)   = 1;
  systm(3,i)   = 0;
  systm(4,i)   = 0;
  systm(5,i)   = 0;
  systm(6,i)   = -x*xp;
  systm(7,i)   = -y*xp;
  systm(8,i)   = -xp;
  systm(0,i+1) = 0;
  systm(1,i+1) = 0;
  systm(2,i+1) = 0;
  systm(3,i+1) = x;
  systm(4,i+1) = y;
  systm(5,i+1) = 1;
  systm(6,i+1) = -x*yp;
  systm(7,i+1) = -y*yp;
  systm(8,i+1) = -yp;
}

Image getBlurredLumi(const Image &im, float sigmaG) {
  vector<Image> lumChrom = lumiChromi(im);
  Image lumi = lumChrom[0];
  Image lumiBlurred = gaussianBlur_seperable(im, sigmaG);
  return lumiBlurred;
}

int countBoolVec(vector<bool> ins) {
  int count = 0;
  int size = ins.size();
  for (int i = 0; i < size; i++) {
    if (ins[i]) {
      count++;
    }
  }
  return count;
}


Matrix getH(Image &im1, Image &im2, float blurDescriptor, float radiusDescriptor) {
  vector<Point> h1 = HarrisCorners(im1);
  vector<Feature> f1 = computeFeatures(im1, h1, blurDescriptor, radiusDescriptor);
  vector<Point> h2 = HarrisCorners(im2);
  vector<Feature> f2 = computeFeatures(im2, h2, blurDescriptor, radiusDescriptor);

  // correspondances
  vector<Correspondance> corr = findCorrespondences(f1, f2);

  Matrix H = RANSAC(corr, 100);
  return H;
}
/******************************************************************************
 * Do Not Modify Below This Point
 *****************************************************************************/

 // Pset07 RANsac helper. re-shuffle a list of correspondances
 vector<Correspondance> sampleCorrespondances(vector <Correspondance> listOfCorrespondences) {
   random_shuffle(listOfCorrespondences.begin(), listOfCorrespondences.end());
   return listOfCorrespondences;
 }

 // Pset07 RANsac helper: go from 4 correspondances to a list of points [4][2][3] as used in Pset06.
 // Note: The function uses the first 4 correspondances passed
 void getListOfPairs(vector <Correspondance> listOfCorrespondences, array423 listOfPairs) {
   for (int i = 0; i < 4; i++)
     listOfCorrespondences[i].toListOfPairs(listOfPairs[i]);
 }

// Corner visualization.
Image visualizeCorners(const Image &im, vector<Point> pts, int rad, const vector <float> & color) {
  Image vim = im;
  for (int i = 0; i < (int) pts.size(); i++) {
    int px = pts[i].x;
    int py = pts[i].y;

    int minx = max(px - rad, 0);

    for (int delx = minx; delx < min(im.width(), px + rad + 1); delx++) {
      for (int dely = max(py - rad, 0); dely < min(im.height(), py + rad + 1); dely++) {

        if ( sqrt(pow(delx-px, 2) + pow(dely - py, 2)) <= rad) {

          for (int c = 0; c < im.channels(); c++) {
            vim(delx, dely, c) = color[c];
          }
        }
      }
    }
  }
  return vim;
}

Image visualizeFeatures(const Image &im, vector <Feature> LF, float radiusDescriptor) {
  // assumes desc are within image range
  Image vim = im;
  int rad = radiusDescriptor;

  for (int i = 0; i < (int) LF.size(); i++) {
    int px = LF[i].point().x;
    int py = LF[i].point().y;
    Image desc = LF[i].desc();

    for (int delx = px - rad; delx < px + rad + 1; delx++) {
      for (int dely = py - rad; dely < py + rad + 1; dely++) {
        vim(delx, dely, 0) = 0;
        vim(delx, dely, 1) = 0;
        vim(delx, dely, 2) = 0;

        if (desc(delx - (px-rad), dely - (py - rad)) > 0) {
          vim(delx, dely, 1) = 1;
        } else if (desc(delx - (px-rad), dely - (py - rad)) < 0) {
          vim(delx, dely, 0) = 1;
        }
      }
    }
  }
  return vim;
}

void drawLine(Point p1, Point p2, Image &im,  const vector<float> & color) {
  float minx = min(p1.x, p2.x);
  float miny = min(p1.y, p2.y);
  float maxx = max(p1.x, p2.x);
  float maxy = max(p1.y, p2.y);

  int spaces = 1000;
  for (int i = 0; i < spaces; i++) {
    float x = minx + (maxx - minx) / spaces * (i+1);
    float y = miny + (maxy - miny) / spaces * (i+1);
    for (int c = 0; c < im.channels(); c++) {
      im(x, y, c) = color[c];
    }
  }
}

Image visualizePairs(const Image &im1, const Image &im2, vector<Correspondance> corr) {
  Image vim(im1.width() + im2.width(), im1.height(), im1.channels());

  // stack the images
  for (int j = 0; j < im1.height(); j++) {
    for (int c = 0; c < im1.channels(); c++) {
      for (int i = 0; i < im1.width(); i++) {
        vim(i,j,c) = im1(i,j,c);
      }
      for (int i = 0; i < im2.width(); i++) {
        vim(i+im1.width(),j,c) = im2(i,j,c);
      }
    }
  }

  // draw lines
  for (int i = 0; i < (int) corr.size(); i++) {
    Point p1 = corr[i].feature(0).point();
    Point p2 = corr[i].feature(1).point();
    p2.x = p2.x + im1.width();
    drawLine(p1, p2, vim);
  }
  return vim;
}

Image visualizePairsWithInliers(const Image &im1, const Image &im2, vector<Correspondance> corr, const vector<bool> & ins) {
  Image vim(im1.width() + im2.width(), im1.height(), im1.channels());

  // stack the images
  for (int j = 0; j < im1.height(); j++) {
    for (int c = 0; c < im1.channels(); c++) {
      for (int i = 0; i < im1.width(); i++) {
        vim(i,j,c) = im1(i,j,c);
      }
      for (int i = 0; i < im2.width(); i++) {
        vim(i+im1.width(),j,c) = im2(i,j,c);
      }
    }
  }

  // draw lines
  vector<float> red(3,0);
  vector<float> green(3,0);
  red[0] = 1.0f;
  green[1]= 1.0f;

  for (int i = 0; i < (int) corr.size(); i++) {
    Point p1 = corr[i].feature(0).point();
    Point p2 = corr[i].feature(1).point();
    p2.x = p2.x + im1.width();
    if (ins[i]) {
      drawLine(p1, p2, vim, green);
    } else {
      drawLine(p1, p2, vim, red);
    }
  }
  return vim;

}

// Inliers:  Detected corners are in green, reprojected ones are in red
// Outliers: Detected corners are in yellow, reprojected ones are in blue
vector<Image> visualizeReprojection(const Image &im1, const Image &im2, Matrix  H, vector<Correspondance> & corr, const vector<bool> & ins) {
  // Initialize colors
  vector<float> red(3,0);
  vector<float> green(3,0);
  vector<float> blue(3,0);
  vector<float> yellow(3,0);
  red[0] = 1.0f;
  green[1]= 1.0f;
  blue[2] = 1.0f;
  yellow[0] = 1.0f;
  yellow[1] = 1.0f;

  vector<Point> detectedPts1In;
  vector<Point> projectedPts1In;
  vector<Point> detectedPts1Out;
  vector<Point> projectedPts1Out;

  vector<Point> detectedPts2In;
  vector<Point> projectedPts2In;
  vector<Point> detectedPts2Out;
  vector<Point> projectedPts2Out;

  for (int i = 0 ; i < (int) corr.size(); i++) {
    Point pt1 = corr[i].feature(0).point();
    Point pt2 = corr[i].feature(1).point();
    Matrix P1 = pt1.toHomogenousCoords();
    Matrix P2 = pt2.toHomogenousCoords();
    Matrix P2_proj = H.multiply(P1);
    Matrix P1_proj = H.inverse().multiply(P2);
    Point reproj1 = Point(P1_proj(0,0)/P1_proj(0,2), P1_proj(0,1)/P1_proj(0,2));
    Point reproj2 = Point(P2_proj(0,0)/P2_proj(0,2), P2_proj(0,1)/P2_proj(0,2));
    if (ins[i]) { // Inlier
      detectedPts1In.push_back(pt1);
      projectedPts1In.push_back(reproj1);
      detectedPts2In.push_back(pt2);
      projectedPts2In.push_back(reproj2);
    } else { // Outlier
      detectedPts1Out.push_back(pt1);
      projectedPts1Out.push_back(reproj1);
      detectedPts2Out.push_back(pt2);
      projectedPts2Out.push_back(reproj2);
    }
  }
  vector<Image> output;
  Image vim1(im1);
  Image vim2(im2);
  vim1 = visualizeCorners(im1, detectedPts1In,2, green);
  vim1 = visualizeCorners(vim1, projectedPts1In,1, red);
  vim1 = visualizeCorners(vim1, detectedPts1Out,2, yellow);
  vim1 = visualizeCorners(vim1, projectedPts1Out,1, blue);

  vim2 = visualizeCorners(im2, detectedPts2In,2, green);
  vim2 = visualizeCorners(vim2, projectedPts2In,1, red);
  vim2 = visualizeCorners(vim2, detectedPts2Out,2, yellow);
  vim2 = visualizeCorners(vim2, projectedPts2Out,1, blue);

  output.push_back(vim1);
  output.push_back(vim2);
  return output;
}

/***********************************************************************
 * Point and Feature Definitions *
 **********************************************************************/
Point::Point(int xp, int yp) {x = xp; y = yp;}
Point::Point() {x = 0; y = 0;}
Point::Point(const Point& other) { // cc
  x = other.x;
  y = other.y;
};
Point& Point::operator= (const Point& other) {
  x = other.x;
  y = other.y;
  return *this;
};
void Point::print() {printf("(%d, %d)\n", x, y); }

Matrix Point::toHomogenousCoords() {
  Matrix b(1, 3);
  b(0,0) = x;
  b(0,1) = y;
  b(0,2) = 1;
  return b;
}

// Feature Constructors
Feature::Feature(Point &ptp, Image &descp) {
  pt = new Point(ptp);
  dsc = new Image(descp);
}
Feature::Feature(const Feature& other) { // copy constructor
  pt = new Point(*other.pt);
  dsc = new Image(*other.dsc);
}
Feature& Feature::operator= (Feature& other) { // copy assignment operator
  pt = new Point(*other.pt);
  dsc = new Image(*other.dsc);
  return *this;
};

// getter functions
Point Feature::point() { return *pt;}
Image Feature::desc() { return *dsc;}

// printer
void Feature::print() {
  printf("Feature:");
  point().print();
  for (int j = 0; j < dsc->height(); j++) {
    for (int i = 0; i < dsc->width(); i++) {
      printf("%+07.2f ", (*dsc)(i, j));
    }
    printf("\n");
  }
}

// Correspondance Constructors
Correspondance::Correspondance(Feature &f1p, Feature &f2p) {
  f1 = new Feature(f1p);
  f2 = new Feature(f2p);
}
Correspondance::Correspondance(const Correspondance& other) { // copy constructor
  f1 = new Feature(*other.f1);
  f2 = new Feature(*other.f2);
}
Correspondance& Correspondance::operator= (const Correspondance& other) { // copy assignment operator
  f1 = new Feature(*other.f1);
  f2 = new Feature(*other.f2);
  return *this;
};
vector<Feature> Correspondance::features() {
  vector<Feature> feats;
  feats.push_back(*f1);
  feats.push_back(*f2);
  return feats;
}
Feature Correspondance::feature(int i) {
  if (i == 0)
    return (*f1);
  else
    return (*f2);
}
// printer
void Correspondance::print() {
  printf("Correspondance:");
  (*f1).print();
  (*f2).print();
}

void Correspondance::toListOfPairs(array23 arr) {
  arr[0][0] = (float) (*f1).point().x;
  arr[0][1] = (float) (*f1).point().y;
  arr[0][2] = 1;
  arr[1][0] = (float) (*f2).point().x;
  arr[1][1] = (float) (*f2).point().y;
  arr[1][2] = 1;
}
