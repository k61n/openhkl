//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/data/EigenToVector.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/data/EigenToVector.h"

namespace nsx {

std::vector<double> eigenToVector(const Eigen::VectorXd& ev)
{
    const size_t size(ev.size());
    std::vector<double> v(size);
    for (size_t i = 0; i < size; ++i) {
        v[i] = ev(i);
    }
    return v;
}

} // end namespace nsx
