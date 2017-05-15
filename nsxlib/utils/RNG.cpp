/*
 * RNG.cpp
 *
 *  Created on: Mar 29, 2016
 *      Author: pellegrini
 */

#include <cstdint>
#include "RNG.h"

namespace nsx {

namespace Utils {

double xor128(void)
{
    static uint32_t x = 123456789;
    static uint32_t y = 362436069;
    static uint32_t z = 521288629;
    static uint32_t w = 88675123;
    uint32_t t;
    t = x ^ (x << 11);
    x = y; y = z; z = w;
    w = w ^ (w >> 19) ^ (t ^ (t >> 8));
    // Divide by 2^32-1 to insure that the generated random number will be in [0,1]
    return w/4294967295.0;
}

} /* namespace Utils */

} // end namespace nsx
