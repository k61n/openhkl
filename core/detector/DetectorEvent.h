//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/detector/DetectorEvent.h
//! @brief     Defines class DetectorEvent
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_DETECTOR_DETECTOREVENT_H
#define OHKL_CORE_DETECTOR_DETECTOREVENT_H

#include <Eigen/Dense>

namespace ohkl {

//! Lightweight class for detector events.

class DetectorEvent {
 public:
    //! Constructor
    DetectorEvent() { }
    //! Constructor
    DetectorEvent(double px, double py, double frame, double tof = -1);

    //! Construct from a 3 vector
    explicit DetectorEvent(Eigen::Vector3d x, double tof = -1);

    //! Return the real space vector
    Eigen::Vector3d vector() const;

    bool isValid() const;

    //! Detector x-coord
    double px{0};
    //! Detector y-coord
    double py{0};
    //! Frame number, if applicable
    double frame{-1};
    //! Time of flight, if applicable
    double tof{-1};

 private:
    bool _valid;
};

} // namespace ohkl

#endif // OHKL_CORE_DETECTOR_DETECTOREVENT_H
