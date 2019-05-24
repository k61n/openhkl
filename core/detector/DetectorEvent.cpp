#include <cmath>
#include <stdexcept>

#include "Detector.h"
#include "DetectorFactory.h"
#include "Gonio.h"

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
