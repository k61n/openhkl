//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/parser/EigenToVector.cpp
//! @brief     Defines function eigenToVector
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "base/parser/EigenToVector.h"

namespace nsx {

std::vector<double> eigenToVector(const Eigen::VectorXd& ev)
{
    const size_t size(ev.size());
    std::vector<double> v(size);
    for (size_t i = 0; i < size; ++i)
        v[i] = ev(i);
    return v;
}

} // namespace nsx
