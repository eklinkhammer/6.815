#ifndef __a1__h
#define __a1__h

using namespace std;

// a1.h
// Assignment 1
// by Neal Wadhwa

#include "Image.h"
#include <iostream>
#include <math.h>
// Functions from previous assignment
Image brightness(const Image &im, const float &factor);

Image contrast(const Image &im, const float &factor, const float &midpoint = 0.5);

// Gamma and exposure
Image changeGamma(const Image & im, const double & old_gamma, const double & new_gamma);

Image exposure(const Image &im, const float &factor);



// Colorspace problems
static const float weight_init[3] = {0.299, 0.587, 0.114};
Image color2gray(const Image &im, const std::vector<float> &weights = std::vector<float>(weight_init, weight_init+3));

std::vector<Image> lumiChromi(const Image &im);

Image brightnessContrastLumi(const Image &im, const float &brightF, const float & contrastF, const float & midpoint = 0.3);

Image rgb2yuv(const Image &im);

Image yuv2rgb(const Image &im);

Image saturate(const Image &im, const float &k);

std::vector<Image> spanish(const Image &im);

// White Balance
Image grayworld(const Image & in);





#endif
