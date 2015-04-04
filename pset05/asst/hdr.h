#ifndef __hdr__h
#define __hdr__h

using namespace std;

// hdr.h
// Assignment 5

#include "Image.h"
#include "basicImageManipulation.h"
#include <iostream>
#include <math.h>

// HDR
Image computeWeight(const Image &im, float epsilonMini=0.002, float epsilonMaxi=0.99);
float computeFactor(const Image &im1, const Image &w1, const Image &im2, const Image &w2);
Image makeHDR(vector<Image> &imSeq, float epsilonMini=0.002, float epsilonMaxi=0.99);

// Tone Mapping
Image toneMap(const Image &im, float targetBase=100, float detailAmp=3, bool useBila=false, float sigmaRange=0.1);
Image exp10Image(const Image &im);
Image log10Image(const Image &im);
float image_minnonzero(const Image &im);

// Quickselect
float select(vector<float> &list, int left, int right, int n);
int partition(vector<float> &list, int left, int right, int pivotIndex);
void swap(vector<float> &list, int a, int b);

vector<float> selectionSort(vector<float> &list);
int minInRange(vector<float> v, int start);
void printMyVector(vector<float> v);
float getMedian(vector<float> &list);
#endif
