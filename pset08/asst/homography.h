#ifndef __homography__h
#define __homography__h

using namespace std;

#include "Image.h"
#include "matrix.h"
#include "basicImageManipulation.h"
#include <iostream>
#include <math.h>

// homography.h
// Assignment 6

// homographies
void applyhomography(const Image &source, Image &out, Matrix &H, bool bilinear=false);
void applyhomographyFast(const Image &source, Image &out, Matrix &H, bool bilinear);
Matrix homogenize(Matrix &v);
Matrix computeHomography(const float listOfPairs[4][2][3]);
void addConstraint(Matrix &systm,  int i, const float constr[2][3]);

// bbox related functions
vector<float> computeTransformedBBox(int imwidth, int imheight, Matrix H);
Matrix translate(vector<float> B);
vector <float> bboxUnion(vector<float> B1, vector<float> B2);
// would help you debug
Image drawBoundingBox(const Image &im, vector<float> minmax);

// stitching
Image stitch(const Image &im1, const Image &im2, const float listOfPairs[4][2][3]);
Image stitch(const Image &im1, const Image &im2, Matrix H);

// Implemented for your use
float min_vec_elem(vector<float> v);
float max_vec_elem(vector<float> v);

#endif
