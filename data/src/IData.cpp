#include <utility>
#include <stdexcept>

#include <boost/filesystem.hpp>

#include "IData.h"
#include "Detector.h"
#include "Gonio.h"
#include "Sample.h"

namespace SX
{

namespace Data
{

IData::IData(const std::string& filename, std::shared_ptr<Diffractometer> diffractometer, bool inMemory)
: _filename(filename),
  _diffractometer(diffractometer),
  _metadata(new MetaData()),
  _inMemory(inMemory),
  _data(),
  _detectorStates(),
  _sampleStates()
{
	if ( !boost::filesystem::exists(_filename.c_str()))
		throw std::runtime_error("ILLAsciiData, file:"+_filename+" does not exist");

}

IData::~IData()
{
	delete _metadata;
}

const std::string& IData::getFilename() const
{
	return _filename;
}

std::shared_ptr<Diffractometer> IData::getDiffractometer() const
{
	return _diffractometer;
}

MetaData* const IData::getMetadata() const
{
	return _metadata;
}

bool IData::isInMemory() const
{
	return (!_inMemory);
}

ComponentState IData::getDetectorInterpolatedState(double frame)
{

	if (frame>(_detectorStates.size()-1) || frame<0)
		throw std::runtime_error("Error when interpolating detector states: invalid frame value");

	std::size_t idx=static_cast<std::size_t>(std::floor(frame));

	std::size_t nPhysicalAxes=_diffractometer->getDetector()->getGonio()->getNPhysicalAxes();

	const std::vector<double> prevState=_detectorStates[idx].getValues();
	const std::vector<double> nextState=_detectorStates[idx+1].getValues();
	std::vector<double> state(nPhysicalAxes);
	for (std::size_t i=0;i<nPhysicalAxes;++i)
		state[i] = prevState[i] + (frame-static_cast<double>(idx)*(nextState[i]-nextState[i]));

	return _diffractometer->getDetector()->createState(state);

}

ComponentState IData::getSampleInterpolatedState(double frame)
{

	if (frame>(_sampleStates.size()-1) || frame<0)
		throw std::runtime_error("Error when interpolating sample states: invalid frame value");

	std::size_t idx=static_cast<std::size_t>(std::floor(frame));

	std::size_t nPhysicalAxes=_diffractometer->getSample()->getGonio()->getNPhysicalAxes();

	const std::vector<double> prevState=_sampleStates[idx].getValues();
	const std::vector<double> nextState=_sampleStates[idx+1].getValues();
	std::vector<double> state(nPhysicalAxes);
	for (std::size_t i=0;i<nPhysicalAxes;++i)
		state[i] = prevState[i] + (frame-static_cast<double>(idx)*(nextState[i]-nextState[i]));

	return _diffractometer->getSample()->createState(state);

}

} // end namespace Data

} // end namespace SX
