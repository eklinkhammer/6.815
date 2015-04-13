#ifndef __basicImageManipulation__h
#define __basicImageManipulation__h

using namespace std;

#include "Image.h"
#include <iostream>
#include <math.h>

// Functions from PS01
Image changeGamma(const Image & im, const double & old_gamma, const double & new_gamma);
Image exposure(const Image &im, const float &factor);
static const float weight_init[3] = {0.299, 0.587, 0.114};
Image color2gray(const Image &im, const std::vector<float> &weights = std::vector<float>(weight_init, weight_init+3));
std::vector<Image> lumiChromi(const Image &im);
Image brightnessContrastLumi(const Image &im, const float &brightF, const float & contrastF, const float & midpoint = 0.3);
Image rgb2yuv(const Image &im);
Image yuv2rgb(const Image &im);
Image saturate(const Image &im, const float &k);
std::vector<Image> spanish(const Image &im);
Image grayworld(const Image & in);
Image lumiChromi2rgb(const Image &lumi, const Image &chromi);

// Functions from PS00
Image brightness(const Image &im, const float &factor);
Image contrast(const Image &im, const float &factor, const float &midpoint = 0.5);

// PS02 Functions: Scaling and Rotating an Image
Image scaleNN(const Image &im, const float &factor);
Image scaleLin(const Image &im, const float &factor);
float interpolateLin(const Image &im, float x, float y, int z, bool clamp=false);
Image rotate(const Image &im, const float &theta); 

#endif
