#include "morphing.h"


using namespace std;

void testDistance() {
  Image bear("./Input/bear.png");
  Image output(bear.width(), bear.height(), 1);
  int i, j, k;
  float norm = sqrt(bear.height()^2 + bear.width()^2);
  Segment bar(0,0,10,10);
  for (i = 0; i < output.width(); i++) {
    for (j = 0; j < output.height(); j++) {
      float fi = (float) i; float fj = (float) j;
      float distance = bar.dist(fi,fj);
      for (k = 0; k < output.channels(); k++) {
        output(i,j,k) =  distance / sqrt(bear.height()*bear.height() + bear.width()*bear.width());
      }
    }
  }
  output.write("./Output/distance.png");
}
void testGetU() {
	float z = 0.0;
	float t = 2.0;
	float o = 1.0;
	Segment PQ(z,z,z,t);
	float u = PQ.getU(o,o);
	cout << u << endl;
}

void testGetV() {
	float z = 0.0;
	float t = 2.0;
	float o = 1.0;
	Segment PQ(z,z,z,t);
	float v = PQ.getV(o,o);
	cout << v << endl;
}

// test the smart accessor function
void testSmartAccessor(){
    
    // load an image and create 2 images that will test the smart accessor
    const Image input("./Input/bear.png");
    Image clampTrue(input.width(), input.height(), input.channels());
    Image clampFalse(input.width(), input.height(), input.channels());
    
    for (int z=0; z<input.channels(); z++){
        for (int x=0; x<input.width(); x++){
            for (int y=0; y<input.height(); y++){
                
                // replace non-valid pixel values with the value of the nearest pixel
                clampTrue(x,y,z) = input.smartAccessor(x-10, y-10, z, true);
                // replace non-valid pixel values with black (value=0)
                clampFalse(x,y,z) = input.smartAccessor(x-10, y-10, z);
            }
        }
    }
    
    clampTrue.write("./Output/smartAccessor_clampTrue.png");
    clampFalse.write("./Output/smartAccessor_clampFalse.png");
    
}

// a function to test scaling
void testScaling(){
    
    // load in the image and print out original size
    float fact = 2.253;
    const Image bostonim("./Input/BostonRainbow-crop-400.png");
    printf("Boston image is %dx%dx%d\n", bostonim.width(), bostonim.height(), bostonim.channels());
    
    // scale using NN interpolation and print the size of the new image
    Image scaledNN = scaleNN(bostonim, fact);
    scaledNN.write("./Output/BostonRainbow-scaled-NN.png");
    printf("Scaled-NN image is %dx%dx%d\n", scaledNN.width(), scaledNN.height(), scaledNN.channels());
    
    // scale using bilinear interpolation and print the size of the new image
    Image scaledLin = scaleLin(bostonim, fact);
    scaledLin.write("./Output/BostonRainbow-scaled-Lin.png");
    printf("Scaled-Lin image is %dx%dx%d\n", scaledLin.width(), scaledLin.height(), scaledLin.channels());
    
}

// a function to test rotation for those who have done it
void testRotation(){
    
    float theta = 3.141/4;
    
    const Image bostonim("./Input/BostonRainbow-crop-400.png");
    
    Image rot = rotate(bostonim, theta);
    rot.write("./Output/BostonRainbow-rotated.png");
    printf("Scaled-Lin image is %dx%dx%d\n", rot.width(), rot.height(), rot.channels());
    
}


// test warp by 1
void testWarpBy1(){
    
    Image bearim("./Input/bear.png");
    
    // define before and after segments
    Segment segBefore(0,0, 10,0);
    Segment segAfter(10, 10, 30, 15);
    
    Image warp1 = warpBy1(bearim, segBefore, segAfter);
    
    warp1.write("./Output/warpBy1.png");
    
}

void testWarpWith1() {
  Image bearim("./Input/bear.png");
  Segment segBefore(0,0,10,0);
  Segment segAfter(10,10,30,15);
  vector<Segment> segsBefore;
  segsBefore.push_back(segBefore);
  vector<Segment> segsAfter;
  segsAfter.push_back(segAfter);
  Image warp1 = warp(bearim, segsBefore, segsAfter);
  warp1.write("./Output/warp.png");
}


