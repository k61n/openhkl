#define BOOST_TEST_MODULE "Test Convolver"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <map>
#include <string>
#include <cmath>

#include <Eigen/Dense>

#include <nsxlib/imaging/Convolver.h>
#include <nsxlib/imaging/DeltaKernel.h>

// Generate a image to use for testing the Fourier transform
nsx::Types::RealMatrix generateImage(int rows, int cols)
{
    nsx::Types::RealMatrix image(rows, cols);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            image(i, j) = i*i + i - 2*j - j*j; // something NOT symmetric in i or j, and i<->j
        }
    }
    return image;
}

double computeError(int rows, int cols)
{
    nsx::Types::RealMatrix original = generateImage(rows,cols);
    nsx::Imaging::Convolver convolver;
    nsx::Imaging::DeltaKernel kernel(rows,cols);

    convolver.setKernel(kernel.getKernel());
    nsx::Types::RealMatrix transformed = convolver.apply(original);
    nsx::Types::RealMatrix difference =  original-transformed;
    double error = (difference * difference.transpose()).sum() / rows / cols;
    return std::sqrt(error);
}

BOOST_AUTO_TEST_CASE(Test_Convolver)
{
    double error = computeError(30, 20);
    BOOST_ASSERT(error < 1e-10);
}
