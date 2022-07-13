//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/peak_find/TestDeltaConvolver.cpp
//! @brief     Test ...
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/convolve/DeltaConvolver.h"
#include "test/cpp/catch.hpp"

// Generate a image to use for testing the Fourier transform
ohkl::RealMatrix generateImage(int rows, int cols)
{
    ohkl::RealMatrix image(rows, cols);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            image(i, j) = i * i + i - 2 * j - j * j; // something NOT symmetric in i or j, and i<->j
        }
    }
    return image;
}

double computeError(int nrows, int ncols)
{
    ohkl::RealMatrix original = generateImage(nrows, ncols);
    ohkl::DeltaConvolver convolver;

    ohkl::RealMatrix transformed = convolver.convolve(original);
    ohkl::RealMatrix difference = original - transformed;
    double error = (difference * difference.transpose()).sum() / nrows / ncols;
    return std::sqrt(error);
}

TEST_CASE("test/peak_find/TestDeltaConvolver.cpp", "")
{
    double error = computeError(30, 20);
    CHECK(error < 1e-10);
}
