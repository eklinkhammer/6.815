// a1.h
// Assignment 1


#include "a1.h"
using namespace std;

// The implementation of these functions will be given to you after the pset 0 deadline
Image brightness(const Image &im, const float &factor) {
  return im * factor;
}

Image contrast(const Image &im, const float &factor, const float &midpoint) {
  return (im - midpoint)*factor+midpoint;
}



Image changeGamma(const Image & im, const double & old_gamma, const double & new_gamma) {
  // Image output(im.width(), im.height(), im.channels());
  // Figure out what power to take the values of im, to get the values of output
  // return output;
  Image output = Image(im.width(), im.height(), im.channels());
  int w, h, c;
  double old_intensity;
  for (w = 0; w < im.width(); w++) {
    for (h = 0; h < im.height(); h++) {
      for (c = 0; c < im.channels(); c++) {
        old_intensity = pow(im(w,h,c), (1.0/old_gamma));
        output(w,h,c) = pow(old_intensity, new_gamma);
      }
    }
  }
  return output;
}

// Change the exposure of the image. This is different than brightness because
// it means you multiply an image's intensity in the linear domain.
Image exposure(const Image &im, const float &factor) {
  // Assumes input is gamma encoded with 1/2.2
  const double gamma = 1.0/2.2;
  Image decoded = changeGamma(im, gamma, 1);
  Image scaled = brightness(decoded, factor);
  Image recoded = changeGamma(scaled, 1, gamma);
  return recoded;
}


Image color2gray(const Image &im, const std::vector<float> &weights) {
  // Image output(im.width(), im.height(), 1);
  // Convert to grayscale
  // return output;
  Image output = Image(im.width(), im.height(), 1);
  double average;
  int i,j;
  for (i = 0; i < im.width(); i++) {
    for (j = 0; j < im.height(); j++) {
      average = weights[0] * im(i,j,0) + weights[1] * im(i,j,1) + weights[2] * im(i,j,2);
      output(i,j,0) = average;
    }
  }
  return output;
}


// For this function, we want two outputs, a single channel luminance image 
// and a three channel chrominance image. Return them in a vector with luminance first
std::vector<Image> lumiChromi(const Image &im) {
  // Image im_chrominance(im.width(), im.height(), im.channels);
  // Image im_luminance(im.width(), im.height(), 1);
  // std::vector<Image> output;
  // Create luminance and chrominance images
  // output.push_back(im_luminance);
  // output.push_back(im_chrominance);
  // return output;
  std::vector<Image> output;
  Image im_chrom = Image(im.width(), im.height(), im.channels());
  Image im_lum = color2gray(im);
  int i, j, k;
  for (i = 0; i < im.width(); i++) {
    for (j = 0; j < im.height(); j++) {
      for (k = 0; k < im.channels(); k++) {
        if (im_lum(i,j,0) == 0) {
          im_chrom(i,j,k) = 0;
        } else {
          im_chrom(i,j,k) = im(i,j,k) / im_lum(i,j,0);
        }
      }
    }
  }
  output.push_back(im_lum);
  output.push_back(im_chrom);
  return output;
}

// Modify brightness then contrast
Image brightnessContrastLumi(const Image &im, const float &brightF, const float & contrastF, const float & midpoint) {
  // Create output image of appropriate size
  // Modify brightness, then contrast of luminance image
  // return output;
  std::vector<Image> images = lumiChromi(im);
  Image lumi = images[0];
  Image brighter = brightness(lumi, brightF);
  Image contrasted = contrast(brighter, contrastF, midpoint);
  Image chrom = images[1];
  int height = im.height();
  int width = im.width();
  int channels = im.channels();
  Image output = Image(width, height, channels);
  int i, j, k;
  for (i = 0; i < width; i++ ) {
    for (j = 0; j < height; j++ ) {
      for (k = 0; k < channels; k++ ) {
        output(i,j,k) = chrom(i,j,k) * contrasted(i,j,0);
      }
    }
  }
  return output;
}

Image rgb2yuv(const Image &im) {
  // Create output image of appropriate size
  // Change colorspace
  // return output; 
  Image output = Image(im.width(), im.height(), im.channels());
  int i, j;
  for (i = 0; i < im.width(); i++) {
    for (j = 0; j < im.height(); j++) {
      output(i,j,0) = 0.299 * im(i,j,0) + 0.587 * im(i,j,1) + 0.114 * im(i,j,2);// Y
      output(i,j,1) = -0.147 * im(i,j,0) - 0.289 * im(i,j,1) + 0.436 * im(i,j,2);// U
      output(i,j,2) = 0.615 * im(i,j,0) - 0.515 * im(i,j,1) - 0.1 * im(i,j,2);// V
    }
  }
  return output;
}

Image yuv2rgb(const Image &im) {
  // Create output image of appropriate size
  // Change colorspae
  // return output;
   Image output = Image(im.width(), im.height(), im.channels());
  int i, j;
  for (i = 0; i < im.width(); i++) {
    for (j = 0; j < im.height(); j++) {
      output(i,j,0) =  im(i,j,0)  + 1.14 * im(i,j,2);// R
      output(i,j,1) =  im(i,j,0) - 0.395 * im(i,j,1) - 0.581 * im(i,j,2);// G
      output(i,j,2) = im(i,j,0) + 2.032 * im(i,j,1);// B
    }
  }
  return output;
}

Image saturate(const Image &im, const float &factor) {
  // Create output image of appropriate size
  // Saturate image
  // return output;
  Image yuv = rgb2yuv(im);
  int i, j;
  for (i = 0; i < im.width(); i++) {
    for (j = 0; j < im.height(); j++) {
      yuv(i,j,1) = yuv(i,j,1) * factor;
      yuv(i,j,2) = yuv(i,j,2) * factor;
    }
  }
  Image rgb = yuv2rgb(yuv);
  return rgb; 
}

// Return two images in a C++ vector
std::vector<Image> spanish(const Image &im) {
  // Remember to create the output images and the output vector
  // Push the images onto the vector
  // Do all the required processing
  // Return the vector
  Image imag1_negUV = saturate(im, -1);
  Image constY = rgb2yuv(imag1_negUV);
  int i, j;
  for (i = 0; i < im.width(); i++) {
    for (j=0; j < im.height(); j++) {
      constY(i,j,0) = 0.5;
    }
  }
  Image bw = color2gray(im);
  int x = floor(im.width()/2.0);
  int y = floor(im.height()/2.0);
  constY(x,y,0) = 0;
  bw(x,y,0) = 0;
  Image first = yuv2rgb(constY);
  std::vector<Image> output;
  output.push_back(first);
  output.push_back(bw);
  return output;
}


// White balances an image using the gray world assumption
Image grayworld(const Image & im) {
  double average_green_val = 0, avg_blue = 0, avg_red = 0;
  int i, j;
  for (i = 0; i < im.width(); i++) {
    for (j = 0; j < im.height(); j++) {
      average_green_val += im(i,j,1);
      avg_blue += im(i,j,2);
      avg_red += im(i,j,0);
    }
  }
  
  int num = im.width() * im.height();
  average_green_val /= num;
  avg_blue /= num;
  avg_red /= num;
  
  Image balanced = Image(im.width(), im.height(), im.channels());
  for (i = 0; i < im.width(); i++) {
    for (j = 0; j < im.height(); j++) {
      balanced(i,j,0) = im(i,j,0) * average_green_val / avg_red;
      balanced(i,j,1) = im(i,j,1);
      balanced(i,j,2) = im(i,j,2) * average_green_val / avg_blue;
    }
  }
  return balanced;
}
