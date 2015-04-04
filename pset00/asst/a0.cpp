#include "a0.h"

using namespace std;

// Print the sum of two numbers
void helloworld(float a, float b) {
  // Create a variable c that is the sum of a and b
  // Use cout to display the values
  float c = a + b;
  cout << "Hello World!" << endl;
  cout << "The value of a is " << a << "." << endl;
  cout << "The value of b is " << b << "." << endl;
  cout << "The sum of a and b is " << c << "." << endl;
}

// Create an image and return it
// You can test your function by calling my_im.write(filename) to write the output somewhere
Image readAnImage(const std::string &filename) {
  // Use the constructor in Image.h
  Image fromFile = Image(filename);
  return fromFile;
}



// Change the brightness of the image
// const Image & means a reference to im wil get passed to you, 
// but the compiler won't let you modify it
Image brightness(const Image &im, const float &factor) {
  // Image output(im.width(), im.height(), im.channels());
  // Modify image brightness
  // return output;
  if (factor < 0) {
    throw OutOfBoundsException();
  }
  Image output = Image(im.width(), im.height(), im.channels());
  int w,h,c;
  for (w = 0; w < output.width(); w++) {
    for (h = 0; h < output.height(); h++) {
      for (c = 0; c < output.channels(); c++) {
        output(w,h,c) = im(w,h,c)*factor;
      }
    }
  }
  return output;
}

Image contrast(const Image &im, const float &factor, const float &midpoint) {
  // Image output(im.width(), im.height(), im.channels());
  // Modify image contrast
  // return output;
  if ( midpoint < 0 || midpoint > 1 || factor < 0 ) {
    throw OutOfBoundsException();
  }

  Image output = Image(im.width(), im.height(), im.channels());
  int x, y, z;
  for (x = 0; x < im.width(); x++) {
    for (y = 0; y < im.height(); y++) {
      for (z = 0; z < im.channels(); z++) {
        output(x,y,z) = factor * ( im(x,y,z) - midpoint) + midpoint;
      }
    }
  }
  return output;
}
