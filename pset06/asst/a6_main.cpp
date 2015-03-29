#include "homography.h"
#include "messageassert.h"
#include "matrix.h"
#include <ctime>

using namespace std;

int testMatrixClass() {

  float m[] = {
      0.7577,    0.0462,    0.3816,    0.7547,    0.3404,    0.9593,    0.2435,    0.8308,
      0.7431,    0.0971,    0.7655,    0.2760,    0.5853,    0.5472,    0.9293,    0.5853,
      0.3922,    0.8235,    0.7952,    0.6797,    0.2238,    0.1386,    0.3500,    0.5497,
      0.6555,    0.6948,    0.1869,    0.6551,    0.7513,    0.1493,    0.1966,    0.9172,
      0.1712,    0.3171,    0.4898,    0.1626,    0.2551,    0.2575,    0.2511,    0.2858,
      0.7060,    0.9502,    0.4456,    0.1190,    0.5060,    0.8407,    0.6160,    0.7572,
      0.0318,    0.0344,    0.6463,    0.4984,    0.6991,    0.2543,    0.4733,    0.7537,
      0.2769,    0.4387,    0.7094,    0.9597,    0.8909,    0.8143,    0.3517,    0.3804,
  };

  /*
  float m_inv[] = {
      0.3376,    0.9372,   -0.5868,    1.1279,    2.4889,   -1.0771,   -1.7779,   -0.2543,
     -0.4910,   -0.4325,    0.5859,   -0.0524,   -0.7784,    0.8114,   -0.1198,    0.2249,
      0.2556,   -0.0014,   -0.4584,    0.1872,    5.0363,   -1.2410,   -0.6538,   -0.3637,
      0.1154,    0.0941,    1.3860,   -0.4313,   -3.8709,    0.0581,    0.3744,    0.6911,
     -0.6148,    0.2026,   -1.5188,    1.1467,    2.3713,   -0.5642,   -0.3996,    0.5937,
      0.5155,   -0.5027,   -0.3686,   -0.7539,   -0.1047,    0.7492,    0.0995,    0.3880,
     -0.7420,    0.9527,    1.5102,   -1.2262,   -6.3277,    1.4459,    1.2555,    0.3178,
      0.5749,   -0.6974,    0.0594,    0.0401,   -0.0194,    0.3967,    1.2691,   -1.0258,
  };
  */

  std::cerr << "Input matrix" << std::endl;
  Matrix A(8,8,m);
  A.print();

  std::cerr << "Computed inverse" << std::endl;
  A.inverse().print();

  std::cerr << "Matrix multiplied to matrix inverse" << std::endl;
  A.multiply(A.inverse()).print();

  return 0;
}

// test apply homography function
void testApplyHomography() {
  float hdata[] = {
    1.19897030, 0.00865164180, 95.0000000,
    0.144940136, 1.12265192, 170.000000,
    0.000806420365, 0.000255704864, 1.0,
  };
  Matrix H(3, 3, hdata);
  cout << "Given manual homography for green line example:" << endl;
  H.print();

  // apply homography and output image
  Image poster("Input/poster.png");
  Image green("Input/green.png");
  applyhomography(poster, green, H, false);
  green.write("Output/greenwithposter.png");
}

// test fast apply homography function
void testApplyHomographyFast() {
  std::clock_t start;


  float hdata[] = {
    1.19897030, 0.00865164180, 95.0000000,
    0.144940136, 1.12265192, 170.000000,
    0.000806420365, 0.000255704864, 1.0,
  };
  Matrix H(3, 3, hdata);
  cout << "Given manual homography for green line example:" << endl;
  H.print();

  // apply homography and output image
  Image poster("Input/poster.png");
  Image green("Input/green.png");
  start = std::clock();
  applyhomography(poster, green, H, false);
  float timediff = (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000);
  printf("Time taken with normal applyhomography: %fms\n", timediff);
  green.write("Output/greenwithposter_slow.png");

  // apply it fast
  green = Image("Input/green.png");
  start = std::clock();
  applyhomographyFast(poster, green, H, false);
  timediff = (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000);
  printf("Time taken with fast time: %fms\n", timediff);
  green.write("Output/greenwithposter_fast.png");
}

