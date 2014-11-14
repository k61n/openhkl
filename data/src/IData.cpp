#include <utility>
#include <stdexcept>

#include <boost/filesystem.hpp>
#include<boost/filesystem/operations.hpp>

#include "IData.h"
#include "Detector.h"
#include "Gonio.h"
#include "Sample.h"

namespace SX
{

namespace Data
{

using namespace boost::filesystem;

IData::IData(const std::string& filename, std::shared_ptr<Diffractometer> diffractometer, bool inMemory)
: _filename(filename),
  _nFrames(0),
  _diffractometer(diffractometer),
  _metadata(new MetaData()),
  _inMemory(inMemory),
  _data(),
  _detectorStates(),
  _sampleStates(),
  _peaks(),
  _isMapped(false)
{
	if ( !boost::filesystem::exists(_filename.c_str()))
		throw std::runtime_error("ILLAsciiData, file: "+_filename+" does not exist");

}

IData::~IData()
{
	clearPeaks();
	delete _metadata;
}

std::string IData::getBasename() const
{
	path pathname(_filename);
	return pathname.filename().string();
}

const std::vector<vint>& IData::getData() const
{
	return _data;
}

vint& IData::getData(std::size_t idx)
{
	return _data[idx];
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

std::size_t IData::getNFrames() const
{
	return _nFrames;
}

std::set<Peak3D*>& IData::getPeaks()
{
	return _peaks;
}

void IData::addPeak(Peak3D* peak)
{
	_peaks.insert(peak);
}

void IData::clearPeaks()
{
	for (auto ptr : _peaks)
		delete ptr;
	_peaks.clear();
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
		state[i] = prevState[i] + (frame-static_cast<double>(idx))*(nextState[i]-prevState[i]);

	return _diffractometer->getDetector()->createState(state);

}

const ComponentState& IData::getDetectorState(int frame) const
{
	if (frame>(_detectorStates.size()-1) || frame<0)
			throw std::runtime_error("Error when returning detector state: invalid frame value");
	return _detectorStates[frame];
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
		state[i] = prevState[i] + (frame-static_cast<double>(idx))*(nextState[i]-prevState[i]);

	return _diffractometer->getSample()->createState(state);

}

const ComponentState& IData::getSampleState(int frame) const
{
	if (frame>(_sampleStates.size()-1) || frame<0)
			throw std::runtime_error("Error when returning sample state: invalid frame value");
	return _sampleStates[frame];
}

const std::vector<ComponentState>& IData::getDetectorStates() const
{
	return _detectorStates;
}

const std::vector<ComponentState>& IData::getSampleStates() const
{
	return _sampleStates;
}

bool IData::removePeak(Peak3D* peak)
{
	auto it=_peaks.find(peak);
	if (it==_peaks.end())
		return false;

	delete *it;
    _peaks.erase(it);
	return true;
}

bool IData::isMapped() const
{
	return _isMapped;
}

} // end namespace Data

} // end namespace SX
