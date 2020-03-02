//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/detector/DetectorEvent.h
//! @brief     Defines class DetectorEvent
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_DETECTOR_DETECTOREVENT_H
#define CORE_DETECTOR_DETECTOREVENT_H

#include <Eigen/Dense>

namespace nsx {

//! Lightweight class for detector events.

class DetectorEvent {
 public:
    //! Constructor
    DetectorEvent() {}
    //! Constructor
    DetectorEvent(double px, double py, double frame, double tof = -1);

    //! Construct from a 3 vector
    explicit DetectorEvent(Eigen::Vector3d x, double tof = -1);

    //! Detector x-coord
    double _px{0};
    //! Detector y-coord
    double _py{0};
    //! Frame number, if applicable
    double _frame{-1};
    //! Time of flight, if applicable
    double _tof{-1};
};

} // namespace nsx

#endif // CORE_DETECTOR_DETECTOREVENT_H
