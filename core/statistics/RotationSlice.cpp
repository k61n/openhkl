//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/RotationSlice.cpp
//! @brief     Implements class RotationSlice
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/statistics/RotationSlice.h"

#include "core/peak/Peak3D.h"

namespace ohkl {

RotationSlice::RotationSlice(double fmin, double fmax, std::size_t num_slices)
    : _slices(std::max(num_slices, std::size_t(1)))
{
    std::size_t n_slices = _slices.size();
    double ds = (fmax - fmin) / static_cast<double>(num_slices);

    for (std::size_t idx = 0; idx < n_slices - 1; ++idx) {
        _slices[idx].fmin = fmin;
        _slices[idx].fmax = _slices[idx].fmin + ds * idx;
    }

    _slices[n_slices - 1].fmax = fmax;
}

void RotationSlice::addPeak(Peak3D* peak)
{
    double frame = peak->shape().center()[2];
    double fmin, fmax;
    for (std::size_t idx = 0; idx < _slices.size(); ++idx) {
        fmin = _slices[idx].fmin;
        fmax = _slices[idx].fmax;
        if (frame > fmin && frame <= fmax)
            _slices[idx].peaks.push_back(peak);
    }
}

const Slice& RotationSlice::slice(size_t i) const
{
    if (i >= _slices.size())
        throw std::runtime_error("RotationSlice::slice index out of bounds");
    return _slices[i];
}

std::size_t RotationSlice::nslices() const
{
    return _slices.size();
}

} // namespace ohkl
