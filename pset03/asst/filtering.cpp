// filtering.cpp
// Assignment 3


#include "filtering.h"
#include <math.h>

using namespace std;

/**************************************************************
 //            IMAGE CONVOLUTION AND FILTERING               //
 *************************************************************/


// PS03 - 2.1.1 - convolve an image with a box filter of size k by k
Image boxBlur(const Image &im, const int &k, bool clamp) {
    int height, width, channels;
    height = im.height(); width = im.width(); channels = im.channels();
    Image output = Image(width, height, channels);
    
    int k_over_2 = floor(k/2);
    // Loop through output image
    int i, j, c;
    for (i = 0; i < width; i++) {
      for (j = 0; j < height; j++) {
        for (c = 0; c < channels; c++) {
          int m, n;
          // Loop through box
          float val = 0;
          for (m = i - k_over_2; m <= i + k_over_2; m++) {
            for (n = j - k_over_2; n <= j + k_over_2; n++) {
              val += im.smartAccessor(m, n, c, true);
            }
          }
          val /= (k*k);
          output(i,j,c) = val;
        }
      }
    }
    return output;
}


// PS03 - 2.2.2 - reimeplement the box filter using the filter class.
// check that your results math those in the previous function "boxBlur"
Image boxBlur_filterClass(const Image &im, const int &k, bool clamp) {
 
    float fData[(k*k)];
    int i;
    for (i = 0; i < k*k; i++) {
      fData[i] = 1.0/(k*k);
    }
    vector<float> fDataVector (fData, fData + sizeof(fData) / sizeof(float));
    Filter boxBlur(fDataVector, k, k);
    Image output = boxBlur.Convolve(im, clamp);
    return output;
}


// PS03 - 2.3.1 - uses a Sobel kernel to compute the horizontal and vertical
// components of the gradient of an image and returns the gradient magnitude.
Image gradientMagnitude(const Image &im, bool clamp){
    // Horizontal Sobel Filter
    // [ -1 0 1 ]
    // [ -2 0 2 ]
    // [ -1 0 1 ]
    float fData[] = { -1.0, 0.0, 1.0, -2.0, 0.0, 2.0, -1.0, 0.0, 1.0 };
    vector<float> fDataX (fData, fData + sizeof(fData) / sizeof(float) );
    Filter sobelX(fDataX, 3, 3);

    // Vertical Sobel Filter
    // [ -1 -2 -1 ]
    // [  0  0  0 ]
    // [  1  2  1 ]
    float fDataV[] = { -1.0, -2.0, -1.0, 0.0, 0.0, 0.0, 1.0, 2.0, 1.0 };
    vector<float> fDataY (fDataV, fDataV + sizeof(fDataV) / sizeof(float) );
    Filter sobelY(fDataY, 3, 3);

    Image hGrad = sobelX.Convolve(im, clamp);
    Image vGrad = sobelY.Convolve(im, clamp);

    int width, height, channels, i, j, k;
    width = im.width(); height = im.height(); channels = im.channels();
    Image output(width, height, channels);

    // Loop through output, each pixel is the magnitude of the hGrad and vGrad for that color
    for (i = 0; i < width; i++) {
      for (j = 0; j < height; j++) {
        for (k = 0; k < channels; k++) {
          float vert = vGrad(i,j,k);
          vert = pow(vert,2);
          float horz = hGrad(i,j,k);
          horz = pow(horz,2);
          output(i,j,k) = sqrt( (vert + horz) );
        }
      }
    }
    return output;
    
}

