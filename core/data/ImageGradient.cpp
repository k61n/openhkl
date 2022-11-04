//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/data/ImageGradient.cpp
//! @brief     Implements class ImageGradient
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/data/ImageGradient.h"

#include "base/utils/Logger.h"
#include "core/convolve/ConvolverFactory.h"
#include <stdexcept>
#include <memory>

namespace ohkl {

ImageGradient::ImageGradient(const Eigen::MatrixXd& image, bool realspace)
    : _image(&image)
    , _real_space(realspace)
{
    _dx = Eigen::MatrixXd::Zero(image.rows(), image.cols());
    _dy = Eigen::MatrixXd::Zero(image.rows(), image.cols());
}

double ImageGradient::pixel(int row, int col)
{
    if (row < 0)
        row = _image->rows() + row + 1;
    else if (row >= _image->rows())
        row = row % _image->rows();
    if (col < 0)
        col = _image->cols() + col + 1;
    else if (col >= _image->cols())
        col = col % _image->cols();
    return (*_image)(row, col);
}

void ImageGradient::compute(GradientKernel kernel)
{
    if (_real_space) {
        ohklLog(
            Level::Debug, "ImageGradient::compute: real space ",
            _convolver_callbacks.at(kernel), " kernel");
        computeRealSpace(kernel);
    } else {
            ohklLog(
            Level::Debug, "ImageGradient::compute: FFT ",
            _convolver_callbacks.at(kernel), " kernel");
        computeFFT(kernel);
    }
}

void ImageGradient::computeRealSpace(GradientKernel kernel)
{
    switch(kernel) {
    case GradientKernel::CentralDifference:
        gradient([this](int row, int col){ centralDifference(row, col); });
        break;
    case GradientKernel::Sobel:
        gradient([this](int row, int col) { sobel(row, col); });
        break;
    case GradientKernel::Prewitt:
        gradient([this](int row, int col) { prewitt(row, col); });
        break;
    case GradientKernel::Roberts:
        gradient([this](int row, int col) { roberts(row, col); });
        break;
    default:
        throw std::runtime_error(
            "ImageGradient::compute: Gradient kernel not implemented for real space operation");
    }
}

void ImageGradient::computeFFT(GradientKernel kernel)
{
    _convolver.reset(ConvolverFactory{}.create(_convolver_callbacks.at(kernel), {{"x", 0.0}}));
    _dx = _convolver->convolve(*_image);
    _convolver.reset(ConvolverFactory{}.create(_convolver_callbacks.at(kernel), {{"y", 0.0}}));
    _dy = _convolver->convolve(*_image);
}

void ImageGradient::gradient(std::function<void (int, int)> kernel_operator)
{
    for (int col = 0; col < _image->cols(); ++col) {
        for (int row = 0; row < _image->rows(); ++row) {
            kernel_operator(row, col);
        }
    }
}

void ImageGradient::centralDifference(int row, int col)
{
    _dx(row, col) = (- pixel(row, col - 1) + pixel(row, col + 1)) / 2.0;
    _dy(row, col) = (- pixel(row - 1, col) + pixel(row + 1, col)) / 2.0;
}

void ImageGradient::sobel(int row, int col)
{
    _dx(row, col) =
        (- pixel(row - 1, col - 1) - 2 * pixel(row, col - 1) - pixel(row + 1, col - 1)
         + pixel(row - 1, col + 1) + 2 * pixel(row, col + 1) + pixel(row + 1, col + 1)) / 8.0;

    _dy(row, col) =
        (- pixel(row - 1, col - 1) - 2 * pixel(row - 1, col) - pixel(row - 1, col + 1)
         + pixel(row + 1, col - 1) + 2 * pixel(row + 1, col) + pixel(row + 1, col + 1)) / 8.0;
}

void ImageGradient::prewitt(int row, int col)
{
    _dx(row, col) =
        (- pixel(row - 1, col - 1) - pixel(row, col - 1) - pixel(row + 1, col - 1)
         + pixel(row - 1, col + 1) + pixel(row, col + 1) + pixel(row + 1, col + 1)) / 6.0;

    _dy(row, col) =
        (- pixel(row - 1, col - 1) - pixel(row - 1, col) - pixel(row - 1, col + 1)
         + pixel(row + 1, col - 1) + pixel(row + 1, col) + pixel(row + 1, col + 1)) / 6.0;
}

void ImageGradient::roberts(int row, int col)
{
    _dx(row, col) = (pixel(row, col) - pixel(row + 1, col + 1)) / 2.0;
    _dy(row, col) = (pixel(row + 1, col) - pixel(row, col + 1)) / 2.0;
}

Eigen::MatrixXd ImageGradient::magnitude() const
{
    Eigen::MatrixXd mag = Eigen::MatrixXd::Zero(_image->rows(), _image->cols());
    for (int col = 0; col < mag.cols(); ++col) {
        for (int row = 0; row < mag.rows(); ++row) {
            mag(row, col) = std::sqrt(_dx(row, col) * _dx(row, col) + _dy(row, col) * _dy(row, col));
        }
    }
    // return _dx.cwiseAbs(); // How to check x component is computed correctly
    return mag;
}

} // namespace ohkl
