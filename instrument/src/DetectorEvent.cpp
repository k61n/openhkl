#include <DetectorEvent.h>

namespace SX
{
namespace Instrument
{
DetectorEvent::DetectorEvent():_detector(nullptr),_x(0),_y(0),_values()
{

}

DetectorEvent::~DetectorEvent()
{
}

} /* namespace Instrument */
} // Namespace SX
