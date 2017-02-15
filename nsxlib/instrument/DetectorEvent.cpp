#include "Detector.h"
#include "DetectorEvent.h"

namespace SX {
namespace Instrument
{
//DetectorEvent::DetectorEvent():_detector(nullptr),_x(0),_y(0),_values()
//{

//}

DetectorEvent::DetectorEvent(const Detector *detector, double x, double y, std::vector<double> values):
    _detector(detector), _x(x), _y(y), _values(std::move(values))
{

}

DetectorEvent::DetectorEvent(const DetectorEvent& rhs):_detector(rhs._detector),_x(rhs._x),_y(rhs._y),_values(rhs._values)
{

}

DetectorEvent::DetectorEvent(DetectorEvent&& other):
    _detector(other._detector),
    _x(other._x),
    _y(other._y),
    _values(std::move(other._values))
{

}

DetectorEvent& DetectorEvent::operator=(const DetectorEvent& rhs)
{
    if (this!=&rhs)
    {
        _detector=rhs._detector;
        _x=rhs._x;
        _y=rhs._y;
    }
    return *this;
}

DetectorEvent::~DetectorEvent()
{
}

const Detector* DetectorEvent::getParent() const
{
    return _detector;
}

const std::vector<double> &DetectorEvent::getValues() const
{
    return _values;
}

double DetectorEvent::getX() const
{
    return _x;
}

double DetectorEvent::getY() const
{
    return _y;
}

double DetectorEvent::get2Theta(const Eigen::Vector3d& si) const
{
    Eigen::Vector3d p = getParent()->getEventPosition(*this);
    double proj = p.dot(si);
    return acos(proj/p.norm()/si.norm());
}


Eigen::Vector3d DetectorEvent::getKf(double wave,const Eigen::Vector3d& from) const
{
    // Get the event position x,y,z, taking into account the Gonio current setting
    Eigen::Vector3d p = getParent()->getEventPosition(*this);
    p-=from;
    p.normalize();
    return (p/wave);
}


} /* namespace Instrument */
} // Namespace SX
