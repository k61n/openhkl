#include <DetectorEvent.h>

namespace SX
{
namespace Instrument
{
DetectorEvent::DetectorEvent():_detector(nullptr),_x(0),_y(0),_values()
{

}

DetectorEvent::DetectorEvent(const DetectorEvent& rhs):_detector(rhs._detector),_x(rhs._x),_y(rhs._y)
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

} /* namespace Instrument */
} // Namespace SX
