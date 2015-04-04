#include "align.h"
#include "demosaic.h"
#include <ctime>

using namespace std;


// test denoising function by averaging function and visualizing SNR
void testDenoiseSeq() {
    
    // load an image sequence
    vector<Image> imSeq;
    int nImages = 50;
    char baseName[] = "./Input/aligned-ISO3200/1D2N-iso3200-";
    char fileName [100];
    
    // add each image to the vector of images
    for (int i=1; i<=nImages; i++){
        sprintf(fileName, "%s%d.png", baseName, i);
        imSeq.push_back( Image(fileName) );
    }
    
    
    // denoise by averaging across all of the images
    Image imDenoise = denoiseSeq(imSeq);
    imDenoise.write("./Output/denoisedImg.png");
    
    // estimate the SNR of the image sequence
    Image SNRIm = logSNR(imSeq);
    SNRIm.write("./Output/snrImg.png"); 
    
}

// test your function to find the optimal offset between 2 images
void testOffset(){
    
    // load the images that are not aligned
    Image im1("./Input/green/noise-small-1.png");
    Image im2("./Input/green/noise-small-2.png");
    
    // get the best offset for these 2 images
    int maxOffset = 10;
    vector<int> offset = align(im1, im2, maxOffset);
    
    // print out the offset values that best aligns im2 to im1
    cout << "x offset: " << offset[0] << " y offset: " << offset[1] << endl;
}

// test your denoising function by aligned averaging
void testDenoiseShiftSeq() {
    
    // load an image sequence
    vector<Image> imSeq;
    int nImages = 9;
    char baseName[] = "./Input/green/noise-small-";
    char fileName [100];
    
    // add each image to the vector of images
    for (int i=1; i<=nImages; i++){
        sprintf(fileName, "%s%d.png", baseName, i);
        imSeq.push_back( Image(fileName) );
    }
    
    // denoise using just pure averaging - this should result in a blurry output
    Image imDenoiseBlur = denoiseSeq(imSeq);
    imDenoiseBlur.write("./Output/denoisedImg_Avg.png");
    
    // denoise by averaging aligned images
    Image imDenoiseShift = alignAndDenoise(imSeq, 5);
    imDenoiseShift.write("./Output/denoisedImg_AlignedAvg.png");
    
}

// test your demosaicing functions that use simple interpolation
void testBasicDemosaic() {
    
    // load raw image (notice that although this is a 3 channel image each channel
    // has the same value so you can think of it as having only 1 channel)
    Image raw("./Input/raw/signs-small.png");
    
    // demosaic the green channel
    Image green = basicGreen(raw);
    green.write("./Output/green_basic.png");
    
    // demosaic the red channel
    Image red = basicRorB(raw, 1, 1);
    red.write("./Output/red_basic.png");
    
    // demosaic the blue channel
    Image blue = basicRorB(raw, 0, 0);
    blue.write("./Output/blue_basic.png");
    
    // generate a full rgb image by demaosicing each of the channels
    // and concatinating them together
    Image rgb = basicDemosaic(raw);
    rgb.write("./Output/rgb_basic.png");
}


// test your demosaicing functions that use the green channel to incoperate information
// about edges in your image
void testGreenEdgeDemosaic() {
    
    // load raw image (notice that although this is a 3 channel image each channel
    // has the same value so you can think of it as having only 1 channel)
    Image raw("./Input/raw/signs-small.png");
    
    // demosaic the green channel using the edge-based method
    Image green = edgeBasedGreen(raw);
    green.write("./Output/green_edge.png");
    
    // generate the rgb image using edge based demosaicing algorithm for the
    // green channel but the naive red and blue demosaicing algorithm
    Image rgb_greenEdge = edgeBasedGreenDemosaic(raw);
    rgb_greenEdge.write("./Output/rgb_greenEdge.png");
    
    // generate the rgb image using the edge based demosaicing algorithm
    // for the green, red, and blue channels
    Image rgb_fullEdge = improvedDemosaic(raw);
    rgb_fullEdge.write("./Output/rgb_fullEdge.png");
    
}

// 6.865 only (or 5% extra credit)
// test your functions to generate an rgb image from the Sergey images
void testSergey(){
    
    // load a sergey image
    Image sergeyImg("./Input/Sergey/00088v_third.png");
    
    // split the grayscale image to generate a rgb image
    Image rgb = split(sergeyImg);
    rgb.write("./Output/sergeyRGB.png");
    
    // split the grayscale image and align the channels to
    // generate a clean RGB image
    Image rgbAlign = sergeyRGB(sergeyImg);
    rgbAlign.write("./Output/sergeyRGB_align.png");
}



// This is a way for you to test your functions.
// We will not grade the contents of the main function
int main() {
    
    // uncomment these test functions as you complete the assignment
    //testDenoiseSeq();
    //testOffset();
    //testDenoiseShiftSeq();
    //testBasicDemosaic();
    //testGreenEdgeDemosaic();
    //testSergey();
    
}
