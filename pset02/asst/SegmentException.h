#ifndef __SEGMENTEXCEPTION__H
#define __SEGMENTEXCEPTION__H

#include <stdexcept>
#include <stdio.h>

class MismatchedSizeException : public std::runtime_error {
 public:
 MismatchedSizeException() : std::runtime_error("Vectors must have the same size") {}

};

#endif
