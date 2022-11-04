//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/data/ImageGradient.h
//! @brief     Defines class ImageGradient
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_DATA_IMAGEGRADIENT_H
#define OHKL_CORE_DATA_IMAGEGRADIENT_H

#include "core/convolve/Convolver.h"

#include <functional>
#include <map>
#include <memory>
#include <Eigen/Dense>

namespace ohkl {

enum class GradientKernel {
    CentralDifference,
    Sobel,
    Sobel5,
    Prewitt,
    Roberts
};

/*! \addtogroup python_api
 *  @{*/

/*! \brief Compute image gradients
 *
 *  Compute image gradients via real space operations or convolution. Real space method is
 *  provided mainly as a sanity check for FFT method.
 */
class ImageGradient {
 public:
    ImageGradient(const Eigen::MatrixXd& image, bool realspace = true);

    //! Get an element, respecting periodic boundary condition
    double pixel(int row, int col);

    //! Compute the gradient (but not the magnitude)
    void compute(GradientKernel kernel);
    //! Compute gradient using real space method
    void computeRealSpace(GradientKernel kernel);
    //! Compute gradient using FFT method
    void computeFFT(GradientKernel kernel);
    //! Loop over the image to compute the gradient the gradient
    void gradient(std::function<void (int, int)> kernel_operator);
    //! Compute gradient of a pixel with central differences
    void centralDifference(int row, int col);
    //! Compute gradient of a pixel with Sobel operator
    void sobel(int row, int col);
    //! Compute gradient of a pixel with Prewitt operator
    void prewitt(int row, int col);
    //! Compute gradient of a pixel with Roberts operator
    void roberts(int row, int col);
    //! Return the magnitude of the gradient
    Eigen::MatrixXd magnitude() const;

 private:
    const Eigen::MatrixXd* _image;

    //! Gradient in the x direction
    Eigen::MatrixXd _dx;
    //! Gradient in the y direction
    Eigen::MatrixXd _dy;
    //! Do calculations in real space (as opposed to FFT)
    bool _real_space;
    //! Convolver for image filtering
    std::unique_ptr<Convolver> _convolver;

    const std::map<GradientKernel, std::string> _convolver_callbacks = {
        {GradientKernel::CentralDifference, "central"},
        {GradientKernel::Sobel, "sobel"},
        {GradientKernel::Sobel5, "sobel5"},
        {GradientKernel::Prewitt, "prewitt"},
        {GradientKernel::Roberts, "roberts"}
    };
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_DATA_IMAGEGRADIENT_H
