#include "blending.h"
#include "matrix.h"
#include <math.h>
#include <ctime>

using namespace std;

/*****************************************************************************
 * blending related functions re-written from previous asasignments
 *****************************************************************************/

// PSet 08 instead of writing source in out, *add* the source to out based on the weight
// so out(x,y) = out(x, y) + weight * image
void applyhomographyBlend(const Image &source, const Image &weight, Image &out, Matrix &H, bool bilinear) {
}

// PSet 08 stitch using image weights.
// note there is no weight normalization.
Image stitchLinearBlending(const Image &im1, const Image &im2, const Image &we1, const Image &we2, Matrix H) {
  return Image(0);
}

/*****************************************************************************
 * blending functions Pset 08
 *****************************************************************************/

// PSet 08 weight image
// Single channel weight image
// Separable in x and y, weights are proportional to distance
//  from middle
Image blendingweight(int imwidth, int imheight) {
  float midX = (float) imwidth / 2.0;
  float midY = (float) imheight / 2.0;
  Image weights(imwidth, imheight, 1);
  for (int i = 0; i < imwidth; i++) {
    for (int j = 0; j < imheight; j++) {
      float x = (midX - fabs(midX -  (float) i)) / midX;
      float y = (midY - fabs(midY -  (float) j)) / midY;
      weights(i,j,0) = x*y;
    }
  }
  return weights;
}

// Optional: low freq and high freq (2-scale) decomposition
vector<Image> scaledecomp(const Image &im, float sigma) {
  vector <Image> ims;
  return ims;
}

// PSet 08 stitch using different blending models
// blend can be 0 (none), 1 (linear) or 2 (2-layer)
// use more helper functions if necessary
Image stitchBlending(Image &im1, Image &im2, Matrix H, int blend) {
  return Image(0);
}

// PSet 08 auto stitch
Image autostitch(Image &im1, Image &im2, int blend, float blurDescriptor, float radiusDescriptor) {
  return Image(0);
}

/************************************************************************
 * Coolness: mini planets.
 ************************************************************************/

Image pano2planet(const Image &pano, int newImSize, bool clamp) {
  return Image(0);
}


/************************************************************************
 * 6.865: Stitch N images into a panorama
 ************************************************************************/

// Pset08-865. Compute sequence of N-1 homographies going from Im_i to Im_{i+1}
// Implement me!
vector<Matrix> sequenceHs(vector<Image> ims, float blurDescriptor, float radiusDescriptor) {
  vector<Matrix> Hs;
  return Hs;
}

// Pset08-865. stack homographies:
//   transform a list of (N-1) homographies that go from I_i to I_i+1
//   to a list of N homographies going from I_i to I_refIndex.
vector <Matrix> stackHomographies(vector <Matrix> Hs, int refIndex) {
  vector <Matrix> gHs;
  return gHs;
}

// Pset08-865: compute bbox around N images given one main reference.
vector<float> bboxN(const vector<Matrix> &Hs, const vector<Image> &ims) {
  vector<float> ubbox;
  return ubbox;
}

// Pset08-865.
// Implement me!
Image autostitchN(vector<Image> ims, int refIndex, float blurDescriptor, float radiusDescriptor) {
  return Image(0);
}


/******************************************************************************
 * Helpful functions
 *****************************************************************************/

 // copy a single-channeled image to several channels
Image copychannels(const Image &im, int nChannels) {
 assert(im.channels() == 1, "image must have one channel");
 Image oim(im.width(), im.height(), nChannels);

 for (int i = 0; i < im.width(); i++) {
   for (int j = 0; j < im.height(); j++) {
     for (int c = 0; c < nChannels; c++) {
       oim(i, j, c) = im(i, j);
     }
   }
 }
 return oim;
}

// create an n x n identity matrix
Matrix eye(int n) {
  Matrix m(n, n);
  for (int i = 0; i < n; i++) m(i, i) = 1;
  return m;
}
