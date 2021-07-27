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

#ifndef NSX_CORE_DETECTOR_DETECTOREVENT_H
#define NSX_CORE_DETECTOR_DETECTOREVENT_H

#include <Eigen/Dense>

namespace nsx {

//! Lightweight class for detector events.

class DetectorEvent {
 public:
    //! Constructor
    DetectorEvent() { }
    //! Constructor
    DetectorEvent(double px, double py, double frame, double tof = -1);

    //! Construct from a 3 vector
    explicit DetectorEvent(Eigen::Vector3d x, double tof = -1);

    //! Detector x-coord
    double px{0};
    //! Detector y-coord
    double py{0};
    //! Frame number, if applicable
    double frame{-1};
    //! Time of flight, if applicable
    double tof{-1};
};

} // namespace nsx

#endif // NSX_CORE_DETECTOR_DETECTOREVENT_H
