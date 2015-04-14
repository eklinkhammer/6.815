#ifndef __panorama__h
#define __panorama__h

using namespace std;

#include "Image.h"
#include "matrix.h"
#include "filtering.h"
#include "homography.h"
#include "messageassert.h"
#include "basicImageManipulation.h"
#include <iostream>
#include <algorithm>    // std::max, random_shuffle
#include <math.h>

// panorama.h
// Assignment 7
typedef float (&array423)[4][2][3];
typedef float (&array23)[2][3];

// Point and Feature classes, already implemented for Pset07
// N.B. Need "Rule of three" here since these objects are used in vectors
class Point {
public:
  int x, y; // point info
  Point(int xp, int yp); // Constructors
  Point();              // Constructors
  Point(const Point& other); // copy constructor
  Point & operator= (const Point & other); // copy assignment operator
  void print(); // helpful printing
  Matrix toHomogenousCoords();
  ~Point() {}; // Destructor. printf("Destroying Point\n");
};

class Feature {
public:
  Feature(Point &ptp, Image &descp); // Constructor
  Feature(const Feature& other); // Copy constructor
  Feature& operator= (Feature& other); // copy assignment operator
  Point point(); // get point (this calls point cc)
  Image desc(); // get Image (this maybe calls image cc)
  void print(); // pretty printing
  ~Feature() {}; // Destructor printf("Destroying Feature\n");
private:
  Point *pt;
  Image *dsc; // 9x9 descriptor
};

class Correspondance {
public:
  Correspondance(Feature &f1p, Feature &f2p); // Constructors
  Correspondance(const Correspondance& other); // copy constructor
  Correspondance & operator= (const Correspondance & other); // copy assignment operator
  vector<Feature> features();
  Feature feature(int i);
  void print(); // helpful printing
  void toListOfPairs(array23);
  ~Correspondance() {}; // Destructor. printf("Destroying Correspondance\n");
private:
  Feature *f1, *f2; // corresp info
};

// Pset07: Harris Corners
void zeroOrMax(Image &R, float maxiDiam);
void excludeCorners(Image &im, float boundarySize);
Image computeTensor(const Image &im, float sigmaG=1, float factorSigma=4);

vector <Point> HarrisCorners(const Image &im, float k=0.15, float sigmaG=1, float factorSigma=4, float maxiDiam=7, float boundarySize=5);

Image cornerResponse(const Image &im, float k=0.15, float sigmaG=1, float factorSigma=4);

Image descriptor(const Image &blurredIm, Point p, float radiusDescriptor=4);

vector <Feature> computeFeatures(const Image &im, vector<Point> cornersL, float sigmaBlurDescriptor=0.5, float radiusDescriptor=4);

// Pset07: Correspondances
float l2Features(Feature &f1, Feature &f2);

vector <Correspondance> findCorrespondences(vector<Feature> listFeatures1, vector<Feature> listFeatures2, float threshold=1.7);

// Pset07: RANSAC
vector<bool> inliers(Matrix H, vector <Correspondance> listOfCorrespondences, float epsilon=4);

Matrix RANSAC(vector <Correspondance> listOfCorrespondences, int Niter=200, float epsilon=4);

vector<Correspondance> sampleCorrespondances(vector <Correspondance> listOfCorrespondences);
Matrix getH(Image &im1, Image &im2, float blurDescriptor, float radiusDescriptor);

void getListOfPairs(vector <Correspondance> listOfCorrespondences, array423 listOfPairs);

// PSet07: Final stitching
Image autostitch(Image &im1, Image &im2, float blurDescriptor=0.5, float radiusDescriptor=4);

// potentially useful function, optional to implement
Image getBlurredLumi(const Image &im, float sigmaG);

int countBoolVec(vector<bool> ins);

/***********************************************************************
 * Helpful Functions, already implemented in Pset07 *
 ***********************************************************************/
void addConstraint9(Matrix &systm,  int i, const float constr[2][3]);
Matrix leastSquares(Matrix A, Matrix b);
static const float color_init[3] = {1.0, 1.0, 1.0};
static const vector<float> color_vinit(color_init, color_init + 3);
Image visualizeCorners(const Image &im, vector<Point> pts, int rad=2, const vector<float> & color = color_vinit);

Image visualizeFeatures(const Image &im, vector <Feature> LF, float radiusDescriptor=4);


void drawLine(Point p1, Point p2, Image &im, const vector<float> & color = color_vinit);

Image visualizePairs(const Image &im1, const Image &im2, vector<Correspondance> corr);

Image visualizePairsWithInliers(const Image &im1, const Image &im2, vector<Correspondance> corr, const vector<bool> & ins);

vector<Image> visualizeReprojection(const Image &im1, const Image &im2, Matrix H, vector<Correspondance> & corr, const vector<bool> & ins);
#endif
