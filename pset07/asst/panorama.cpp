#include "panorama.h"
#include "matrix.h"
#include <unistd.h>
#include <ctime>

using namespace std;

// Pset 07. Compute xx/xy/yy Tensor of an image.
Image computeTensor(const Image &im, float sigmaG, float factorSigma) {
  return Image(0);
}

// Pset07. Compute Harris Corners.
vector<Point> HarrisCorners(const Image &im, float k, float sigmaG, float factorSigma, float maxiDiam, float boundarySize) {
  return vector<Point>();
}

// Pset07. Compute response = det(M) - k*[(trace(M)^2)]
Image cornerResponse(const Image &im, float k, float sigmaG, float factorSigma) {
  return Image(0);
}

// Pset07. Descriptor helper function
Image descriptor(const Image &blurredIm, Point p, float radiusDescriptor) {
  return Image(0);
}

// Pset07. obtain corner features from a list of corners
vector <Feature> computeFeatures(const Image &im, vector<Point> cornersL,
  float sigmaBlurDescriptor, float radiusDescriptor) {
  return vector <Feature>();
}

// Pset07. SSD difference between features
float l2Features(Feature &f1, Feature &f2) {
  return 0;
}

vector <Correspondance> findCorrespondences(vector<Feature> listFeatures1, vector<Feature> listFeatures2, float threshold) {
  return vector <Correspondance>();
}

// Pset07: Implement as part of RANSAC
// return a vector of bools the same size as listOfCorrespondences indicating
//  whether each correspondance is an inlier according to the homography H and threshold epsilon
vector<bool> inliers(Matrix H, vector <Correspondance> listOfCorrespondences, float epsilon) {
  return vector<bool>();
}

Matrix RANSAC(vector <Correspondance> listOfCorrespondences, int Niter, float epsilon) {
  float listOfPairs[4][2][3]; // keep this at the top of your code.

  // RANSAC loop. Note: this can take a while.
      // getListOfPairs(corr, listOfPairs); // Call this line inside of your RANsac for loop

  return Matrix(0, 0);
}

Image autostitch(Image &im1, Image &im2, float blurDescriptor, float radiusDescriptor) {
  return Image(0);
}

/******************************************************************************
 * Helpful optional functions to implement
 *****************************************************************************/

Image getBlurredLumi(const Image &im, float sigmaG) {
  return Image(0);
}

int countBoolVec(vector<bool> ins) {
  return 0;
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
