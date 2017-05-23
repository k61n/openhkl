#define BOOST_TEST_MODULE "Test Convolver"
#define BOOST_TEST_DYN_LINK

#include <cmath>

#include <boost/test/unit_test.hpp>

#include <nsxlib/imaging/Convolver.h>
#include <nsxlib/imaging/DeltaKernel.h>
#include <nsxlib/mathematics/MathematicsTypes.h>

// Generate a image to use for testing the Fourier transform
nsx::RealMatrix generateImage(int rows, int cols)
{
    nsx::RealMatrix image(rows, cols);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            image(i, j) = i*i + i - 2*j - j*j; // something NOT symmetric in i or j, and i<->j
        }
    }
    return image;
}

double computeError(int rows, int cols)
{
    nsx::RealMatrix original = generateImage(rows,cols);
    nsx::Convolver convolver;
    nsx::DeltaKernel kernel(rows,cols);

    convolver.setKernel(kernel.getKernel());
    nsx::RealMatrix transformed = convolver.apply(original);
    nsx::RealMatrix difference =  original-transformed;
    double error = (difference * difference.transpose()).sum() / rows / cols;
    return std::sqrt(error);
}

BOOST_AUTO_TEST_CASE(Test_Convolver)
{
    double error = computeError(30, 20);
    BOOST_ASSERT(error < 1e-10);
}
