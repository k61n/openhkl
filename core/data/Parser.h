#pragma once

#include <vector>

namespace nsx {

//! Method to read a vector of int values from two char pointers, using spirit
//! This is faster than the C atoi() function.
void readIntsFromChar(const char* begin, const char* end, std::vector<int>& v);

//! Method to read a vector of double values from two char pointers, using
//! spirit
void readDoublesFromChar(const char* begin, const char* end, std::vector<double>& v);

} // end namespace nsx
