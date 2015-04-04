#include <iostream>
#include <vector>
#include "Image.h"
#include "messageassert.h"
using namespace std;

// Print the sum of two numbers
void helloworld(float a, float b);

// Create an image and return it
Image readAnImage(const std::string &filename);

Image brightness(const Image &im, const float &factor);

Image contrast(const Image &im, const float &factor, const float &midpoint = 0.5);
