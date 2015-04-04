// hdr.cpp
// Assignment 5


#include "hdr.h"
#include "filtering.h"
#include <math.h>
#include <algorithm>
#include <assert.h>


using namespace std;

/**************************************************************
 //                       HDR MERGING                        //
 *************************************************************/

// PS05 - 2.1 - generate a weight image that indicates which pixels are good to use in hdr
Image computeWeight(const Image &im, float epsilonMini, float epsilonMaxi){

  // Init output image
  int height, width, channels;
  width = im.width();
  height = im.height();
  channels = im.channels();
  Image output(width, height, channels);

  int i, j, k;
  float imageVal;
  for (i = 0; i < width; i++) {
    for (j = 0; j < height; j++) {
      for (k = 0; k < channels; k++) {
        imageVal = im(i,j,k);
        output(i,j,k) = (imageVal >= epsilonMini && imageVal <= epsilonMaxi) ? 1 : 0;
      }
    }
  }

  return output;
}

// PS05 - 2.2 - compute the multiplication factor between a pair of images
float computeFactor(const Image &im1, const Image &w1, const Image &im2, const Image &w2){
    
  // Get image data
  int height, width, channels;
  width = im1.width();
  height = im1.height();
  channels = im1.channels();

  // Asserts!!!
  assert(width == w1.width() && width == w2.width() && width == im2.width());
  assert(height == w1.height() && height == w2.height() && height == im2.height());
  assert(channels == w1.channels() && channels == w2.channels() && channels == im2.channels());

  vector<float> ratios;

  int i,j,k, numValid = 0;
  float im1Val;
  float miniEps = pow(10,(-9));
  for (i = 0; i < width; i++) {
    for (j = 0; j < height; j++) {
      for (k = 0; k < channels; k++) {
        if (w1(i,j,k) > 0.99999 && w2(i,j,k) > 0.99999) {
          im1Val = im1(i,j,k) + miniEps;
          ratios.push_back(im2(i,j,k) / im1Val);
          numValid++;
        }
      }
    }
  }
  std::sort(ratios.begin(), ratios.end());
  return ratios[floor(ratios.size() / 2.0)];
}

// PS05 - 2.3 - merge images to make a single hdr image
Image makeHDR(vector<Image> &imSeq, float epsilonMini, float epsilonMaxi){

    // invert gamma correction
    for (int i=0; i < ((int) imSeq.size()) ; i++) {
      imSeq[i] = changeGamma(imSeq[i], 1.0/2.2, 1.0f);
    }


  // Get image data
  int height, width, channels;
  width = imSeq[0].width();
  height = imSeq[0].height();
  channels = imSeq[0].channels();
  Image imOut(width,height,channels);

  vector<Image> weights;
  weights.push_back(computeWeight(imSeq[0], epsilonMini,imSeq[0].max()));
  // Get the weights
  for (int i=1; i < ((int) imSeq.size()) - 1 ; i++) {
    weights.push_back(computeWeight(imSeq[i],epsilonMini, epsilonMaxi));
  }
  weights.push_back(computeWeight(imSeq[((int)imSeq.size() - 1)], imSeq[imSeq.size() - 1].min(), epsilonMaxi));
  
  // for (int i = 0; i < weights.size(); i++) {
  //   weights[i].debug_write();
  // }

  // Get the factors
  vector<float> factors;
  factors.push_back(1.0);
  float newVal;
  for (int i = 1; i < ((int) imSeq.size()) ; i++) {
    newVal = factors[i-1] * computeFactor(imSeq[i - 1], weights[i - 1], imSeq[i], weights[i]);
    factors.push_back(newVal);
  }
  assert(weights.size() == factors.size());
  int weight, weightSum;
  double val;
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      for (int k = 0; k < channels; k++) {
        weightSum = 0;
        val = 0;
        for (int image = 0; image < ((int) weights.size()); image++) {
          weight = weights[image](i,j,k);
          weightSum += weight;
          val += weight * (1.0/factors[image]) * imSeq[image](i,j,k);
        } 
        val = weightSum > 0.1 ? val / weightSum : imSeq[0](i,j,k); 
        imOut(i,j,k) = val;
      }
    }
  }
  return imOut;
}

void printMyVector(vector<float> v) {
  int i;
  for (i = 0; i < v.size(); i++) {
    cout << v[i] << " ";
  }
  cout << endl;
}
/**************************************************************
 //                      TONE MAPPING                        //
 *************************************************************/


// PS05 - 3 - tone map an hdr image
Image toneMap(const Image &im, float targetBase, float detailAmp, bool useBila, float sigmaRange) {

    vector<Image> lumiChrom = lumiChromi(im);

    float sigmaDomain = max(im.width(), im.height());
    sigmaDomain /= 50;

    Image logLumi = log10Image(lumiChrom[0]);
    Image blurLogLumi = useBila ? bilateral(logLumi, sigmaRange, sigmaDomain, 3, true) 
                                : gaussianBlur_seperable(logLumi, sigmaDomain, 3, true);
                     
    Image detail = logLumi - blurLogLumi;
    
    float logMax = logLumi.max();
    float largeRange = logMax - logLumi.min();
    float k = 2 / largeRange;

    int width = logLumi.width(), height = logLumi.height(), channels = logLumi.channels();
    Image outLog(width, height, channels);
    for (int x = 0; x < width; x++) {
      for (int y = 0; y < height; y++) {
        for (int c = 0; c < channels; c++) {
          outLog(x,y,c) = detailAmp * detail(x,y,c) + k * (logLumi(x,y,c) - logMax);
        }
      }
    }

    Image out10 = exp10Image(outLog);
    Image outRGB = lumiChromi2rgb(out10, lumiChrom[1]);
    // UNCOMMENT THIS LINE!
    // add gamma correction back into the image right before returning
    Image outImage = changeGamma(outRGB, 1.0f, 1/2.2);
    
    return outImage;
}



// Tone Mapping Helpful Functions

// image --> log10Image
Image log10Image(const Image &im) {
  // Taking a linear image im, transform to log10 scale.
  // To avoid infinity issues, make any 0-valued pixel be equal the the minimum
  // non-zero value. See image_minnonzero(im).
  float min = (im.min() == 0) ? image_minnonzero(im) : im.min();
  float logMin = log10(min);

    int width = im.width(), height = im.height(), channels = im.channels();
    Image output(width, height, channels);

    for (int x = 0; x < width; x++) {
      for (int y = 0; y < height; y++) {
        for (int c = 0; c < channels; c++) {
          output(x,y,c) = (im(x,y,c) != 0) ? log10(im(x,y,c)) : logMin;
        }
      }
    }

  return output;
}

// Image --> 10^Image
Image exp10Image(const Image &im) {
    int width = im.width(), height = im.height(), channels = im.channels();
    Image output(width, height, channels);

    for (int x = 0; x < width; x++) {
      for (int y = 0; y < height; y++) {
        for (int c = 0; c < channels; c++) {
          output(x,y,c) = pow(10, im(x,y,c));
        }
      }
    }
  return output;
}

// min non-zero pixel value of image
float image_minnonzero(const Image &im) {
    int width = im.width(), height = im.height(), channels = im.channels();
    Image output(width, height, channels);

    float val;
    float min = im.max();

    // find min value
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
        for (int c = 0; c < channels; c++) {
          val = im(i,j,c);
          min = (val != 0 && min > val) ? val : min;
        }
      }
    }
    return min;
}
