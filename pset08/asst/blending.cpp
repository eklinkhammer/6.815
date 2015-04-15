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
  int width = out.width();
  int height = out.height();

  Matrix inverseH = H.inverse();
  float m[3];
  m[2] = 1;
  float xPos, yPos, w;

  for (int i = 0; i < width; i++) {
    m[0] = i;
    for (int j = 0; j < height; j++) {
      m[1] = j;
      Matrix output(1,3,m);
      Matrix input = inverseH.multiply(output);
      input = homogenize(input);
      xPos = input(0,0);
      yPos = input(0,1);
      if (pixelInFrame(source, xPos, yPos)) {
        w = interpolateLin(weight, xPos, yPos, 0, bilinear);
        for (int c = 0; c < out.channels(); c++) {
          out(i,j,c) = out(i,j,c) + w*interpolateLin(source, xPos, yPos, c, bilinear);
        }
      }
    }
  }

}

// PSet 08 stitch using image weights.
// note there is no weight normalization.
Image stitchLinearBlending(const Image &im1, const Image &im2, const Image &we1, const Image &we2, Matrix H) {
  vector<float> bBox = getBBox(im1, im2, H);
  Matrix T = translate(bBox);
  Image output = create_output(T, bBox, im1.channels());

  Matrix TH = T.multiply(H);
  applyhomographyBlend(im2, we2, output, T, true);
  applyhomographyBlend(im1, we1, output, TH, true);
  return output;
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
  Image lowFreq = gaussianBlur_seperable(im, sigma);
  Image highFreq = im - lowFreq;
  ims.push_back(lowFreq);
  ims.push_back(highFreq);
  return ims;
}

// PSet 08 stitch using different blending models
// blend can be 0 (none), 1 (linear) or 2 (2-layer)
// use more helper functions if necessary
Image stitchBlending(Image &im1, Image &im2, Matrix H, int blend) {
  if (blend == 0) {
    return stitch(im1, im2, H);
  } else {
    Image w1 = blendingweight(im1.width(), im1.height());
    Image w2 = blendingweight(im2.width(), im2.height());
    Image sumOfWeights = stitchLinearBlending(w1, w2, w1*0+1.0, w2*0+1.0,H);
    if (blend == 1) {
      Image blended = stitchLinearBlending(im1, im2, w1, w2, H);
      Image out = safeDivide(blended, sumOfWeights);
      return out;
    } else {
      vector<Image> ims1 = scaledecomp(im1);
      vector<Image> ims2 = scaledecomp(im2);
      Image highFreq1 = ims1[1];
      Image highFreq2 = ims2[1];
      Image lowFreq1 = ims1[0];
      Image lowFreq2 = ims2[0];

      Image high1 = stitchLinearBlending(highFreq1, highFreq2, w1*0+1.0, w2*0, H);
      Image high2 = stitchLinearBlending(highFreq1, highFreq2, w1*0, w2*0+1.0, H);

      Image w1O = stitchLinearBlending(w1, w2, w1*0+1.0, w2*0, H);
      Image w2O = stitchLinearBlending(w1, w2, w1*0, w2*0+1.0, H);

      Image blendedLows = stitchLinearBlending(lowFreq1, lowFreq2, w1, w2, H);
      Image blendedLowNorm = safeDivide(blendedLows, sumOfWeights);

      float val;
      for (int i = 0; i < high2.width(); i++) {
        for (int j = 0; j < high2.height(); j++) {
          for (int c = 0; c < high2.channels(); c++) {
            val = w1O(i,j,0) > w2O(i,j,0) ? high1(i,j,c) : high2(i,j,c);
            blendedLowNorm(i,j,c) = blendedLowNorm(i,j,c) + val;
          }
        }
      }
      return blendedLowNorm;
    }
  }
}

Image safeDivide(Image &im1, Image &im2) {
  int width = im1.width();
  int height = im1.height();
  int channels = im1.channels();
  Image out(width, height, channels);
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      for (int c = 0; c < channels; c++) {
        if (im2(i,j) == 0) {
          out(i,j,c) = 0;
        } else {
          out(i,j,c) = im1(i,j,c) / im2(i,j);
        }
      }
    }
  }
  return out;
}
// PSet 08 auto stitch
Image autostitch(Image &im1, Image &im2, int blend, float blurDescriptor, float radiusDescriptor) {
  Matrix H = getH(im1, im2, blurDescriptor, radiusDescriptor);
  return stitchBlending(im1, im2, H, blend);
}

/************************************************************************
 * Coolness: mini planets.
 ************************************************************************/

Image pano2planet(const Image &pano, int newImSize, bool clamp) {
  Image out(newImSize, newImSize, pano.channels());
  float mid = (float) newImSize / 2.0;
  float rmax = sqrt(2)*mid;
  for (int i = 0; i < newImSize; i++) {
    for (int j = 0; j < newImSize; j++) {
      float newX = (float) i - mid;
      float newY = mid - (float) j;
      float theta = atan2(newY, newX);
      float r = newY*newY + newX*newX;
      r = sqrt(r);

      // we have the r's and theta's of the output
      // now, we scale the r to the size of the output
      // theta = 0 -> right side -> xPan = w
      // theta = pi -> center -> xPan = w/2
      // (1 - theta / pi) w/2 + w/2 

      // theta = -pi -> center -> xPan = w/2
      // theta = -0 -> left side -> xPan = 0
      // (theta / pi) w/2

      //size of r is height of image (from bottom)
      // r = 0 -> y = h
      // r = max -> y = 0
      // r = 1/2 max -> y = 1/2 max
      float xPan;
      int panWidth = pano.width();
      if (theta > 0) {
        xPan = (1 - theta / pi) * (panWidth / 2) + panWidth / 2;
      } else {
        xPan =  -1* (theta / pi) * (panWidth / 2);
      }
      float yPan = (1 - r / rmax) * pano.height();
      // cout << "For output values " << i << ", " << j << " in an image of size " << newImSize << endl;
      // cout << "newX: " << newX << "\tnewY: " << newY << endl;
      // cout << "theta: " << theta << "\tr: " << r << endl;
      // cout << "xPan: " << xPan << "\tyPan: " << yPan << " in an image of size (" << pano.width() << ", " << pano.height() << endl << endl;;
      for (int c = 0; c < pano.channels(); c++) {
        out(i,j,c) = interpolateLin(pano, xPan, yPan, c, true);
      }
    }
  }
  return out;
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

Image create_output(Matrix T, vector<float> bBox, int channels) {
  float tx = T(2,0);
  float ty = T(2,1);

  int im3Height = bBox[3] + (int) ty;
  int im3Width = bBox[1] + (int) tx;
  Image im3(im3Width, im3Height, channels);
  return im3;
}

vector<float> getBBox(const Image &im1, const Image &im2, Matrix H) {
  vector<float> bbox1 = computeTransformedBBox(im1.width(),im1.height(),H);
  vector<float> bbox2;
  bbox2.push_back(0.0);
  bbox2.push_back( (float)(im2.width() - 1) );
  bbox2.push_back(0.0);
  bbox2.push_back( (float)(im2.height() - 1) );

  vector<float> bBox = bboxUnion(bbox1, bbox2);
  return bBox;
}

