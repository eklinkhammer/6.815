#include "homography.h"
#include "panorama.h"
#include "blending.h"
#include "messageassert.h"
#include "matrix.h"
#include <ctime>
// #include <unistd.h>

using namespace std;

// test blending weight
void testBlendingWeight() {
  Image boston("Input/boston-sunset-2.png");
  cout << boston.channels() << endl;
  Image w = blendingweight(boston.width(), boston.height());
  w.write("Output/boston-sunset-2-weight.png");
}

// test apply homography function
void testApplyHomographyBlend() {
  float hdata[] = {
    1.19897030, 0.00865164180, 95.0000000,
    0.144940136, 1.12265192, 170.000000,
    0.000806420365, 0.000255704864, 1.0,
  };
  Matrix H(3, 3, hdata);

  // apply homography and output image
  Image poster("Input/poster.png");
  Image green("Input/green.png");

  Image w = blendingweight(poster.width(), poster.height());
  w.write("Output/posterWeight.png");

  applyhomographyBlend(poster, w, green, H, false);
  green.write("Output/greenwithposterblendweight.png");

  green = Image("Input/green.png");
  applyhomographyBlend(poster, w*0+0.5, green, H, false);
  green.write("Output/greenwithposter05weight.png");
}

// test stitch with stata
void testStitchBlendStata() {
  Image im1("Input/stata-1.png");
  Image im2("Input/stata-2.png");
  float hdata[] = {
    1.30954, -0.174891, -244.261,
    0.390429, 1.1775, -60.5194,
    0.000985249, -0.000229697,    1,
  };
  Matrix H(3, 3, hdata);
  stitchBlending(im1, im2, H, 0).write("./Output/stata-stitch-noblending.png");
  stitchBlending(im1, im2, H, 1).write("./Output/stata-stitch-linearblending.png");
  stitchBlending(im1, im2, H, 2).write("./Output/stata-stitch-2scaleblending.png");
}

// test autostitchers
void testAutoStitchBlendStata() { // stata
  Image stata1("./Input/stata-1.png");
  Image stata2("./Input/stata-2.png");
  autostitch(stata1, stata2, 1).write("./Output/Stata-autostitch-linearblending.png");
  autostitch(stata1, stata2, 2).write("./Output/Stata-autostitch-2scaleblending.png");
}
void testAutoStitchBlendBoston() { // boston
  Image boston1("./Input/boston-sunset-1.png");
  Image boston2("./Input/boston-sunset-2.png");
  autostitch(boston1, boston2, 1).write("./Output/Boston-autostitch-linearblending.png");
  autostitch(boston1, boston2, 2).write("./Output/Boston-autostitch-2scaleblending.png");
}
void testAutoStitchBlendCastle() { // castle
  Image castle1("./Input/guedelon-1.png");
  Image castle2("./Input/guedelon-2.png");
  autostitch(castle1, castle2, 1).write("./Output/guedelon-autostitch-linearblending.png");
  autostitch(castle1, castle2, 2).write("./Output/guedelon-autostitch-2scaleblending.png");
}

// test plane
void testPano2Planet() {
  Image im("./Input/boston_winter.png");
  Image world = pano2planet(im, 500);
  world.write("./Output/boston_winter_planet.png");

  Image mars("./Input/mars_pano.png");
  pano2planet(mars, 300).write("./Output/mars_planet.png");

}

// 6.865 - N stitch - Boston
void testAutoStitchNBoston() {
  vector<Image> ims;
  ims.push_back(Image("./Input/boston-sunset-1.png"));
  ims.push_back(Image("./Input/boston-sunset-2.png"));
  ims.push_back(Image("./Input/boston-sunset-3.png"));
  autostitchN(ims, 1).write("./Output/boston-autostitchN.png");
}

// 6.865 - N stitch - Castle
void testAutoStitchNCastle() {
  vector<Image> ims;
  ims.push_back(Image("./Input/guedelon-1.png"));
  ims.push_back(Image("./Input/guedelon-2.png"));
  ims.push_back(Image("./Input/guedelon-3.png"));
  autostitchN(ims, 1).write("./Output/guedelon-autostitchN.png");
}

// Your code **has to compile** on the submission system with all of the test cases
// uncommented! We will not grade the contents of this main function
int main() {

  // Blending
  //testBlendingWeight();
  //testApplyHomographyBlend();
  //testStitchBlendStata();

  // AutoStitch with Blending
  //testAutoStitchBlendStata();
  //testAutoStitchBlendBoston();
  //testAutoStitchBlendCastle();

  // Planet
  testPano2Planet();

  // N - stitch
  //testAutoStitchNBoston();
  //testAutoStitchNCastle();

  return 0;
}
