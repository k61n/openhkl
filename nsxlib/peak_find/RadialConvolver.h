// Convolver description: the kernel is non-zero in a circular and annular region, as follows

// [0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0]
// [0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0]
// [0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0]
// [0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 0, 0, 0]
// [0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0]
// [0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0]
// [0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0]
// [0, 0, 0, 2, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 0]
// [0, 0, 0, 2, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 2, 2, 2, 0]
// [0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2]
// [0, 0, 0, 2, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 2, 2, 2, 0]
// [0, 0, 0, 2, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 0]
// [0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0]
// [0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0]
// [0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0]
// [0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 0, 0, 0]
// [0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0]
// [0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0]
// [0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0]

// In region 1, pixels are assigned constant positive value
// In region 2, pixels are assigned a constant negative value.
// The values are chosen so that the integral over region 1 is equal to +1,
// and the integral over region 2 is equal to -1
//
// The motivation is as follows: if a peak is contained inside region 1, then
// the convolution with region 1 computes the average intensity of the peak,
// and the convolution with region 2 computes (minus) the average background
// The the total convolution computes the average intensity minus the average background.
// This make the kernel effective for peak-finding in images which have a non-homogeneous
// background.

#pragma once

#include "AtomicConvolver.h"

namespace nsx {

class RadialConvolver : public AtomicConvolver {

public:

    RadialConvolver();

    RadialConvolver(const RadialConvolver& other)=default;

    RadialConvolver(const std::map<std::string,double>& parameters);

    ~RadialConvolver()=default;

    RadialConvolver& operator=(const RadialConvolver &other)=default;

    Convolver* clone() const override;

    virtual std::pair<size_t,size_t> kernelSize() const override;

private:

    RealMatrix _matrix(int nrows, int ncols) const override;
};

} // end namespace nsx
