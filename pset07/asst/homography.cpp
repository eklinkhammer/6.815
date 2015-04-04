#include "homography.h"
#include "matrix.h"

using namespace std;


// PS06: apply homography
void applyhomography(const Image &source, Image &out, Matrix &H, bool bilinear) {
  // do something :)
}

// PS06: 6.865 or extra credit: apply homography fast
void applyhomographyFast(const Image &source, Image &out, Matrix &H, bool bilinear) {
  // do something :)

  // Hint: a lot of the work is similar to applyhomography(). You may want to
  // write a small helpful function to avoid duplicating code.
}

// PS06: compute homography given a list of point pairs
Matrix computeHomography(const float listOfPairs[4][2][3]) {
  return Matrix(3, 3); // fix me!
}

// PS06: optional function that might help in computeHomography
void addConstraint(Matrix &systm,  int i, const float constr[2][3]) {}

// PS06: compute a transformed bounding box
// returns [xmin xmax ymin ymax]
vector<float> computeTransformedBBox(int imwidth, int imheight, Matrix H) {
  // fix me!
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
