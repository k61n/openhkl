//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/CentralDifference.h
//! @brief     Defines class CentralDifference
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_CONVOLVE_CENTRALDIFFERENCE_H
#define OHKL_CORE_CONVOLVE_CENTRALDIFFERENCE_H

#include "core/convolve/AtomicConvolver.h" // inherits from

namespace ohkl {

//! Convolver with central difference gradient kernel
//!
//! dx = [  0  0  0 ] dy = [  0  1  0 ]
//!      [  1  0 -1 ]      [  0  0  0 ]
//!      [  0  0  0 ]      [  0 -1  0 ]
//!
//! Computes a central difference gradient

class CentralDifference : public AtomicConvolver {
 public:
    CentralDifference();

    CentralDifference(const CentralDifference& other) = default;

    CentralDifference(const std::map<std::string, double>& parameters);

    ~CentralDifference() = default;

    CentralDifference& operator=(const CentralDifference& other) = default;

    Convolver* clone() const override;

    virtual std::pair<size_t, size_t> kernelSize() const override;

 private:
    RealMatrix _matrix(int nrows, int ncols) const override;
};

} // namespace ohkl

#endif // OHKL_CORE_CONVOLVE_CENTRALDIFFERENCE_H
