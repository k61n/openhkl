//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/outlier/Distance.cpp
//! @brief     Implements class Distance
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "base/outlier/Distance.h"

#include <vector>

namespace ohkl {

Distance::Distance(unsigned int dimension) : _dimension(dimension) {};

double Distance::euclidean(Eigen::VectorXd point1, Eigen::VectorXd point2)
{
    Eigen::VectorXd differences = point1 - point2;
    return std::sqrt((differences * differences / _dimension).sum());
}

double Distance::manhattan(Eigen::VectorXd point1, Eigen::VectorXd point2)
{
    Eigen::VectorXd differences = point1 - point2;
    return (differences / _dimension).sum();
}

double Distance::cosine(Eigen::VectorXd point1, Eigen::VectorXd point2)
{
    return point1.dot(point2) / (point1.norm() * point2.norm());
}

} // namespace ohkl