// PS03 - 2.4.1 - create a vector containing the normalized values in a 1D Gaussian filter
vector<float> gauss1DFilterValues(float sigma, float truncate){
    int len = 1 + 2*ceil(sigma * truncate);
    vector<float> gauss1D (len, 0);
    // f(x) = 1 / (\sigma * sqrt(2 pi) ) * e^(x^2 / 2\sigma^2)
    float root2PiSigma = sqrt( (2*M_PI) ) * sigma;
    
    // Every value is shifted so middle is at 1 + ceil(sigma*truncate)
    int x;
    for (x = 0; x < len; x++) {
      int gaussIndex = x - ceil(sigma*truncate);
      double exponent = pow(gaussIndex, 2) / (2 * pow(sigma, 2));
      double result = exp((-1*exponent)) / root2PiSigma;
      gauss1D[x] = result;
    }
    return gauss1D;
}

// PS03 - 2.4.2 - blur across the rows of an image
Image gaussianBlur_horizontal(const Image &im, float sigma, float truncate, bool clamp){
    int width = 1 + 2*ceil(sigma*truncate);
    int height = 1;
    vector<float> gauss1D = gauss1DFilterValues(sigma, truncate);
    Filter gauss(gauss1D, width, height);
    Image output = gauss.Convolve(im, clamp);
    return output;
}

// PS03 - 2.4.3 - create a vector containing the normalized values in a 2D Gaussian filter
vector<float> gauss2DFilterValues(float sigma, float truncate){
    int len = 1 + 2*ceil(sigma * truncate);
    vector<float> gauss2D (len*len, 0);
    float sum = 0;
    // f(x) = 1 / (\sigma * sqrt(2 pi) ) * e^(x^2 +y^2 / 2\sigma^2)
    float PiSigma2 = 2*M_PI * pow(sigma, 2);
    float ceilSigmaTruncate = ceil(sigma*truncate);
    // Every value is shifted so middle is at 1 + ceilSigmaTruncate, 1 + ceilSigmaTruncate
    int x, y;
    for (x = 0; x < len; x++) {
      int gaussX = x - ceilSigmaTruncate;
      for (y = 0; y < len; y++) {
        int gaussY = y - ceilSigmaTruncate;
        double exponent = ( pow(gaussX, 2) + pow(gaussY, 2) ) / (2 * pow(sigma,2));
        exponent *= -1;
        double result = exp(exponent) / PiSigma2;
        int index = x + y*len;
        gauss2D[index] = result;
        printf("%1.3f\t",result);
        sum += result;
      }
    }
    return gauss2D;
}


// PS03 - 2.4.4 - Blur an image with a full  full 2D rotationally symmetric Gaussian kernel
Image gaussianBlur_2D(const Image &im, float sigma, float truncate, bool clamp){
    int len = 1 + 2*ceil(sigma * truncate);
    len *= len;
    int width = 1 + 2*ceil(sigma*truncate);
    int height = width;
    vector<float> gauss2D = gauss2DFilterValues(sigma, truncate);
    Filter gauss(gauss2D, width, height);
    Image output = gauss.Convolve(im, clamp);
    return output;
}

// PS03 - 2.4.5 - Use principles of seperabiltity to blur an image using 2 1D Gaussian Filters
Image gaussianBlur_seperable(const Image &im, float sigma, float truncate, bool clamp){
    int len = 1 + ceil(sigma*truncate);
    int width = 1 + 2*ceil(sigma*truncate);
    int height = 1;
    vector<float> gauss1D = gauss1DFilterValues(sigma, truncate);
    Filter gaussX(gauss1D, width, height);
    Filter gaussY(gauss1D, height, width);
    Image filteredX = gaussX.Convolve(im, clamp);
    Image output = gaussY.Convolve(filteredX, clamp);
    return output;
}


// PS03 - 2.5.1 - sharpen an image
Image unsharpMask(const Image &im, float sigma, float truncate, float strength, bool clamp){
    Image blurred = gaussianBlur_seperable(im, sigma, truncate, clamp);
    int i, j, k;
    Image output(im.width(), im.height(), im.channels());
    // subtract blurred from im, then add that result to im strength times
    for (i = 0; i < im.width(); i++) {
      for (j = 0; j < im.height(); j++) {
        for (k = 0; k < im.channels(); k++) {
          float detail = im(i,j,k) - blurred(i,j,k);
          output(i,j,k) = im(i,j,k) + strength*detail;
        }
      }
    }
    return output;
    
}


