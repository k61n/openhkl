#ifndef CORE_DETECTOR_DETECTOREVENT_H
#define CORE_DETECTOR_DETECTOREVENT_H

#include <string>
#include <vector>

#include <yaml-cpp/yaml.h>

#include <Eigen/Dense>

#include "Component.h"
#include "DirectVector.h"
#include "Enums.h"
#include "GeometryTypes.h"

namespace nsx {

//! Lightweight class for detector events.
class DetectorEvent {
public:
    //! Constructor
    DetectorEvent(double px = 0, double py = 0, double frame = -1, double tof = -1);

    //! Construct from a 3 vector
    explicit DetectorEvent(Eigen::Vector3d x, double tof = -1);

    //! Detector x-coord
    double _px;
    //! Detector y-coord
    double _py;
    //! Frame number, if applicable
    double _frame;
    //! Time of flight, if applicable
    double _tof;
};

} // end namespace nsx

#endif // CORE_DETECTOR_DETECTOREVENT_H
