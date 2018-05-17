/*
 * IDataReader.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: pellegrini
 */

#include <cassert>

#include "blosc.h"

#include "H5Cpp.h"

#include "BloscFilter.h"
#include "Detector.h"
#include "Diffractometer.h"
#include "Gonio.h"
#include "IDataReader.h"
#include "MathematicsTypes.h"
#include "Path.h"
#include "Sample.h"
#include "Source.h"
#include "Units.h"

namespace nsx {

IDataReader::IDataReader(const std::string& filename, const sptrDiffractometer& diffractometer)
: _diffractometer(std::move(diffractometer)),
  _nFrames(0),
  _sampleStates(),
  _detectorStates(),
  _fileSize(0),
  _isOpened(false)
{
    _metadata.add<std::string>("filename",filename);

    _nRows = _diffractometer->getDetector()->getNRows();
    _nCols = _diffractometer->getDetector()->getNCols();
}

size_t IDataReader::getNFrames() const {
    return _nFrames;
}

size_t IDataReader::getNCols() const {
    return _nCols;
}

size_t IDataReader::getNRows() const {
    return _nRows;
}

const MetaData& IDataReader::getMetadata() const {
    return _metadata;
}

const Diffractometer& IDataReader::getDiffractometer() const {
    return *_diffractometer;
}

InstrumentState IDataReader::getState(size_t frame) const
{
    assert(frame < _nFrames);

    InstrumentState state(_diffractometer);

    // compute transformations
    auto detector_gonio = _diffractometer->getDetector()->getGonio();
    auto sample_gonio = _diffractometer->getSample()->getGonio();

    Eigen::Transform<double,3,Eigen::Affine> detector_trans = detector_gonio->getHomMatrix(_detectorStates[frame]);
    Eigen::Transform<double,3,Eigen::Affine> sample_trans = sample_gonio->getHomMatrix(_sampleStates[frame]);

    state.detectorOrientation = detector_trans.rotation();
    state.sampleOrientation = Eigen::Quaterniond(sample_trans.rotation());

    state.detectorOffset = detector_trans.translation();
    state.samplePosition = sample_trans.translation();

    state.ni = _diffractometer->getSource()->getSelectedMonochromator().getKi().rowVector();
    state.ni.normalize();
    state.wavelength = _diffractometer->getSource()->getSelectedMonochromator().getWavelength();

    return state;
}

std::string IDataReader::getBasename() const
{
    return fileBasename(_metadata.getKey<std::string>("filename"));
}

std::string IDataReader::getFilename() const
{
    return _metadata.getKey<std::string>("filename");
}


bool IDataReader::isOpened() const
{
    return _isOpened;
}


std::size_t IDataReader::getFileSize() const
{
    return _fileSize;
}

const std::vector<std::vector<double>>& IDataReader::sampleStates() const
{
    return _sampleStates;
}

const std::vector<std::vector<double>>& IDataReader::detectorStates() const
{
    return _detectorStates;
}

} // end namespace nsx