// PS03 - 3.0.1 -  Denoise an image using bilateral filtering
Image bilateral(const Image &im, float sigmaRange, float sigmaDomain, float truncateDomain, bool clamp){
    int width, height, channels;
    int len = 1 + 2*ceil(sigmaDomain*truncateDomain);
    width = im.width(); height = im.height(); channels = im.channels();
    Image output(width, height, channels);
    float sigmaD2, sigmaR2;
    sigmaD2 = pow(sigmaDomain, 2); 
    sigmaR2 = pow(sigmaRange, 2);

    // Loop through output
    int i, j, k, l, c, kShifted, lShifted, shift;
    shift = ceil(sigmaDomain*truncateDomain);
    float weightDomain, weightRange, weight, weightSum, acc, val;
    for (i = 0; i < width; i++) {
      for (j = 0; j < height; j++) {
        for (c = 0; c < channels; c++) {
          // Loop through kernel
          acc = 0;
          weightSum = 0;
          for (k = 0; k < len; k++) {
            for (l = 0; l < len; l++) {
              // Want values of image centered on middle of filter.
              // Shift is so that 0,0 is the middle
              // Adding i (or j) moves the filter to the target pixel
              kShifted = i + k - shift;
              lShifted = j + k - shift;
              //cout << "Reached to access." << endl;
              val = im.smartAccessor(kShifted, lShifted, c, clamp);
              //cout << "Passed access." << endl;
              weightDomain = weightD(i,j, kShifted, lShifted, sigmaD2);
              weightRange = weightR(im, i, j, val, c, sigmaR2);
              weight = exp(weightDomain + weightRange);
              weightSum += weight;
              acc += weight * val;
            }
          }
          output(i,j,c) = acc / weightSum;
        }
      }
    }
    return output; // change this
}

float weightD(int i, int j, int k, int l, float sigmaD2) {
  float x1, y1;
  x1 = i - k;
  y1 = j - l;
  float x_domain = -1 * (pow(x1, 2) + pow(y1,2) ) / (2 * sigmaD2);
  return x_domain;
}

float weightR(const Image &im, int i, int j, float val, int c, float sigmaR2) {
  float imageDiff = im(i,j,c) - val;
  imageDiff = pow(imageDiff, 2);
  float x_range = -1 * imageDiff / (2 * sigmaR2);
  return x_range;
}

// PS03 - 3.1.1 - 6.865 only: Bilaterial Filter an image seperatly for
// the Y and UV components of an image
Image bilaYUV(const Image &im, float sigmaRange, float sigmaY, float sigmaUV, float truncateDomain, bool clamp){
    int width, height, channels;
    int len;
    int lenY = 1 + 2*ceil(sigmaY*truncateDomain);
    int lenUV = 1 + 2*ceil(sigmaUV*truncateDomain);
    width = im.width(); height = im.height(); channels = im.channels();
    Image output(width, height, channels);
    float sigmaD2, sigmaR2, sigmaDY2, sigmaDUV2;
    sigmaD2 = pow(sigmaUV, 2); 
    sigmaR2 = pow(sigmaRange, 2);
    sigmaDY2 = pow(sigmaY, 2);
    // Loop through output
    int i, j, k, l, c, kShifted, lShifted, shift, shiftUV, shiftY;
    shiftUV = ceil(sigmaUV*truncateDomain);
    shiftY = ceil(sigmaY*truncateDomain);

    float weightDomain, weightRange, weight, weightSum, acc, val;
    for (i = 0; i < width; i++) {
      for (j = 0; j < height; j++) {
        for (c = 0; c < channels; c++) {
          // Loop through kernel
          acc = 0;
          weightSum = 0;
          shift = (c == 0) ? shiftY : shiftUV;
          sigmaD2 = (c == 0) ? sigmaDY2 : sigmaD2;
          len = ( c == 0 ) ? lenY : lenUV;
          for (k = 0; k < len; k++) {
            for (l = 0; l < len; l++) {
              // Want values of image centered on middle of filter.
              // Shift is so that 0,0 is the middle
              // Adding i (or j) moves the filter to the target pixel
              kShifted = i + k - shift;
              lShifted = j + k - shift;
              //cout << "Reached to access." << endl;
              val = im.smartAccessor(kShifted, lShifted, c, clamp);
              //cout << "Passed access." << endl;
              weightDomain = weightD(i,j, kShifted, lShifted, sigmaD2);
              weightRange = weightR(im, i, j, val, c, sigmaR2);
              weight = exp(weightDomain + weightRange);
              weightSum += weight;
              acc += weight * val;
            }
          }
          output(i,j,c) = acc / weightSum;
        }
      }
    }
    return output; // change this
}


