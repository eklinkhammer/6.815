#include "Image.h"


using namespace std;

//PS02 - 2.0.1: Safe Accessor that will return a black pixel (clamp = false) or the
//nearest pixel value (clamp = true) when indexing out of the bounds of the image
float Image::smartAccessor(int x, int y, int z, bool clamp) const{
  if (x < width() && y < height() && z < channels() && x >= 0 && y >= 0 && z >= 0) {
    return (*this)(x,y,z);
  }
  
  int newX, newY;
  if (clamp) {
    newX = (x < width()) ? x : width() - 1;
    newY = (y < height()) ? y : height() - 1;
    newX = (newX >= 0) ? newX : 0;
    newY = (newY >= 0) ? newY : 0;
    return (*this)(newX, newY, z);
  } else {
    return 0;
 }
}


/**************************************************************
 //               DON'T EDIT BELOW THIS LINE                //
 //          Functions from previous problem sets           //
 *************************************************************/


// You'll get the implementation of these functions after the pset 0 deadline
long long Image::number_of_elements()const {
  return image_data.size();
}


// Accessors and Setters
const float & Image::operator()(int x) const {
  if (x < 0 || x>= number_of_elements())
    throw OutOfBoundsException();
  return image_data[x];
}

const float & Image::operator()(int x, int y) const {
  if (x < 0 || x >= width())
    throw OutOfBoundsException();
  if ( y < 0 || y >= height())
    throw OutOfBoundsException();

  return image_data[x*stride_[0]+y*stride_[1]];

}

const float & Image::operator()(int x, int y, int z) const {
  if (x < 0 || x >= width())
    throw OutOfBoundsException();
  if ( y < 0 || y >= height())
    throw OutOfBoundsException();
  if ( z < 0 || z >= channels())
    throw OutOfBoundsException();

  return image_data[x*stride_[0]+y*stride_[1]+stride_[2]*z];

}

float & Image::operator()(int x) {
  if (x < 0 || x>= number_of_elements())
    throw OutOfBoundsException();
  return image_data[x];
}

float & Image::operator()(int x, int y) {
  if (x < 0 || x >= width())
    throw OutOfBoundsException();
  if ( y < 0 || y >= height())
    throw OutOfBoundsException();

  return image_data[x*stride_[0]+y*stride_[1]];
}

float & Image::operator()(int x, int y, int z) {
    if (x < 0 || x >= width())
    throw OutOfBoundsException();
  if ( y < 0 || y >= height())
    throw OutOfBoundsException();
  if ( z < 0 || z >= channels())
    throw OutOfBoundsException();

  return image_data[x*stride_[0]+y*stride_[1]+stride_[2]*z];
}


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


void compareDimensions(const Image & im1, const Image & im2)  {
  if(im1.dimensions() != im2.dimensions())
    throw MismatchedDimensionsException();
  for (int i = 0; i < im1.dimensions(); i++ ) {
    if (im1.extent(i) != im2.extent(i))
      throw MismatchedDimensionsException();
  }
}


Image operator+ (const Image & im1, const Image & im2) {
  compareDimensions(im1, im2);
  long long total_pixels = im1.number_of_elements();

  Image output(im1.extent(0), im1.extent(1), im1.extent(2));
  for (int i = 0 ; i < total_pixels; i++) {
    output(i) = im1(i) + im2(i);
  }
  return output;
}

Image operator- (const Image & im1, const Image & im2) {
  compareDimensions(im1, im2);
  long long total_pixels = im1.number_of_elements();
  Image output(im1.extent(0), im1.extent(1), im1.extent(2));
  for (int i = 0 ; i < total_pixels; i++) {
    output(i) = im1(i) - im2(i);
  }
  return output;
}

Image operator* (const Image & im1, const Image & im2) {
  compareDimensions(im1, im2);
  long long total_pixels = im1.number_of_elements();
  Image output(im1.extent(0), im1.extent(1), im1.extent(2));
  for (int i = 0 ; i < total_pixels; i++) {
    output(i) = im1(i) * im2(i);
  }
  return output;

}

Image operator/ (const Image & im1, const Image & im2) {
  compareDimensions(im1, im2);
  long long total_pixels = im1.number_of_elements();
  Image output(im1.extent(0), im1.extent(1), im1.extent(2));
  for (int i = 0 ; i < total_pixels; i++) {
    if (im2(i) == 0)
      throw DivideByZeroException();
    output(i) = im1(i) / im2(i);
  }
  return output;
}

Image operator+ (const Image & im1, const float & c) {
  long long total_pixels = im1.number_of_elements();
  Image output(im1.extent(0), im1.extent(1), im1.extent(2));
  for (int i = 0 ; i < total_pixels; i++) {
    output(i) = im1(i) + c;
  }
  return output;
}

Image operator- (const Image & im1, const float & c) {
  long long total_pixels = im1.number_of_elements();
  Image output(im1.extent(0), im1.extent(1), im1.extent(2));
  for (int i = 0 ; i < total_pixels; i++) {
    output(i) = im1(i) -  c;
  }
  return output;
}
Image operator* (const Image & im1, const float & c) {
  long long total_pixels = im1.number_of_elements();
  Image output(im1.extent(0), im1.extent(1), im1.extent(2));
  for (int i = 0 ; i < total_pixels; i++) {
    output(i) = im1(i) * c;
  }
  return output;
}
Image operator/ (const Image & im1, const float & c) {
  long long total_pixels = im1.number_of_elements();
  Image output(im1.extent(0), im1.extent(1), im1.extent(2));
  if (c==0)
    throw DivideByZeroException();
  for (int i = 0 ; i < total_pixels; i++) {
    output(i) = im1(i)/c;
  }
  return output;
}

Image operator+(const float & c, const Image & im1) {
  long long total_pixels = im1.number_of_elements();
  Image output(im1.extent(0), im1.extent(1), im1.extent(2));
  for (int i = 0 ; i < total_pixels; i++) {
    output(i) = im1(i) + c;
  }
  return output;
}

Image operator- (const float & c, const Image & im1) {
  long long total_pixels = im1.number_of_elements();
  Image output(im1.extent(0), im1.extent(1), im1.extent(2));
  for (int i = 0 ; i < total_pixels; i++) {
    output(i) = c - im1(i);
  }
  return output;
}

Image operator* (const float & c, const Image & im1) {
  long long total_pixels = im1.number_of_elements();
  Image output(im1.extent(0), im1.extent(1), im1.extent(2));
  for (int i = 0 ; i < total_pixels; i++) {
    output(i) = im1(i) * c;
  }
  return output;
}
Image operator/ (const float & c, const Image & im1) {
  long long total_pixels = im1.number_of_elements();
  Image output(im1.extent(0), im1.extent(1), im1.extent(2));
  for (int i = 0 ; i < total_pixels; i++) {
    if (im1(i) == 0)
      throw DivideByZeroException();
    output(i) = c/im1(i);
  }
  return output;
}
