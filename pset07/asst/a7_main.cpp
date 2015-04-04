#include "homography.h"
#include "panorama.h"
#include "messageassert.h"
#include "matrix.h"
#include <ctime>
#include <unistd.h>

using namespace std;


void testComputeTensor() {
  // load images
  Image stata1("./Input/stata-1.png");
  Image stata2("./Input/stata-2.png");

  // compute tensors
  Image tensor1 = computeTensor(stata1);
  tensor1.write("./Output/stataTensor-1.png");
  tensor1 = tensor1 / tensor1.max();
  tensor1.write("./Output/stataTensor-1normed.png");

  Image tensor2 = computeTensor(stata2);
  tensor2.write("./Output/stataTensor-2.png");
  tensor2 = tensor2 / tensor2.max();
  tensor2.write("./Output/stataTensor-2normed.png");
}

void testCornerResponse() {
  // load images
  Image stata1("./Input/stata-1.png");
  Image stata2("./Input/stata-2.png");

  // corner responses
  Image cr1 = cornerResponse(stata1);
  (cr1/cr1.max()).write("./Output/stata1-cornerResponse.png");
  Image cr2 = cornerResponse(stata2);
  (cr2/cr2.max()).write("./Output/stata2-cornerResponse.png");
}

void testHarrisCorners() {
  // load images
  Image stata1("./Input/stata-1.png");
  Image stata2("./Input/stata-2.png");

  // get harris corners
  vector<Point> h1 = HarrisCorners(stata1);
  vector<Point> h2 = HarrisCorners(stata2);
  visualizeCorners(stata1, h1).write("./Output/stata1-corners.png");
  visualizeCorners(stata2, h2).write("./Output/stata2-corners.png");
}

void testDescriptor() {
  Image stata1("./Input/stata-1.png");
  vector<Image> lc = lumiChromi(stata1);
  Image desc = descriptor(lc[0], Point(10, 10));
  int myints[] = {10-4,10+4,10-4,10+4};
  std::vector<float> minmax (myints, myints + sizeof(myints) / sizeof(int));
  drawBoundingBox(lc[0], minmax).write("./Output/stata1-descriptor-location.png");
  desc.write("./Output/stata1-descriptor-test.png");
}

void testComputeFeatures() {
  // load images
  Image stata1("./Input/stata-1.png");
  Image stata2("./Input/stata-2.png");

  // get and output features
  vector<Point> h1 = HarrisCorners(stata1);
  vector<Feature> f1 = computeFeatures(stata1, h1);
  visualizeFeatures(stata1, f1).write("./Output/stata1-features.png");

  vector<Point> h2 = HarrisCorners(stata2);
  vector<Feature> f2 = computeFeatures(stata2, h2);
  visualizeFeatures(stata2, f2).write("./Output/stata2-features.png");
}

void testFindCorrespondences() {
  // load images
  Image stata1("./Input/stata-1.png");
  Image stata2("./Input/stata-2.png");

  // get and output features
  vector<Point> h1 = HarrisCorners(stata1);
  vector<Feature> f1 = computeFeatures(stata1, h1);
  vector<Point> h2 = HarrisCorners(stata2);
  vector<Feature> f2 = computeFeatures(stata2, h2);

  // correspondances
  vector<Correspondance> corr = findCorrespondences(f1, f2);
  visualizePairs(stata1, stata2, corr).write("./Output/stata-featcorr.png");
}


void testRANSAC() {
  Image stata1("./Input/stata-1.png");
  Image stata2("./Input/stata-2.png");

  // get and output features
  vector<Point> h1 = HarrisCorners(stata1);
  vector<Feature> f1 = computeFeatures(stata1, h1);
  vector<Point> h2 = HarrisCorners(stata2);
  vector<Feature> f2 = computeFeatures(stata2, h2);

  // correspondances
  vector<Correspondance> corr = findCorrespondences(f1, f2);

  Matrix H = RANSAC(corr, 100);
  vector<bool> ins = inliers(H, corr);
  visualizePairsWithInliers(stata1, stata2, corr, ins).write("./Output/stata-RANSAC-featcorr-inliers.png");

  vector<Image> output = visualizeReprojection(stata1, stata2, H, corr, ins);
  output[0].write("./Output/stata-RANSAC-1-featreproject.png");
  output[1].write("./Output/stata-RANSAC-2-featreproject.png");
}

void testVisualizeCorrespondenceInliers() {
  Image stata1("./Input/stata-1.png");
  Image stata2("./Input/stata-2.png");

  // get and output features
  vector<Point> h1 = HarrisCorners(stata1);
  vector<Feature> f1 = computeFeatures(stata1, h1);
  vector<Point> h2 = HarrisCorners(stata2);
  vector<Feature> f2 = computeFeatures(stata2, h2);

  // correspondances
  vector<Correspondance> corr = findCorrespondences(f1, f2);

  float listOfPairs[4][2][3];
  Matrix H(3,3);
  vector<Correspondance> corrSample = sampleCorrespondances(corr);
  getListOfPairs(corrSample, listOfPairs);

  H = computeHomography(listOfPairs);

  vector<bool> ins = inliers(H, corr);
  visualizePairsWithInliers(stata1, stata2, corr, ins).write("./Output/stata-featcorr-inliers.png");

  vector<Image> output = visualizeReprojection(stata1, stata2, H, corr, ins);
  output[0].write("./Output/stata-1-featreproject.png");
  output[1].write("./Output/stata-2-featreproject.png");
}

void testAutoStitch() {
  Image stata1("./Input/stata-1.png");
  Image stata2("./Input/stata-2.png");
  autostitch(stata1, stata2).write("./Output/Stata-final-stitch.png");
}

void testAutoStitchBoston() {
  Image stata1("./Input/boston-skyline-1.png");
  Image stata2("./Input/boston-skyline-2.png");
  autostitch(stata1, stata2).write("./Output/boston-skyline-final-stitch.png");
}

// Your code **has to compile** on the submission system with all of the test cases
// uncommented! We will not grade the contents of this main function
int main() {

  // This line gives the randomness a seed.
  srand(time(0));
  // If you want deterministic results, uncomment this line
  // srand(0); // Fixed seed for deterministic results

  // testComputeTensor();
  // testCornerResponse();
  // testHarrisCorners();
  // testDescriptor();
  // testComputeFeatures();
  // testFindCorrespondences();
  // testRANSAC();
  // testAutoStitch();
  // testAutoStitchBoston();


  // testVisualizeCorrespondenceInliers();
  return 0;
}
