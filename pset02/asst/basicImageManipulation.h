#ifndef __basicImageManipulation__h
#define __basicImageManipulation__h

using namespace std;

#include "Image.h"
#include <iostream>
#include <math.h>

// PS02 Functions: Scaling and Rotating an Image
Image scaleNN(const Image &im, const float &factor);
Image scaleLin(const Image &im, const float &factor);
float interpolateLin(const Image &im, float x, float y, int z, bool clamp=false);
Image rotate(const Image &im, const float &theta); 

// Functions from PS00
Image brightness(const Image &im, const float &factor);
Image contrast(const Image &im, const float &factor, const float &midpoint = 0.5);


#endif
