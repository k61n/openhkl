//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/geometry/DirectVector.cpp
//! @brief     Implements class DirectVector
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <stdexcept>

#include <Eigen/Dense>

#include "core/geometry/DirectVector.h"

namespace nsx {

DirectVector::DirectVector(double x, double y, double z) : _dvector(x, y, z) {}

DirectVector::DirectVector(const Eigen::Vector3d& dvector) : _dvector(dvector) {}

const Eigen::Vector3d& DirectVector::vector() const
{
    return _dvector;
}

double DirectVector::operator[](int index) const
{
    if (index < 0 || index > 2)
        throw std::runtime_error("Invalid index for a 3D vector");
    return _dvector[index];
}

const double& DirectVector::operator[](int index)
{
    if (index < 0 || index > 2)
        throw std::runtime_error("Invalid index for a 3D vector");
    return _dvector[index];
}

void DirectVector::print(std::ostream& os) const
{
    os << _dvector;
}

std::ostream& operator<<(std::ostream& os, const DirectVector& dvector)
{
    dvector.print(os);
    return os;
}

} // namespace nsx
