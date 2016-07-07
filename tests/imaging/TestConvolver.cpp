#define BOOST_TEST_MODULE "Test Convolver"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <map>
#include <string>
#include <cmath>

#include <Eigen/Dense>

#include "Convolver.h"
#include "DeltaKernel.h"

// Generate a image to use for testing the Fourier transform
SX::Types::RealMatrix generateImage(int rows, int cols)
{
    SX::Types::RealMatrix image(rows, cols);

    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            image(i, j) = i*i + i - 2*j - j*j; // something NOT symmetric in i or j, and i<->j

    return image;
}

double computeError(int rows, int cols)
{
    SX::Imaging::Convolver convolver;
    SX::Imaging::DeltaKernel kernel;

    SX::Types::RealMatrix original = generateImage(rows,cols);

    kernel.getParameters()["rows"] = rows;
    kernel.getParameters()["cols"] = cols;

    convolver.setKernel(kernel.getKernel());

    SX::Types::RealMatrix transformed = convolver.apply(original);

    SX::Types::RealMatrix difference =  original-transformed;

    double error = (difference * difference.transpose()).sum() / rows / cols;

    return std::sqrt(error);
}



BOOST_AUTO_TEST_CASE(Test_Convolver)
{
    double error = computeError(30, 20);
    BOOST_ASSERT(error < 1e-10);
}
