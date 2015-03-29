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

/****************************************************
 * PS06: Must implement these functions.            *
 ***************************************************/

// homographies
void applyhomography(const Image &source, Image &out, Matrix &H, bool bilinear=false);
void applyhomographyFast(const Image &source, Image &out, Matrix &H, bool bilinear);
Matrix computeHomography(const float listOfPairs[4][2][3]);

// bbox related functions
vector<float> computeTransformedBBox(int imwidth, int imheight, Matrix H);
Matrix translate(vector<float> B);
vector <float> bboxUnion(vector<float> B1, vector<float> B2);

// stitching
Image stitch(const Image &im1, const Image &im2, const float listOfPairs[4][2][3]);



/****************************************************
 * PS06: Helpful optional functions.                *
 ***************************************************/

// Potentially useful functions, but optional-to-implement functions:
Image drawBoundingBox(const Image &im, vector<float> minmax);
Matrix homogenize(Matrix &v);
void addConstraint(Matrix &systm,  int i, const float constr[2][3]);

// Useful functions: already implemented for your use
float min_vec_elem(vector<float> v);
float max_vec_elem(vector<float> v);

#endif
