//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/AnnularConvolver.h
//! @brief     Defines class AnnularConvolver
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_CONVOLVE_ANNULARCONVOLVER_H
#define OHKL_CORE_CONVOLVE_ANNULARCONVOLVER_H

#include "core/convolve/Convolver.h" // inherits from

namespace ohkl {

//! Annular convolution kernel used for local background subtraction.

//! The kernel is non-zero in a circular and annular region, as follows
//!
//! [0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0]\n
//! [0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0]\n
//! [0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0]\n
//! [0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 0, 0, 0]\n
//! [0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0]\n
//! [0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0]\n
//! [0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0]\n
//! [0, 0, 0, 2, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 0]\n
//! [0, 0, 0, 2, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 2, 2, 2, 0]\n
//! [0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2]\n
//! [0, 0, 0, 2, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 2, 2, 2, 0]\n
//! [0, 0, 0, 2, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 0]\n
//! [0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0]\n
//! [0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0]\n
//! [0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0]\n
//! [0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 0, 0, 0]\n
//! [0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0]\n
//! [0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0]\n
//! [0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0]
//!
//! In region 1, pixels are assigned constant positive value
//! In region 2, pixels are assigned a constant negative value.
//! The values are chosen so that the integral over region 1 is equal to +1,
//! and the integral over region 2 is equal to -1
//!
//! The motivation is as follows: if a peak is contained inside region 1, then
//! the convolution with region 1 computes the average intensity of the peak,
//! and the convolution with region 2 computes (minus) the average background
//! The the total convolution computes the average intensity minus the average
//! background. This make the kernel effective for peak-finding in images which
//! have a non-homogeneous background.

class AnnularConvolver : public Convolver {
 public:
    AnnularConvolver();

    AnnularConvolver(const AnnularConvolver& other) = default;

    AnnularConvolver(const std::map<std::string, double>& parameters);

    ~AnnularConvolver() = default;

    AnnularConvolver& operator=(const AnnularConvolver& other) = default;

    Convolver* clone() const override;

    virtual std::pair<size_t, size_t> kernelSize() const override;

    RealMatrix convolve(const RealMatrix& image) override;
};

} // namespace ohkl

#endif // OHKL_CORE_CONVOLVE_ANNULARCONVOLVER_H
