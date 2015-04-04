// basicImageManipulation.cpp
// Assignment 2

#include "basicImageManipulation.h"
using namespace std;




/**************************************************************
 //               DON'T EDIT BELOW THIS LINE                //
 //          Functions from previous problem sets           //
 *************************************************************/



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
    double exponent = new_gamma/old_gamma;
    Image output = im;
    for (int i = 0 ; i < im.number_of_elements();i++) {
        output(i) = pow(im(i), exponent);
    }
    return output;
}

// Change the exposure of the image. This is different than brightness because
// it means you multiply an image's intensity in the linear domain.
Image exposure(const Image &im, const float &factor) {
    Image output = changeGamma(im, 1/2.2, 1.0);
    output = output*factor;
    output = changeGamma(output, 1.0, 1/2.2);
    return output;

}


Image color2gray(const Image &im, const std::vector<float> &weights) {
    // Image output(im.width(), im.height(), 1);
    // Convert to grayscale
    // return output;
    float normalization = 0;
    for (unsigned int i = 0 ; i < weights.size();i++) {
      normalization += weights[i];
    }
    Image output(im.width(), im.height(), 1);
    for (int i = 0 ; i < im.width(); i++ ) {
        for (int j = 0 ; j < im.height(); j++ ) {
            output(i,j,0) = im(i,j,0) * weights[0] + im(i,j,1) * weights[1] + im(i,j,2) *weights[2];
        }
    }
    output = output/normalization;
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
    Image im_chrominance = im;
    Image im_luminance(im.width(), im.height(), 1);
    std::vector<Image> output;
    // Create luminance and chrominance images
    im_luminance = color2gray(im);
    for (int i = 0 ; i < im.width(); i++) {
        for (int j = 0 ; j < im.height(); j++) {
            for (int c = 0 ; c < im.channels(); c++ ){
                im_chrominance(i,j,c) = im_chrominance(i,j,c) / im_luminance(i,j);
            }
        }
    }
    output.push_back(im_luminance);
    output.push_back(im_chrominance);
    return output;
}


// go from a luminance/crominance images back to an rgb image
// the function doesn't do any error checking!
Image lumiChromi2rgb(const Image &lumi, const Image &chromi) {
  Image final = Image(lumi.width(), lumi.height(), 3);
  for (int w = 0; w < lumi.width(); w++) {
    for (int h = 0; h < lumi.height(); h++) {
      for (int c = 0; c < 3; c++) {
        final(w,h,c) = lumi(w,h) * chromi(w,h,c);
      }
    }
  }
  return final;
}

// Modify brightness then contrast
Image brightnessContrastLumi(const Image &im, const float &brightF, const float & contrastF, const float & midpoint) {
    // Create output image of appropriate size
    // Modify brightness, then contrast of luminance image
    // return output;
    std::vector<Image> output = lumiChromi(im);
    Image im_luminance = output[0];
    Image im_chrominance = output[1];
    im_luminance = brightness(im_luminance, brightF);
    im_chrominance = contrast(im_chrominance, contrastF, midpoint);
    for (int i = 0 ; i < im.width(); i++ ){
        for (int j = 0 ; j < im.height(); j++) {
            for (int c = 0; c < im.channels(); c++) {
                im_chrominance(i,j,c) = im_chrominance(i,j,c) * im_luminance(i,j);
            }
        }
    }
    return im_chrominance;
}

Image rgb2yuv(const Image &im) {
    // Create output image of appropriate size
    // Change colorspae
    // return output;
    Image output(im.width(), im.height(), im.channels());
    for (int i = 0; i < im.width(); i++) {
        for (int j = 0 ; j < im.height(); j++) {
            output(i,j,0) = 0.299 * im(i,j,0) + 0.587 * im(i,j,1) + 0.114 * im(i,j,2);
            output(i,j,1) = -0.147 * im(i,j,0) + (-0.289) *im(i,j,1) + 0.436 * im(i,j,2);
            output(i,j,2) = 0.615 * im(i,j,0) + -0.515 * im(i,j,1) + (-0.100) * im(i,j,2);
        }
    }
    return output;
}

Image yuv2rgb(const Image &im) {
    // Create output image of appropriate size
    // Change colorspae
    // return output;
    Image output(im.width(), im.height(), im.channels());
    for (int i = 0; i < im.width(); i++) {
        for (int j = 0 ; j < im.height(); j++) {
            output(i,j,0) =  im(i,j,0) + 0 * im(i,j,1) + 1.14 * im(i,j,2);
            output(i,j,1) =  im(i,j,0) -0.395 *im(i,j,1) -0.581 * im(i,j,2);
            output(i,j,2) =  im(i,j,0) + 2.032 * im(i,j,1) + 0 * im(i,j,2);
        }
    }
    return output;
}

Image saturate(const Image &im, const float &factor) {
    // Create output image of appropriate size
    // Saturate image
    // return output;
    Image output = rgb2yuv(im);
    for (int i = 0 ; i < im.width(); i++) {
        for (int j = 0 ; j < im.height(); j++) {
            output(i,j,1) = output(i,j,1) * factor;
            output(i,j,2) = output(i,j,2) * factor;
        }
    }
    output = yuv2rgb(output);
    return output;
}

// Return two images in a C++ vector
std::vector<Image> spanish(const Image &im) {
    // Remember to create the output images and the output vector
    // Push the images onto the vector
    // Do all the required processing
    // Return the vector, color image first
    Image output_L = color2gray(im);
    Image output_C = rgb2yuv(im);

    for (int i = 0; i < im.width(); i++) {
        for (int j = 0; j < im.height(); j++) {
            output_C(i,j,0) = 0.5;
            output_C(i,j,1) = -output_C(i,j,1);
            output_C(i,j,2) = -output_C(i,j,2);
        }
    }
    output_C = yuv2rgb(output_C);
    int bdot1 = floor(im.width()/2);
    int bdot2 = floor(im.height()/2);

    output_L(bdot1, bdot2,0) = 0;
    output_C(bdot1,bdot2,0) = 0;
    output_C(bdot1,bdot2,1) = 0;
    output_C(bdot1,bdot2,2) = 0;

    std::vector<Image> output;
    output.push_back(output_C);
    output.push_back(output_L);
    return output;
}


// White balances an image using the gray world assumption
Image grayworld(const Image & im) {
    // Your code goes here
    float kr = 0, kg = 0, kb = 0;
    float N = im.width()*im.height();
    for (int i = 0 ; i < im.width(); i++) {
        for (int j = 0 ; j < im.height(); j++) {
            kr = kr + im(i,j,0);
            kg = kg + im(i,j,1);
            kb = kb + im(i,j,2);
        }
    }
    kr = kr/N;
    kg = kg/N;
    kb = kb/N;
    float redFactor = kg / kr;
    float blueFactor = kg / kb;
    Image output = im;
    for (int i = 0 ; i < im.width(); i++) {
        for (int j = 0 ; j < im.height();j ++) {
	  output(i,j,0) = output(i,j,0)*redFactor;
	  output(i,j,1) = output(i,j,1);
	  output(i,j,2) = output(i,j,2)*blueFactor;
        }
    }
    return output;
}