// test the poster:green-line example
void testComputeHomographyPoster() {
  Image poster("Input/poster.png");
  Image green("Input/green.png");

  // build 3D array (easier to initialize)
  float w = (float) poster.width();
  float h = (float) poster.height();
  float listOfPairs[4][2][3] = {
    {{0, 0, 1}, {95, 170, 1}},
    {{w, 0, 1}, {238, 171, 1}},
    {{w, h, 1}, {235, 233, 1}},
    {{0, h, 1}, {94, 239, 1}},
  };

  // Compute homography. You should manually make it's similar to that in testApplyHomography();
  Matrix Hcomputed = computeHomography(listOfPairs);
  cout << "Computed homography for green line example from given point pairs" << endl;
  Hcomputed.print();

  // print homography
  applyhomography(poster, green, Hcomputed, false);
  green.write("Output/greenwithposter_frompointpairs.png");
}

void testComputeTransformedBBox() {
  float hdata[] = {
    1.19897030, 0.00865164180, 95.0000000,
    0.144940136, 1.12265192, 170.000000,
    0.000806420365, 0.000255704864, 1.0,
  };
  Matrix H(3, 3, hdata);
  Image poster("Input/poster.png");
  vector<float> bbox = computeTransformedBBox(poster.width(), poster.height(), H);
  cout << "Computed transformed BBox for Poster:" << endl;
  cout << bbox[0] << " " << bbox[1] << " " << bbox[2] << " " << bbox[3] << " " << endl;

  // Students TODO: drawBoundingBox would be useful to implement for debugging
  // Image green("Input/green.png");
  // Image greenwithbox = drawBoundingBox(green, bbox);
  // greenwithbox.write("Output/greenwithposterbbox.png");
}

void testTranslate() {
  vector <float> bbox;
  bbox.push_back(-15);
  bbox.push_back(25);
  bbox.push_back(3);
  bbox.push_back(109);

  // look at translation matrix
  cout << "Translation Matrix:" << endl;
  Matrix m = translate(bbox);
  m.print();

  // transform top left of bounding box, see if it's [0,0].
  Matrix b(1, 3);
  b(0,0) = bbox[0];
  b(0,1) = bbox[2];
  b(0,2) = 1;
  cout << "Corner of translated Bounding Box:" << endl;
  m.multiply(b).print();
}

// test union
void testBBoxUnion() {
  vector <float> bbox1;
  bbox1.push_back(-15);
  bbox1.push_back(25);
  bbox1.push_back(3);
  bbox1.push_back(109);

  vector <float> bbox2;
  bbox2.push_back(-75);
  bbox2.push_back(19);
  bbox2.push_back(9);
  bbox2.push_back(20);

  vector <float> bbox  = bboxUnion(bbox1, bbox2);
  assert(bbox[0] == -75 && bbox[1] == 25 && bbox[2] == 3 && bbox[3] == 109,
    "Incorrect bboxUnion");
}

// test stitch with stata
void testStitchStata() {
  Image im1("Input/stata-1.png");
  Image im2("Input/stata-2.png");
  float listOfPairs[4][2][3] = {
    {{218,209, 1}, {4,232,1}},
    {{300,425,1}, {62,465,1}},
    {{337,209,1}, {125,247,1}},
    {{336, 396, 1}, {102, 433, 1}}
  };
  Image st = stitch(im1, im2, listOfPairs);
  st.write("./Output/stata-stitch.png");
}

// test stitch with Boston Skyline
void testStitchBostonSkyline() {
  // test boston
  Image boston1 = Image("Input/boston-skyline-1.png");
  Image boston2 = Image("Input/boston-skyline-2.png");
  float listOfPairsBoston[4][2][3] = {
    {{64,293,1},{6,201,1}},
    {{106,221,1},{45,130,1}},
    {{158,193,1},{98,102,1}},
    {{194,292,1},{130,195,1}},
  };

  Image boston = stitch(boston1, boston2, listOfPairsBoston);
  boston.write("./Output/boston-skyline-stitch.png");
}

// Your code **has to compile** on the submission system with all of the test cases
// uncommented! We will not grade the contents of this main function
int main() {
    testMatrixClass();  //  this should just work.

    // uncomment these test functions as you complete the assignment
    
    // testApplyHomography();         // test applying homographies
    // testComputeHomographyPoster(); // test compuing poster homography
    // testComputeTransformedBBox();  // test bounding box transformation
    // testTranslate();           // test translate Matrix
    // testBBoxUnion();           // test union of bounding boxes
    // testStitchStata();         // test final stitching
    // testStitchBostonSkyline(); // test final stitching

    // testApplyHomographyFast();     // test apply homography fast (6.865 or extra)

    return 0;
}
