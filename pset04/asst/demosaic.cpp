// demosaic.cpp
// Assignment 4

#include "demosaic.h"
#include <math.h>
#include <assert.h>

using namespace std;


// PS04 - 3.1.1 - takes as input a raw image and returns a single-channel
// 2D image corresponding to the green channel using simple interpolation
Image basicGreen(const Image &raw, int offset){

	assert(offset == 1 || offset == 0);
	assert(raw.channels() >= 1);

	// Init output image
	int width, height, channels;
	width = raw.width();
	height = raw.height();
	Image output(width, height, 1);

	// If no offset, then 
	// G N G N G N G
	// N G N G N G N
	// G N G N G N G
	// Even rows have even greens
	// Odd rows have odd greens
	// With offset, even rows have odd greens, odd rows have even

	// Init loop variables
	int i, j;

	// Loop over output, filling in or interpolating green
	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++) {
			if (i > 1 && i < width - 1 && j > 1 && j < height -1 ) {
				if (i % 2 == 0) {
					output(i,j,0) = ((j + offset) % 2 == 0) ? raw(i,j,0) : interpGreen(raw,i,j);
				} else {
					output(i,j,0) = ((j + offset) % 2 != 0) ? raw(i,j,0) : interpGreen(raw,i,j);
				}
			} else {
				output(i,j,0) = raw(i,j,0);
			}
		}
	}
    return output;
}

float interpGreen(const Image &raw, int x, int y) {
	assert(x > 0 && x < raw.width() - 1);
	assert(y > 0 && y < raw.height() - 1);

	float interpVal = raw(x + 1, y) +
					  raw(x - 1, y) +
					  raw(x, y + 1) +
					  raw(x, y - 1);
	interpVal /= 4;
	return interpVal;
}
// PS04 - 3.2.1 - takes as input a raw image and returns a single-channel
// 2D image corresponding to the red or blue channel using simple interpolation
Image basicRorB(const Image &raw, int offsetX, int offsetY){

	assert(offsetX == 1 || offsetX == 0);
	assert(offsetY == 1 || offsetY == 0);
	assert(raw.channels() >= 1);

	// Init output image
	int width, height, channels;
	width = raw.width();
	height = raw.height();
	Image output(width, height, 1);

	// If no offset, then 
	// B N B N 
	// N N N N
	// B N B N
	// N N N N
	// Even rows have even blues
	// Odd rows have none
	// if row + offset is even, then when colum + offset is even, have value

	// Init loop variables
	int i, j;

	// Loop over output, filling in or interpolating green
	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++) {
			if (i > 1 && i < width - 2 && j > 1 && j < height -2 ) {
				// If on an even row, color values exist on row
				if ( (i + offsetX)  % 2 == 0) {
					output(i,j,0) = ((j + offsetY) % 2 == 0) ? raw(i,j,0) : interpRedBlue(raw,i,j, offsetX, offsetY);
				} else {
					output(i,j,0) = interpRedBlue(raw,i,j, offsetX, offsetY);
				}
			} else {
				output(i,j,0) = raw(i,j,0);
			}
		}
	}
    return output;
}

float interpRedBlue(const Image &raw, int i, int j, int offsetX, int offsetY) {
	// if on a row with colors
	if ( (i + offsetX) % 2 == 0) {
		return ((raw(i,j-1,0) + raw(i,j+1,0)) / 2);
	} else {  // if on a row with no colors
		// if above and below are colored
		if ((j + offsetY) % 2 == 0) {
			return ((raw(i-1,j,0) + raw(i+1,j,0)) / 2);
		} else { // use 4 diagonals
			return ((raw(i-1,j-1,0) + raw(i-1,j+1,0) + raw(i+1,j-1,0) + raw(i+1,j+1,0)) / 4);
		}
	}
}
// PS04 - 3.2.2 - takes as input a raw image and returns an rgb image
// using simple interpolation to demosaic each of the channels
Image basicDemosaic(const Image &raw, int offsetGreen, int offsetRedX, int offsetRedY, int offsetBlueX, int offsetBlueY){

	Image green = basicGreen(raw, offsetGreen);
	Image red = basicRorB(raw, offsetRedX, offsetRedY);
	Image blue = basicRorB(raw, offsetBlueX, offsetBlueY);


    Image output = mergeChannels(red, green, blue);
    return output;
}

