#include "Diffractometer.h"
#include "Detector.h"
#include "Sample.h"

namespace SX
{
namespace Instrument
{

Diffractometer::Diffractometer(const std::string& name):_name(name),_detector(nullptr),_sample(nullptr)
{

}

Diffractometer::~Diffractometer()
{
	if (_detector)
		delete _detector;
	if (_sample)
		delete _sample;
}

void Diffractometer::setDetector(Detector* d)
{
	_detector=d;
}

void Diffractometer::setSample(Sample* s)
{
	_sample=s;
}

Detector* Diffractometer::getDetector()
{
	return _detector;
}

Sample* Diffractometer::getSample()
{
	return _sample;
}



}
} /* namespace SX */
