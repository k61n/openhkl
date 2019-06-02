#include "test/cpp/catch.hpp"
#include "core/search_peaks/DeltaConvolver.h"
#include "core/mathematics/MathematicsTypes.h"

// Generate a image to use for testing the Fourier transform
nsx::RealMatrix generateImage(int rows, int cols)
{
    nsx::RealMatrix image(rows, cols);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            image(i, j) = i * i + i - 2 * j - j * j; // something NOT symmetric in i or j, and i<->j
        }
    }
    return image;
}

double computeError(int nrows, int ncols)
{
    nsx::RealMatrix original = generateImage(nrows, ncols);
    nsx::DeltaConvolver convolver;

    nsx::RealMatrix transformed = convolver.convolve(original);
    nsx::RealMatrix difference = original - transformed;
    double error = (difference * difference.transpose()).sum() / nrows / ncols;
    return std::sqrt(error);
}

TEST_CASE("test/peak_find/TestDeltaConvolver.cpp", "") {

    double error = computeError(30, 20);
    CHECK(error < 1e-10);
}
