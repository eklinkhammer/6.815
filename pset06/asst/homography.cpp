#include "homography.h"
#include "matrix.h"

using namespace std;


// PS06: apply homography
void applyhomography(const Image &source, Image &out, Matrix &H, bool bilinear) {
  int width = out.width();
  int height = out.height();

  Matrix inverseH = H.inverse();
  float m[3];
  m[2] = 1;
  float xPos, yPos;

  // For all output pixels, find the corresponding homography pixels
  // If those pixels exist in the source image, replace the output ones
  for (int i = 0; i < width; i++) {
    m[0] = i;
    for (int j = 0; j < height; j++) {
      m[1] = j;
      Matrix output(1,3,m);
      Matrix input = inverseH.multiply(output);
      xPos = input(0,0) / input(0,2);
      yPos = input(0,1) / input(0,2);
      if (pixelInFrame(source, xPos, yPos)) {
        copyAllChannels(source, out, xPos, yPos, i, j, bilinear);
      }
    }
  }
}

// PS06: 6.865 or extra credit: apply homography fast
void applyhomographyFast(const Image &source, Image &out, Matrix &H, bool bilinear) {
  // do something :)

  // Hint: a lot of the work is similar to applyhomography(). You may want to
  // write a small helpful function to avoid duplicating code.
}

// PS06: compute homography given a list of point pairs
Matrix computeHomography(const float listOfPairs[4][2][3]) {
  /* 
  xa + yb + c = x'(xg + xh + i) => xa + yb + c - x'xg - x'yh - x'i = 0
  xd + ye + f = y'(xg + yh + i) => xd + ye + f - y'xg - y'yh - y'i = 0

  Letting i = 0
  xa + yb + c - x'xg - x'yh = x'
  xd + ye + f - y'xg - y'yh = y'

  Ax = b
  | x y 1 0 0 0 -xx' -yx' | |a|   |x'| 
  | 0 0 0 x y 1 -xy' -yy' | |b|   |y'|
  | x y 1 0 0 0 -xx' -yx' | |c|   |x'|
  | 0 0 0 x y 1 -xy' -yy' | |d| = |y'|
  | x y 1 0 0 0 -xx' -yx' | |e|   |x'|
  | 0 0 0 x y 1 -xy' -yy' | |f|   |y'|
  | x y 1 0 0 0 -xx' -yx' | |g|   |x'|
  | 0 0 0 x y 1 -xy' -yy' | |h|   |y'|

  x = inverse(A)*b
  */
  Matrix systm(8,8);
  for (int i = 0; i < 4; i++) {
    addConstraint(systm, i*2, listOfPairs[i]);
  }

  Matrix b (1,8);
  b(0,0) = listOfPairs[0][1][0];
  b(0,1) = listOfPairs[0][1][1];

  b(0,2) = listOfPairs[1][1][0];
  b(0,3) = listOfPairs[1][1][1];

  b(0,4) = listOfPairs[2][1][0];
  b(0,5) = listOfPairs[2][1][1];

  b(0,6) = listOfPairs[3][1][0];
  b(0,7) = listOfPairs[3][1][1];
  Matrix x = systm.inverse().multiply(b);
  Matrix result (3,3);
  result(0,0) = x(0,0);
  result(1,0) = x(0,1);
  result(2,0) = x(0,2);
  result(0,1) = x(0,3);
  result(1,1) = x(0,4);
  result(2,1) = x(0,5);
  result(0,2) = x(0,6);
  result(1,2) = x(0,7);
  result(2,2) = 1;
  return result;
}

// PS06: optional function that might help in computeHomography
// i is the row that the constraints will be added
// i must be an even number
void addConstraint(Matrix &systm,  int i, const float constr[2][3]) {
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
  systm(0,i+1) = 0;
  systm(1,i+1) = 0;
  systm(2,i+1) = 0;
  systm(3,i+1) = x;
  systm(4,i+1) = y;
  systm(5,i+1) = 1;
  systm(6,i+1) = -x*yp;
  systm(7,i+1) = -y*yp;
}

// PS06: compute a transformed bounding box
// returns [xmin xmax ymin ymax]
vector<float> computeTransformedBBox(int imwidth, int imheight, Matrix H) {
  // fix me!
  float[] topLeft = {0,0,1};
  float[] topRight = {0,imwidth-1,1};
  float[]
  Matrix output(1,3,m);
  vector <float> minmax;
  return minmax;
}

// PS06: homogenize vector v.
// this function is not required, but would be useful for use in
// computeTransformedBBox()
Matrix homogenize(Matrix &v) {
  // TODO: homogenize vector v here.
  return Matrix(1, 3); // fix me
}

// PS06: compute a 3x3 translation Matrix
Matrix translate(vector<float> B) {
  return Matrix(3, 3); // fix me
}

// PS06: compute the union of two bounding boxes
vector <float> bboxUnion(vector<float> B1, vector<float> B2) {
  vector<float> B;
  return B; // fix me
}

// PS06: stitch two images given a list or 4 point pairs
Image stitch(const Image &im1, const Image &im2, const float listOfPairs[4][2][3]) {
  return Image(0); // fix me!
}

// PS06: useful for bounding-box debugging.
Image drawBoundingBox(const Image &im, vector<float> minmax) {
  return Image(0); // fix me!
}


/***********************************************************************
 * Do not edit code  below this line
 **********************************************************************/
// get the minimum vector element
float min_vec_elem(vector<float> v) {
  float mmin = FLT_MAX;
  for (int i = 0; i < (int)v.size(); i++) mmin = min(mmin, v[i]);
  return mmin;
}

// get the maximum vector element
float max_vec_elem(vector<float> v) {
  float mmax = -FLT_MAX;
  for (int i = 0; i < (int)v.size(); i++) mmax = max(mmax, v[i]);
  return mmax;
}
