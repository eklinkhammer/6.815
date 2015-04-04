// basicImageManipulation.cpp
// Assignment 2

#include "basicImageManipulation.h"
using namespace std;

// PS02 - 3.1.1: create a new image that is k times bigger than the input by using nearest neighbor interpolation
Image scaleNN(const Image &im, const float &factor){
    Image output = Image(floor(im.width()*factor), floor(im.height()*factor), im.channels());
    int w = output.width();
    int h = output.height();
    int i, j, k, x, y;
    for (i = 0; i < w; i++) {
      for (j = 0; j < h; j++) {
        for (k = 0; k < im.channels(); k++) {
          x = round((float)i/factor);
          y = round((float)j/factor);
          output(i,j,k) = im(x,y,k);
        }
      }
    }
    return output;
}

// PS02 - 3.2.1: using bilinear interpolation to assign the value of a location from its neighboring pixel values
float interpolateLin(const Image &im, float x, float y, int z, bool clamp){
    float rightFactor = x - floor(x);
    float leftFactor = 1 - rightFactor;
    float bottomFactor = y - floor(y);
    float topFactor = 1 - bottomFactor;
    // Interpolate across x - top row
    float topRow = leftFactor*im.smartAccessor(floor(x), floor(y), z, clamp) + rightFactor*im.smartAccessor(ceil(x), floor(y), z, clamp);
    // bottom row (increasing y)
    float bottomRow = leftFactor*im.smartAccessor(floor(x), ceil(y),z,clamp) + rightFactor*im.smartAccessor(ceil(x), ceil(y), z, clamp);
    // interpolate across y
    float interp = topFactor*topRow + bottomFactor * bottomRow;
    return interp; 
}

// PS02 - 3.2.2: create a new image that is k times bigger than the input by using bilinear interpolation
Image scaleLin(const Image &im, const float &factor){
    Image output = Image(floor(im.width()*factor), floor(im.height()*factor),im.channels());
    int i,j,k;
    float x, y;
    for (i = 0; i < output.width(); i++) {
      for (j = 0; j < output.height(); j++) {
        x = (float) i / factor;
        y = (float) j / factor;
        for (k = 0; k < output.channels(); k++) {
          output(i,j,k) = interpolateLin(im, x, y, k, true);
        }
      }
    }
    return output;
}


// PS02 - 3.3.1 (6.865 required, 6.815 extra credit): rotate an image around its center by theta
Image rotate(const Image &im, const float &theta) {
    // center around which to rotate. Use this center coordinate.
    float centerX = (im.width()-1.0)/2.0;
    float centerY = (im.height()-1.0)/2.0;
    // R = rotation matrix
    // xf = cos x - sin y
    // yf = sin x + cos y
    
    // Translate points first
    float c = cos(theta);
    float s = sin(theta); 
    
    float xt, yt, xr, yr, xf, yf;

    Image output = Image(im.width(), im.height(),im.channels());
    int i,j,k;
    for (i = 0; i < output.width();i++) {
      for (j = 0; j < output.height(); j++) {
        xt = i - centerX;
        yt = j - centerY;
        xr = xt * c - s * yt;
        yr = s * xt + c * yt;
        xf = xr + centerX;
        yf = yr + centerY;
        for (k = 0; k < output.channels(); k++) {
          output(i,j,k) = im.smartAccessor(xf,yf,k,false);
        }
      }
    }
    return output;
}


/**************************************************************
 //               DON'T EDIT BELOW THIS LINE                //
 //          Functions from previous problem sets           //
 *************************************************************/

Image brightness(const Image &im, const float &factor) {
  return im * factor;
}

Image contrast(const Image &im, const float &factor, const float &midpoint) {
  return (im - midpoint)*factor+midpoint;
}