Image mergeChannels(const Image &red, const Image &green, const Image &blue) {
	int width, height;
	width = red.width();
	height = red.height();
	assert(width == green.width());
	assert(height == green.height());
	assert(width == blue.width());
	assert(height == blue.height());

	Image output(width, height, 3);

	int i, j;
	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++) {
			output(i,j,0) = red(i,j,0);
			output(i,j,1) = green(i,j,0);
			output(i,j,2) = blue(i,j,0);
		}
	}
	return output;
}
// PS04 - 4.1 - takes a raw image and outputs a single-channel
// image corresponding to the green channel taking into account edges
Image edgeBasedGreen(const Image &raw, int offset){
    assert(offset == 1 || offset == 0);
	assert(raw.channels() >= 1);

	// Init output image
	int width, height, channels;
	width = raw.width();
	height = raw.height();
	Image output(width, height, 1);

	// Gradient is largest between pixels that represent different the boundary.
	// The color of the boundary should be the color of the adjacent pixels (along the boundary)
	// Pick the average of the colors that have the SMALLER difference

	// Init loop variables
	int i, j;

	// Loop over output, filling in or interpolating green
	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++) {
			if (i > 0 && i < width - 1 && j > 0 && j < height -1 ) {
				if (i % 2 == 0) {
					output(i,j,0) = ((j + offset) % 2 == 0) ? raw(i,j,0) : interpEdgeGreen(raw,i,j);
				} else {
					output(i,j,0) = ((j + offset) % 2 != 0) ? raw(i,j,0) : interpEdgeGreen(raw,i,j);
				}
			} else {
				output(i,j,0) = raw(i,j,0);
			}
		}
	}
    return output;
}

float interpEdgeGreen(const Image &raw, int x, int y) {
	assert(x > 0 && x < raw.width() - 1);
	assert(y > 0 && y < raw.height() - 1);

	float dx, dy, val;
	dx = raw(x,y+1,0) - raw(x, y-1,0);
	dy = raw(x+1, y, 0) - raw(x-1,y,0);
	if (dx > dy) {
		val = raw(x+1,y,0) + raw(x-1,y,0);
	} else {
		val = raw(x,y+1,0) + raw(x,y-1,0);
	}
	val /= 2;
	return val;
}

// PS04 - 4.2 - takes as input a raw image and returns an rgb image
// using edge-based green demosaicing for the green channel and
// simple interpolation to demosaic the red and blue channels
Image edgeBasedGreenDemosaic(const Image &raw, int offsetGreen, int offsetRedX, int offsetRedY, int offsetBlueX, int offsetBlueY){
    Image green = edgeBasedGreen(raw, offsetGreen);
	Image red = basicRorB(raw, offsetRedX, offsetRedY);
	Image blue = basicRorB(raw, offsetBlueX, offsetBlueY);


    Image output = mergeChannels(red, green, blue);
    return output;
}


// PS04 - 5.1 - takes as input a raw image and returns a single-channel
// 2D image corresponding to the red or blue channel using green based interpolation
Image greenBasedRorB(const Image &raw, Image &green, int offsetX, int offsetY){
    
	assert(offsetX == 1 || offsetX == 0);
	assert(offsetY == 1 || offsetY == 0);
	assert(raw.channels() >= 1);

	// Init output image
	int width, height, channels;
	width = raw.width();
	height = raw.height();
	Image output(width, height, 1);

	// Init loop variables
	int i, j;

	// Loop over output, filling in or interpolating green
	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++) {
			if (i > 1 && i < width - 2 && j > 1 && j < height -2 ) {
				// If on an even row, color values exist on row
				if ( (i + offsetX)  % 2 == 0) {
					output(i,j,0) = ((j + offsetY) % 2 == 0) ? raw(i,j,0) : interpGreenBasedRedBlue(raw, green, i,j, offsetX, offsetY);
				} else {
					output(i,j,0) = interpGreenBasedRedBlue(raw, green, i,j, offsetX, offsetY);
				}
			} else {
				output(i,j,0) = raw(i,j,0);
			}
		}
	}
    return output;
}

float interpGreenBasedRedBlue(const Image &raw, const Image &green, int i, int j, int offsetX, int offsetY) {
	// if on a row with colors
	float estGreen = green(i,j,0);
	float diffRG;
	if ( (i + offsetX) % 2 == 0) {
		diffRG = ((raw(i,j-1,0) - green(i,j-1,0) + raw(i,j+1,0) - green(i,j+1,0)) / 2);
	} else {  // if on a row with no colors
		// if above and below are colored
		if ((j + offsetY) % 2 == 0) {
			diffRG =  ((raw(i-1,j,0) - green(i-1,j,0) + raw(i+1,j,0) - green(i+1,j,0)) / 2);
		} else { // use 4 diagonals
			diffRG = ((raw(i-1,j-1,0) - green(i-1,j-1,0) + raw(i-1,j+1,0) - green(i-1,j+1,0) 
				+ raw(i+1,j-1,0) - green(i+1,j-1,0) + raw(i+1,j+1,0) - green(i+1,j+1,0)) / 4);
		}
	}

	return estGreen + diffRG;
}
// PS04 - 5.2 - takes as input a raw image and returns an rgb image
// using edge-based green demosaicing for the green channel and
// simple green based demosaicing of the red and blue channels
Image improvedDemosaic(const Image &raw, int offsetGreen, int offsetRedX, int offsetRedY, int offsetBlueX, int offsetBlueY){
    Image green = edgeBasedGreen(raw, offsetGreen);
	Image red = greenBasedRorB(raw, green, offsetRedX, offsetRedY);
	Image blue = greenBasedRorB(raw, green, offsetBlueX, offsetBlueY);


    Image output = mergeChannels(red, green, blue);
    return output;
}
