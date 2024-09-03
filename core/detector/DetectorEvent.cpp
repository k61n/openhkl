//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/detector/DetectorEvent.cpp
//! @brief     Implements class DetectorEvent
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <cmath>
#include <stdexcept>

#include "core/detector/DetectorEvent.h"

namespace ohkl {

DetectorEvent::DetectorEvent(double px, double py, double frame, double tof)
    : px(px), py(py), frame(frame), tof(tof)
{
}

DetectorEvent::DetectorEvent(Eigen::Vector3d x, double tof)
    : px(x(0)), py(x(1)), frame(x(2)), tof(tof)
{
}

Eigen::Vector3d DetectorEvent::vector() const
{
    return {px, py, frame};
}

bool DetectorEvent::isValid() const
{
    return _valid;
}

} // namespace ohkl