// a function to test your morphing function with the fredo and werewolf images
void testMorph(){
    
    // load the images
    Image fredo("./Input/fredo.png");
    Image werewolf("./Input/werewolf.png");
    
    // print out the size of the two images
    printf("Fredo image is %dx%dx%d\n", fredo.width(), fredo.height(), fredo.channels());
    printf("Werewolf image is %dx%dx%d\n", werewolf.width(), werewolf.height(), werewolf.channels());
    
    
    // define the segments corresponding to fredo (segsBefore) and the werewolf (segsAfter)
    vector<Segment> segsBefore, segsAfter;
    
    segsBefore.push_back(Segment(87, 128, 109, 127));
    segsBefore.push_back(Segment(143, 127, 162, 131));
    segsBefore.push_back(Segment(96, 197, 129, 190));
    segsBefore.push_back(Segment(118, 221, 132, 200));
    segsBefore.push_back(Segment(140, 238, 165, 170));
    segsBefore.push_back(Segment(71, 242, 44, 196));
    segsBefore.push_back(Segment(9, 46, 34, 14));
    segsBefore.push_back(Segment(175, 66, 154, 27));
    
    segsAfter.push_back(Segment(83, 114, 107, 109));
    segsAfter.push_back(Segment(139, 103, 157, 101));
    segsAfter.push_back(Segment(100, 170, 132, 151));
    segsAfter.push_back(Segment(125, 198, 145, 171));
    segsAfter.push_back(Segment(142, 196, 158, 139));
    segsAfter.push_back(Segment(96, 211, 75, 180));
    segsAfter.push_back(Segment(11, 41, 33, 7));
    segsAfter.push_back(Segment(175, 56, 155, 13));
    
    // create an image morphing between fredo and werewolf at time t=0.5
    vector<Image> imMorph = morph(fredo, werewolf, segsBefore, segsAfter);
    // write out images
    char buffer [50];
    for ( int n=0; n<3; n++){
        Image im = imMorph[n];
        sprintf (buffer, "./Output/fredo_werewolf_morph_%d.png", n);
        im.write(buffer);
    }
}

void morphStudents() {
  Image eric("./Input/Eric.png");
  Image min("./Input/Min.png");
  vector<Segment> segsBefore;
segsBefore.push_back(Segment(78, 93, 98, 91)); 
segsBefore.push_back(Segment(127, 90, 149, 94)); 
segsBefore.push_back(Segment(95, 111, 84, 134)); 
segsBefore.push_back(Segment(132, 110, 146, 129)); 
segsBefore.push_back(Segment(100, 160, 130, 161)); 
segsBefore.push_back(Segment(105, 79, 124, 79)); 
segsBefore.push_back(Segment(65, 120, 65, 145)); 
segsBefore.push_back(Segment(167, 127, 160, 153)); 
segsBefore.push_back(Segment(71, 67, 88, 56)); 
segsBefore.push_back(Segment(162, 79, 146, 62));
vector<Segment> segsAfter;
segsAfter.push_back(Segment(74, 83, 87, 81)); 
segsAfter.push_back(Segment(119, 78, 137, 78)); 
segsAfter.push_back(Segment(92, 102, 84, 113)); 
segsAfter.push_back(Segment(123, 100, 131, 115)); 
segsAfter.push_back(Segment(99, 133, 117, 130)); 
segsAfter.push_back(Segment(95, 68, 111, 67)); 
segsAfter.push_back(Segment(64, 104, 70, 124)); 
segsAfter.push_back(Segment(149, 94, 145, 115)); 
segsAfter.push_back(Segment(72, 56, 85, 43)); 
segsAfter.push_back(Segment(134, 59, 121, 43));
    vector<Image> imMorph = morph(eric, min, segsBefore, segsAfter, 13);
    // write out images
    char buffer [50];
    for ( int n=0; n<15; n++){
        Image im = imMorph[n];
        sprintf (buffer, "./Output/eric_min_morph_%d.png", n);
        im.write(buffer);
    }
}


// This is a way for you to test your functions.
// We will not grade the contents of the main function
int main() {
    //testSmartAccessor();
    //testScaling();
    //testRotation();
    //testWarpBy1();
    //testGetU();
    //testGetV();
    //testDistance();
    //testWarpWith1();
    //testMorph();
    morphStudents();
}
