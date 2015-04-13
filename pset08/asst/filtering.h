#ifndef __filtering__h
#define __filtering__h

using namespace std;

// morphing.h
// Assignment 3

#include "Image.h"
#include "basicImageManipulation.h"
#include <iostream>
#include <math.h>


class Filter {

public:

    std::vector<float> kernel;
    int width;
    int height;

    // function to convolve your filter with an image
    Image Convolve(const Image &im, bool clamp=true);

    // Accessors of the filter values
    const float & operator()(int x, int y) const;
    float & operator()(int x, int y);

    //Constructor
    Filter(const vector<float> &fData, const int &fWidth, const int &fHeight);
    Filter(const int &fWidth, const int &fHeight); // kernel with all zero

    // Destructor. Because there is no explicit memory management here, this doesn't do anything
    ~Filter();


// The following are functions and variables that are not accessible from outside the class
private:

};

// Box Blurring
Image boxBlur(const Image &im, const int &k, bool clamp=true);
Image boxBlur_filterClass(const Image &im, const int &k, bool clamp=true);

// Gradient Filter
Image gradientMagnitude(const Image &im, bool clamp=true);
Image gradientX(const Image &im, bool clamp=true);
Image gradientY(const Image &im, bool clamp=true);

// Gaussian Blurring
vector<float> gauss1DFilterValues(float sigma, float truncate);
vector<float> gauss2DFilterValues(float sigma, float truncate);
Image gaussianBlur_horizontal(const Image &im, float sigma, float truncate=3.0, bool clamp=true);
Image gaussianBlur_seperable(const Image &im, float sigma, float truncate=3.0, bool clamp=true);
Image gaussianBlur_2D(const Image &im, float sigma, float truncate=3.0, bool clamp=true);

// Sharpen an Image
Image unsharpMask(const Image &im, float sigma, float truncate=3.0, float strength=1.0, bool clamp=true);

// Bilaterial Filtering
Image bilateral(const Image &im, float sigmaRange=0.1, float sigmaDomain=1.0, float truncateDomain=3.0, bool clamp=true);
Image bilaYUV(const Image &im, float sigmaRange=0.1, float sigmaY=1.0, float sigmaUV=4.0, float truncateDomain=3.0, bool clamp=true);

// Return impulse image of size kxkx1
Image impulseImg(const int &k);

// Pset7: maximum filter
Image maximum_filter(const Image &im, float maxiDiam);

#endif
