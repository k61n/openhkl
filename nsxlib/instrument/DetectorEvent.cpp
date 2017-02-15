#include "DetectorEvent.h"

namespace SX {
namespace Instrument
{
//DetectorEvent::DetectorEvent():_detector(nullptr),_x(0),_y(0),_values()
//{

//}

DetectorEvent::DetectorEvent(Detector *detector, int x, int y, std::vector<double> values):
    _detector(detector), _x(x), _y(y), _values(std::move(values))
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

Detector* DetectorEvent::getParent() const
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

} /* namespace Instrument */
} // Namespace SX