/**************************************************************
 //                 FILTER CLASS FUNCTIONS                  //
 *************************************************************/


// PS03 - 2.2.1 - write a convolution function for the filter class
Image Filter::Convolve(const Image &im, bool clamp){
    int iWidth, iHeight, iChannels;
    iWidth = im.width(); iHeight = im.height(); iChannels = im.channels();
    Image output = Image(iWidth, iHeight, iChannels);
    // values of k/2
    int wDiv2 = floor(width / 2);
    int hDiv2 = floor(height / 2);
    // Loop through output pixels
    int i, j, c;
    for (i = 0; i < iWidth; i++) {
      for (j = 0; j < iHeight; j++) {
        for (c = 0; c < iChannels; c++) {
          float acc = 0;
          // Loop through kernel
          // Loop indices from (0,0) to (k-1, k-1). Shift everything by -k/2
          int m, n;
          for (m = 0; m < width; m++) {
            for (n = 0; n < height; n++) {
              int mprime = m - wDiv2;
              int nprime = n - hDiv2;
              int imI = i - mprime;
              int imJ = j - nprime;
              // k(m,n)*I(i - m, j - n)
              //cout << "Smart Access: " << im.smartAccessor(wDiv2, hDiv2, c, clamp);
              acc += (float) (operator()(m,n)) * im.smartAccessor(imI, imJ, c, clamp);
              //cout << "Operator value: " << operator()(m,n)<< endl;
              //cout << im.smartAccessor(imI, imJ, c, clamp) << endl;
            }
          }
          output(i,j,c) = acc;
        }
      }
    }
    return output;
}


/**************************************************************
 //               DON'T EDIT BELOW THIS LINE                //
 *************************************************************/

// Create an image of 0's with a value of 1 in the middle. This function
// can be used to test that you have properly set the kernel values in your
// Filter object. Make sure to set k to be larger than the size of your kernel
Image impulseImg(const int &k){
    
    // initlize a kxkx1 image of all 0's
    Image impulse(k, k, 1);
    
    // set the center pixel to have intensity 1
    int center = floor(k/2);
    impulse(center,center,0) = 1;
    
    return impulse;
}


// Filter class constructor
Filter::Filter(const vector<float> &fData, const int &fWidth, const int &fHeight) {
    
    kernel = fData;
    width = fWidth;
    height = fHeight;
    
}

Filter::Filter(const int &fWidth, const int &fHeight) {
  width = fWidth;
  height = fHeight;
  kernel = std::vector<float>(width*height,0);
}

const float & Filter::operator()(int x, int y) const {
    if (x < 0 || x >= width)
        throw OutOfBoundsException();
    if ( y < 0 || y >= height)
        throw OutOfBoundsException();
    
    return kernel[x +y*width];
    
}


float & Filter::operator()(int x, int y) {
    if (x < 0 || x >= width)
        throw OutOfBoundsException();
    if ( y < 0 || y >= height)
        throw OutOfBoundsException();
    
    return kernel[x +y*width];
}


Filter::~Filter() {}
