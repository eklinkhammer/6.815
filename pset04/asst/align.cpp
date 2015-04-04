// align.cpp
// Assignment 4


#include "align.h"
#include <math.h>
#include <assert.h>
#include <limits>
using namespace std;

// PS04 - 2.1.1 - Basic denoising by computing the average of a sequence of images
Image denoiseSeq(const vector<Image> &imSeq){

    int size = imSeq.size();
    assert(size > 0);

    // Initialize output image
    int width, height, channels;
    width = imSeq[0].width();
    height = imSeq[0].height();
    channels = imSeq[0].channels();
    Image output(width, height, channels);

    // Create loop variables
    int i, j, k, images;
    float sum;

    // Loop over output; Average sequence
    for (i = 0; i < width; i++) {
        for (j = 0; j < height; j++) {
            for (k = 0; k < channels; k++) {
                sum = 0;
                for (images = 0; images < size; images++) {
                    sum += imSeq[images](i,j,k);
                }
                output(i,j,k) = sum / size;
            }
        }
    }

    return output;
}


// PS04 - 2.2.1 - returns an image visualizing the per-pixel and
// per-channel log of the signal-to-noise ratio scaled by scale.
Image logSNR(const vector<Image> &imSeq, float scale){

    int size = imSeq.size();
    assert(size > 0);
    size = ( size > 1 ) ? size : 2; // so variance formula works

    // Initialize output image and mean image
    int width, height, channels;
    width = imSeq[0].width();
    height = imSeq[0].height();
    channels = imSeq[0].channels();

    Image output(width, height, channels);

    // Create loop variables
    int i, j, k, images; 
    float variance, meanVal, meanVal2, EX2, val;

    vector<Image> imSeq2 (size,0);
    for (images = 0; images < size; images++) {
        Image newImage2(width, height, channels);

        for (i = 0; i < width; i++) {
            for (j = 0; j < height; j++) {
                for (k = 0; k < channels; k++) {
                    newImage2(i,j,k) = pow(imSeq[images](i,j,k),2);
                }
            }
        }
        imSeq2[images] = newImage2;
    }

    // E[x^2]
    Image u2Image = denoiseSeq(imSeq2);
    // E[x]
    Image uImage = denoiseSeq(imSeq);

    // Loop over output; Average sequence
    for (i = 0; i < width; i++) {
        for (j = 0; j < height; j++) {
            for (k = 0; k < channels; k++) {
                meanVal = uImage(i,j,k);
                meanVal2 = pow(meanVal, 2);

                variance = u2Image(i,j,k) - meanVal2;
                variance = (variance * size) / (size - 1);
                variance = (0.000001 > variance) ? 0.000001 : variance;

                val = meanVal2 / variance;
                val = scale * 10 * log10 (val);
                output(i,j,k) = val;
            }
        }
    }
    return output;
}


// PS04 - 2.3.1 -  returns the (x,y) offset that best aligns im2 to match im1.
vector<int> align(const Image &im1, const Image &im2, int maxOffset){

    // Image init
    int height, width, channels;
    height = im1.height();
    width = im1.width();
    channels = im1.channels();

    vector<int> min_offset_index (2,0);

    float minDifference = std::numeric_limits<float>::max();
    float norm;

    int i, j, w, h, c;
    // For each offset, roll image.
    for (i = -maxOffset; i <= maxOffset; i++) {
        for (j = -maxOffset; j <= maxOffset; j++) {

            Image temp = roll(im2, i, j);
            norm = 0;   
            // For each pixel, determine square of norm of difference
            // Ignore all pixels within maxoffset of edges
            for (w = maxOffset; w < width - maxOffset; w++) {
                for (h = maxOffset; h < height - maxOffset; h++) {
                    for (c = 0; c < channels; c++) {
                        norm += pow(im1(w,h,c) - temp(w,h,c), 2);
                    }
                }
            }

            // Update minDifference and offset values if new min found
            if ( norm < minDifference ) {
                minDifference = norm;
                min_offset_index[0] = i;
                min_offset_index[1] = j;
            }
        }
    }

    return min_offset_index;
}

// PS04 - 2.3.2 -  registers all images to the first one in a sequence and outputs
// a denoised image even when the input sequence is not perfectly aligned.
Image alignAndDenoise(const vector<Image> &imSeq, int maxOffset){

    int size = imSeq.size();
    Image first = imSeq[0];
    vector<Image> aligned;

    int image;
    for (image = 1; image < size; image++) {
        Image temp = imSeq[image];
        vector<int> offset = align(first, temp, maxOffset);
        temp = roll(temp, offset[0], offset[1]);
        aligned.push_back(temp);
    }
    return denoiseSeq(aligned);
}

// PS04 - 6.1.1 -  6.865 only:
// split a Sergey images to turn it into one 3-channel image.
Image split(const Image &sergeyImg){
    return Image(0); // change this
}

// PS04 - 6.1.2 -  6.865 only:
// aligns the green and blue channels of your rgb channel of a sergey
// image to the red channel. This should return an aligned RGB image
Image sergeyRGB(const Image &sergeyImg, int maxOffset){
    return Image(0); // change this 
}


/**************************************************************
 //               DON'T EDIT BELOW THIS LINE                //
 *************************************************************/

// This circularly shifts an image by xRoll in the x direction and
// yRoll in the y direction. xRoll and yRoll can be negative or postive
Image roll(const Image &im, int xRoll, int yRoll){
    
    int xNew, yNew;
    Image imRoll(im.width(), im.height(), im.channels());
    
    // for each pixel in the original image find where its corresponding
    // location is in the rolled image
    for (int x=0; x<im.width(); x++){
        for (int y=0; y<im.height(); y++){
            
            // use modulo to figure out where the new location is in the
            // rolled image. Then take care of when this returns a negative number
            xNew = (x + xRoll) % im.width();
            yNew = (y + yRoll) % im.height();
            xNew = (xNew<0)*(imRoll.width() + xNew) + (xNew>=0)*xNew;
            yNew = (yNew<0)*(imRoll.height() + yNew) + (yNew>=0)*yNew;
            
            // assign the rgb values for each pixel in the original image to
            // the location in the new image
            for (int z=0; z<im.channels(); z++){
                imRoll(xNew, yNew, z) = im(x,y,z);
            }
        }
    }
    
    // return the rolled image
    return imRoll;
}
