//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/detector/DetectorEvent.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <cmath>
#include <stdexcept>

#include "core/detector/Detector.h"
#include "core/detector/DetectorFactory.h"
#include "core/gonio/Gonio.h"

namespace nsx {

DetectorEvent::DetectorEvent(double px, double py, double frame, double tof)
    : _px(px), _py(py), _frame(frame), _tof(tof)
{
}

DetectorEvent::DetectorEvent(Eigen::Vector3d x, double tof)
    : _px(x(0)), _py(x(1)), _frame(x(2)), _tof(tof)
{
}

} // end namespace nsx
