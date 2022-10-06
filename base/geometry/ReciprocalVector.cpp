//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/geometry/ReciprocalVector.cpp
//! @brief     Implements class ReciprocalVector
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <Eigen/Dense>

#include "base/geometry/ReciprocalVector.h"

namespace ohkl {

ReciprocalVector::ReciprocalVector() : _valid(false) { }

ReciprocalVector::ReciprocalVector(double x, double y, double z) : _rvector(x, y, z), _valid(true)
{
}

ReciprocalVector::ReciprocalVector(const Eigen::RowVector3d& rvector)
    : _rvector(rvector), _valid(true)
{
}

const Eigen::RowVector3d& ReciprocalVector::rowVector() const
{
    return _rvector;
}

double ReciprocalVector::operator[](int index) const
{
    if (index < 0 || index > 2)
        throw std::runtime_error("Invalid index for a 3D vector");
    return _rvector[index];
}

const double& ReciprocalVector::operator[](int index)
{
    if (index < 0 || index > 2)
        throw std::runtime_error("Invalid index for a 3D vector");
    return _rvector[index];
}

void ReciprocalVector::print(std::ostream& os) const
{
    os << _rvector;
}

std::ostream& operator<<(std::ostream& os, const ReciprocalVector& rvector)
{
    rvector.print(os);
    return os;
}

bool ReciprocalVector::isValid() const
{
    return _valid;
}

} // namespace ohkl
