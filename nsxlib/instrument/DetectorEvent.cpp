#include "DetectorEvent.h"

namespace SX {
namespace Instrument
{
DetectorEvent::DetectorEvent():_detector(nullptr),_x(0),_y(0),_values()
{

}

DetectorEvent::DetectorEvent(const DetectorEvent& rhs):_detector(rhs._detector),_x(rhs._x),_y(rhs._y),_values(rhs._values)
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

Detector* DetectorEvent::getParent()
{
    return _detector;
}

void DetectorEvent::setParent(Detector* d)
{
    _detector=d;
}

} /* namespace Instrument */
} // Namespace SX
