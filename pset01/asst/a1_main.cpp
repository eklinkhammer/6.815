#include "a1.h"

using namespace std;

Image createTestImage(int width, int height) {
  Image output = Image(width, height, 3);
  int i, j, k;
	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++) {
			for (k = 0; k < 3; k++) {
				output(i,j,k) = i + j + k;
			}
		}
	}
	return output;
}

bool testChangeGamma() {
	cout << "Starting changeGamma test." << endl;
	const double old_gamma = 2;
	const double new_gamma = 3;
	Image input = createTestImage(10,10);
	Image output = changeGamma(input, old_gamma, new_gamma);
	bool regular = output(1,2,1) == 8;
	bool edge = output(0,0,1) == 1;
	bool result =  regular && edge;
	cout << "Result of test: " << result << endl;
	return result;
}

bool testExposure() {
	cout << "Starting exposure test." << endl;
	Image input = createTestImage(10,10);
	Image output = exposure(input, 2);
	bool scaled = output(1,1,1) - 4.11105 < 0.0001 && 4.11105 - output(1,1,1) < 0.0001;
	cout << "Result of test: " << scaled << endl;
	return scaled;
}

void testColor2Gray() {
  Image input = Image("Input/flower.png");
  input.debug_write();
  Image output = color2gray(input);
  output.debug_write();
}

void testBrightContrast() {
	Image input = Image("Input/flower.png");
	Image output = brightnessContrastLumi(input, 1, 1, 0.5);
	output.debug_write();
}

void testColorSpace() {
	Image input = Image("Input/flower.png");
	Image yuv = rgb2yuv(input);
	Image rgb = yuv2rgb(yuv);
	rgb.debug_write();
}

void testSat() {
	Image input = Image("Input/flower.png");
	Image saturated = saturate(input, 5);
	saturated.debug_write();
}

void testSpanish() {
	Image input = Image("Input/zebra.png");
	std::vector<Image> output = spanish(input);
	output[0].debug_write();
	output[1].debug_write();
}

void testGray() {
	Image input = Image("Input/flower.png");
	Image output = grayworld(input);
	output.debug_write();
}
// This is a way for you to test your functions. 
// We will only grade the contents of a1.cpp
int main() {
	//testChangeGamma();
	//testExposure();
	//testColor2Gray();
	//testBrightContrast();
	//testColorSpace();
	testSat();
	testSpanish();
	testGray();
	return 0;
}
