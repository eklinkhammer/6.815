#ifndef __blending__h
#define __blending__h

using namespace std;

#include "Image.h"
#include "matrix.h"
#include "filtering.h"
#include "homography.h"
#include "panorama.h"
#include "messageassert.h"
#include "basicImageManipulation.h"
#include <iostream>
#include <math.h>

#define pi 3.1415926535897

// blending.h
// Assignment 8

// blending
void applyhomographyBlend(const Image &source, const Image &weight, Image &out, Matrix &H, bool bilinear=false);
Image stitchLinearBlending(const Image &im1, const Image &im2, const Image &we1, const Image &we2, Matrix H);
Image blendingweight(int imwidth, int imheight);
vector<Image> scaledecomp(const Image &im, float sigma = 2.0);
Image stitchBlending(Image &im1, Image &im2, Matrix H, int blend);
Image autostitch(Image &im1, Image &im2, int blend, float blurDescriptor=0.5, float radiusDescriptor=4);

// mini planets
Image pano2planet(const Image &pano, int newImSize, bool clamp=true);

// stitch N images
vector<Matrix> sequenceHs(vector<Image> ims, float blurDescriptor=0.5, float radiusDescriptor=4);
vector<float> bboxN(const vector<Matrix> &Hs, const vector<Image> &ims);
Image autostitchN(vector<Image> ims, int refIndex, float blurDescriptor=0.5, float radiusDescriptor=4);

// helpful functions
Image copychannels(const Image &im, int nChannels);
Matrix eye(int n);
vector <Matrix> stackHomographies(vector <Matrix> Hs, int refIndex);
Image create_output(Matrix T, vector<float> bBox, int channels);
vector<float> getBBox(const Image &im1, const Image &im2, Matrix H);
void applyhomographyBlendHF(const Image &highFreq1, const Image &highFreq2, const Image &we1, const Image &we2, Matrix T, Matrix TH, Image out, bool bilinear);
Image safeDivide(Image &im1, Image &im2);

#endif
