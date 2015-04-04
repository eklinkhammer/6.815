#include "a0.h"

using namespace std;

void testReadFromFile() {
	Image my_im = readAnImage("Input/Boston_low_contrast.png");
  cout << my_im.channels() << endl;
  cout << my_im.height() << endl;
  cout << my_im.width() << endl;
  cout << my_im.number_of_elements() << endl;
	my_im.debug_write();
}

bool testSize() {
	Image my_im = readAnImage("Input/Boston_low_contrast.png");
	cout << my_im.number_of_elements() << endl;
 return true;
}

void testBright() {
  Image my_im = readAnImage("Input/Boston_low_contrast.png");
  //my_im.debug_write();
  Image brighter  = brightness(my_im, 2);
  brighter.debug_write();
}

void testContrast() {
	Image my_im = readAnImage("Input/Boston_low_contrast.png");
	Image betterContrast = contrast(my_im, 8, 0.5);
	betterContrast.debug_write();
}
// This is a way for you to test your functions. 
// We will only grade the contents of a0.cpp and Image.cpp
int main() {
  cout << "Congratulations, you have compiled the starter code!" << endl;
  //helloworld(1.0,2.0);
  //testReadFromFile();
  //testSize();
  testBright();
  testContrast();
}
