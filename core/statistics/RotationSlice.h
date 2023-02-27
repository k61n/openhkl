//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/RotationSlice.h
//! @brief     Defines class RotationSlice
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_STATISTICS_ROTATIONSLICE_H
#define OHKL_CORE_STATISTICS_ROTATIONSLICE_H

#include <vector>

namespace ohkl {

class Peak3D;

//! Utility structure to store a list of peaks with resolution in the range [dmin, dmax].
struct Slice {
    //! Minimum frame
    double fmin;
    //! Maximum frame
    double fmax;
    //! List of peaks contained within the shell
    std::vector<Peak3D*> peaks;
};

//! Splits a set of peaks into a number of slices of rotation range

class RotationSlice {
 public:
    //! Construct the given number of cells with abolute minimum dmin and absolute maximum dmax.
    RotationSlice(double fmin, double fmax, std::size_t num_slices);
    //! Add a peak to the list of slices
    //! It will automatically be added to the appropriate slice.
    void addPeak(Peak3D* peak);
    //! Returns the given shell.
    const Slice& slice(std::size_t i) const;
    //! Returns the number of slices
    std::size_t nslices() const;

 private:
    std::vector<Slice> _slices;
};

} // namespace ohkl

#endif // OHKL_CORE_STATISTICS_ROTATIONSLICE_H
