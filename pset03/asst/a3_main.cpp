#include "filtering.h"
#include <ctime>

using namespace std;


// test your box blurring fuctions
void testBoxBlur() {
    
    Image im("./Input/Cambridge2.png");
    int k=9; //set the width of your box to be 9 pixels (must be odd)
    
    // blur your image by doing a moving average
    Image boxBlur1 = boxBlur(im, k);
    boxBlur1.write("./Output/boxBlurred1.png");
    // blur your image in the same way using the Filter class
    Image boxBlur2 = boxBlur_filterClass(im, k);
    boxBlur2.write("./Output/boxBlurred2.png");
    // verify that your two funtion implementations obtain the
    // same result for the same value of k
    Image diffImg = (boxBlur1 - boxBlur2)/2 + 0.5;
    diffImg.write("./Output/boxBlurDiff.png");
    
}

// test that your convolution is properly flipping your kernel
void testShiftedImpulse() {
    
    Image im("./Input/pru.png");
    
    // create an impulse kernel shifted 1 pixel to the right and down
    // [ 0 0 0 ]
    // [ 0 0 0 ]
    // [ 0 0 1 ]
    float k = 3;
    vector<float> fData (k*k, 0.0);
    fData[k*k-1] = 1.0;
    Filter impulse(fData, k, k);
    
    // filter your image with this impulse kernel. Remmber that in convolution
    // you must flip your kernel. Thus, this should result in a copy of your image
    // shifted 1 pixel to the right and down.
    Image imFilter = impulse.Convolve(im);
    imFilter.write("./Output/impulseFiltered.png");
    
}

// test functions that extract gradient information from your image
void testGradient() {
    
    // create Sobel Filter that extracts horizontal gradients
    // [ -1 0 1 ]
    // [ -2 0 2 ]
    // [ -1 0 1 ]
    float fDataXArray[] = { -1.0, 0.0, 1.0, -2.0, 0.0, 2.0, -1.0, 0.0, 1.0 };
    vector<float> fDataX (fDataXArray, fDataXArray + sizeof(fDataXArray) / sizeof(float) );
    Filter sobelX(fDataX, 3, 3);
    
    // verify that your filter is correct by using it to filter an impulse image
    Image impulse = impulseImg(11); //create an image containing an impulse
    // convolve the impulse image with the Sobel kernel. We divide the output by 4 and
    // add 0.5 to make the range of the image between 0 and 1
    Image verifyKernel = sobelX.Convolve(impulse)/4 + 0.5;
    verifyKernel.write("./Output/verifySobelKernel.png");
    
    // filter an image using the sobel kernel
    Image im("./Input/lounge_view.png");
    Image sobelFiltered = sobelX.Convolve(im);
    
    // make the range of the output image from 0 to 1
    // since the Sobel filter changes the range of a (0,1) image to (-2,2)
    Image sobelOut = sobelFiltered/4 + 0.5;
    sobelOut.write("./Output/sobelFiltered.png");
    
    // compute the magnitude of the image using Sobel filters
    // that extract horizontal and vertical gradients
    Image imMag = gradientMagnitude(im);
    imMag.write("./Output/imageMagnitude.png");
}

// test filtering by a Gaussian kernel
void testGaussianFilters() {
    
    float sigma = 3.0; //set the standard deviation of the Gaussians
    time_t tstart;
    
    Image im("./Input/Cambridge2.png");
    
    // blur an image in just the X direction
    Image blurHorizontal = gaussianBlur_horizontal(im, sigma);
    blurHorizontal.write("./Output/gaussBlurHorizontal.png");
    
    // blur an image in 2D using a full 2D kernel
    tstart = time(0);
    Image blur2D = gaussianBlur_2D(im, sigma);
    // print the time it takes to run this function
    printf("Filtering with 2D Gaussian kernel took %3.2f seconds\n", difftime(time(0), tstart));
    blur2D.write("./Output/gaussBlur2D.png");

    // blur an image in 2D using 2 1D Gaussian kernels
    tstart = time(0);
    Image blur2DSeperable = gaussianBlur_seperable(im, sigma);
    // print the time it takes to run this function
    printf("Filtering using seperable Gaussian kernels took %3.2f seconds\n", difftime(time(0), tstart));
    blur2DSeperable.write("./Output/gaussBlur2D_seperable.png");
    
    // verify that both methods result in the same image
    Image diffImg = (blur2D - blur2DSeperable)/2 + 0.5;
    diffImg.write("./Output/gaussBlurDiff.png");

}

// test sharpening function
void testSharpen() {
    
    Image im("./Input/Cambridge1.png");
    
    Image sharp = unsharpMask(im,2.0,3.0,1.5);
    sharp.write("./Output/sharp.png");
    
}

// test bilaterial filtering
void testBilaterial() {
    
    Image im("./Input/lens.png");
    
    // Perform bilaterial filtering on an RGB image
    Image rgbBilatIm = bilateral(im);
    rgbBilatIm.write("./Output/bilaterial_RGB.png");
    
    // NOTE: Uncomment the code below if you have implemented bilaYUV
    // Perform bilaterial filtering with different domain sigmas for Y and UV
    Image yuvBilatIm = bilaYUV(im);
    yuvBilatIm.write("./Output/bilaterial_YUV.png");
    
}


// This is a way for you to test your functions.
// We will not grade the contents of the main function
int main() {
    
    // uncomment these test functions as you complete the assignment
    testBoxBlur();
    testShiftedImpulse();
    testGradient();
    testGaussianFilters();
    testSharpen();
    testBilaterial();

}
