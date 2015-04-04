#include "Image.h"
// All functions students need to implement are here at the top

using namespace std;
// Return the total number of floats stored in the array.
// There are a number of ways to implement this.
// You can look at the size of image_data or use the extent function
long long Image::number_of_elements()const {
  return image_data.size();
}

// Accessors and Setters
const float & Image::operator()(int x) const {
  // return the pixel at location x, where x spans all locations and color channels
  // throw OutOfBoundsException if x is negative or larger than the number of elements
  if (!( x < number_of_elements() && x >= 0 )) {
    throw OutOfBoundsException();
  }
  return image_data[x];
}

// Remember to do bounds checking
const float & Image::operator()(int x, int y) const {
  // Return x,y in the channel 0 and throw out of bounds if the input is invalid
  // Assuming the image is not a gray scale image
  int index = y*width() + x;
  int size_per_channel = width()*height();
  if (!(index < number_of_elements() && index >= 0 && index < size_per_channel)) {
    throw OutOfBoundsException();
  }
  return image_data[index];
}

const float & Image::operator()(int x, int y, int z) const {
  // Return the value in x,y,c and throw out of bounds if the input is invalid
  int index = z*width()*height() + y*width() + x;
  if (!(index < number_of_elements() && index >= 0)) {
    throw OutOfBoundsException();
  }
  return image_data[index];
}

// The next three functions should have the same implementation as the previous three
float & Image::operator()(int x) {
  if (!( x < number_of_elements() && x >= 0 )) {
    throw OutOfBoundsException();
  }
  return image_data[x];
}

float & Image::operator()(int x, int y) {
  int index = y*width() + x;
  int size_per_channel = width()*height();
  if (!(index < number_of_elements() && index >= 0 && index < size_per_channel)) {
    throw OutOfBoundsException();
  }
  return image_data[index];
}

float & Image::operator()(int x, int y, int z) {
  int index = z*width()*height() + y*width() + x;
  if (!(index < number_of_elements() && index >= 0)) {
    throw OutOfBoundsException();
  }
  return image_data[index];
}




// ********************************************************//
//               DON'T EDIT BELOW THIS LINE                //
// ********************************************************//

int Image::debugWriteNumber = 0;

Image::Image(int x, int y, int z, const std::string &name_) {
  initialize_image_metadata(x,y,z,name_);
  long long size_of_data = 1;
  for (int k = 0; k < dimensions(); k++) {
    size_of_data *= dim_values[k];
  }
  image_data = std::vector<float>(size_of_data,0);

}

void Image::initialize_image_metadata(int x, int y, int z,  const std::string &name_) {
  dim_values[0] = 0;
  dim_values[1] = 0;
  dim_values[2] = 0;
  stride_[0] = 0;
  stride_[1] = 0;
  stride_[2] = 0;
  gamma_ = 2.2; // This is what most images start out as
  dims = 0;
  long long size_of_data = 1;
  if ( x < 0 )
    throw NegativeDimensionException();
  if ( y< 0)
    throw NegativeDimensionException();
  if (z < 0 )
    throw NegativeDimensionException();

  image_name = name_;


  dims++;
  dim_values[0] = x;
  size_of_data *= x;
  stride_[0] = 1;
  if (y > 0 ) {
      dims++;
      dim_values[1] = y;
      size_of_data *= y;
      stride_[1] = x;
  } else {
    return;
  }
  if (z>0)  {
    dims++;
    dim_values[2] =z;
    size_of_data *= z;
    stride_[2] = x*y;
  } else {
    return;
  }
}

Image::Image(const std::string & filename) {
  std::vector<unsigned char> uint8_image;
  unsigned int height_;
  unsigned int width_;
  unsigned int channels_ = 4;
  unsigned int outputchannels_ = 3; // Throw away transparency
  lodepng::decode(uint8_image, width_, height_, filename.c_str()); // In column major order with packed color values

  image_data = std::vector<float>(height_*width_*outputchannels_,0);
  for (unsigned int x= 0; x < width_; x++) {
    for (unsigned int y = 0; y < height_; y++) {
      for (unsigned int c = 0; c < outputchannels_; c++) {
        image_data[x+y*width_+c*width_*height_] = uint8_to_float(uint8_image[c + x*channels_ + y*channels_*width_]);
      }
    }
  }

  initialize_image_metadata(width_, height_, outputchannels_, filename);
}

Image::~Image() { } // Nothing to clean up





void Image::write(const std::string &filename) const {
  if (channels() != 1 && channels() != 3 && channels() != 4)
    throw ChannelException();
  int png_channels = 4;
  std::vector<unsigned char> uint8_image(height()*width()*png_channels, 255);
  int c;
  for (int x= 0; x < width(); x++) {
    for (int y = 0; y < height(); y++) {
      for (c = 0; c < channels(); c++) {
	uint8_image[c + x*png_channels + y*png_channels*width()] = float_to_uint8(image_data[x+y*width()+c*width()*height()]);
      }
      for ( ; c < 3; c++) { // Only executes when there is one channel

	uint8_image[c + x*png_channels + y*png_channels*width()] = float_to_uint8(image_data[x+y*width()+0*width()*height()]);
      }
    }
  }
  lodepng::encode(filename.c_str(), uint8_image, width(), height());
}

void Image::debug_write() const {
  std::ostringstream ss;
  ss << "./Output/" <<  debugWriteNumber << ".png";
  std::string filename = ss.str();
  write(filename);
  debugWriteNumber++;

}

float Image::uint8_to_float(const unsigned char &in) {
  return ((float) in)/(255.0f);
}

unsigned char Image::float_to_uint8(const float &in) {
  float out = in;
  if (out < 0)
    out = 0;
  if (out > 1)
    out = 1;
  return (unsigned char) (255.0f*out);

}





