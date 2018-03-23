#include <cmath>

#include <nsxlib/Convolver.h>
#include <nsxlib/DeltaKernel.h>
#include <nsxlib/MathematicsTypes.h>
#include <nsxlib/NSXTest.h>

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

double computeError(int nrows, int ncols)
{
    nsx::RealMatrix original = generateImage(nrows,ncols);
    nsx::Convolver convolver;
    nsx::DeltaKernel kernel;

    convolver.setKernel(kernel.matrix(nrows,ncols));
    nsx::RealMatrix transformed = convolver.apply(original);
    nsx::RealMatrix difference =  original-transformed;
    double error = (difference * difference.transpose()).sum() / nrows / ncols;
    return std::sqrt(error);
}

int main()
{
    double error = computeError(30, 20);
    NSX_CHECK_ASSERT(error < 1e-10);
}
