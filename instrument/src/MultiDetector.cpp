#include "MultiDetector.h"

namespace SX
{

namespace Instrument
{

//Detector* MultiDetector::create(const std::string& name)
//{
//	return new MultiDetector(name);
//}

Eigen::Vector3d MultiDetector::getPos(double px, double py) const
{

	for (auto& detector : _components)
	{
		if (detector->hasPixel(px,py))
			return detector->getPos(px,py);
	}
	throw std::runtime_error("Detector: invalid pixel");
}


} // end namespace Instrument

} // end namespace SX
