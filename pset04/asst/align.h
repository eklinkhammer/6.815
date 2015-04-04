// align.h
// Assignment 4

#ifndef __align__h
#define __align__h

using namespace std;

#include "Image.h"
#include "basicImageManipulation.h"
#include <iostream>
#include <math.h>

Image denoiseSeq(const vector<Image> &imgs);
Image logSNR(const vector<Image> &imSeq, float scale=1.0/20.0);
vector<int> align(const Image &im1, const Image &im2, int maxOffset=20);
Image alignAndDenoise(const vector<Image> &imSeq, int maxOffset=20);
Image split(const Image &sergeyImg);
Image sergeyRGB(const Image &sergeyImg, int maxOffset=20);

Image roll(const Image &im, int xRoll, int yRoll); 
 
#endif
