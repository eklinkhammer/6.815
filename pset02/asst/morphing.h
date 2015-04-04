#ifndef __morphing__h
#define __morphing__h

using namespace std;

// morphing.h
// Assignment 2

#include "Image.h"
#include "basicImageManipulation.h"
#include "SegmentException.h"
#include <iostream>
#include <math.h>


    
class Segment {
    
public:
    
    std::vector<float> P;
    std::vector<float> Q;
    std::vector<float> PQ;
    std::vector<float> perpPQ;
    std::vector<float> PQDivByPQlength2;
    std::vector<float> perpPQDivByPQlength;
    float PQ2;
    float PQlength;
    
    float getU(float &x, float &y);
    float getV(float &x, float &y);
    float dist(float &x, float &y);
    vector<float> UVtoX(float &u, float &v);
    float weight(float &x, float &y, float &a, float &b, float &p);

    
    //Constructor
    Segment(const float &x1, const float &y1, const float &x2, const float &y2);
    
    // Destructor. Because there is no explicit memory management here, this doesn't do anything
    ~Segment();

    
// The following are functions and variables that are not accessible from outside the class
private:
    
    float dot(vector<float> &vec1, vector<float> &vec2);
    vector<float> subtract(vector<float> &vec1, vector<float> &vec2);
    vector<float> scalarMult(vector<float> &vec, float &factor);

};

Image warpBy1(const Image &im, Segment &segBefore, Segment &segAfter); 
Image warp(const Image &im, vector<Segment> &segsBefore, vector<Segment> &segsAfter, float a=10.0, float b=1.0, float p=1.0);
vector<Image> morph(const Image &im1, const Image &im2, vector<Segment> &segsBefore, vector<Segment> &segsAfter, int N=1, float a=10.0, float b=1.0, float p=1.0);

 
#endif
