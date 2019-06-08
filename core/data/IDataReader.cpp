//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/data/IDataReader.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <cassert>

#include "blosc.h"

#include "H5Cpp.h"

#include "core/data/BloscFilter.h"
#include "core/detector/Detector.h"
#include "core/instrument/Diffractometer.h"
#include "core/axes/Gonio.h"
#include "core/data/IDataReader.h"
#include "core/mathematics/MathematicsTypes.h"
#include "core/utils/Path.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "core/utils/Units.h"

namespace nsx {

IDataReader::IDataReader(const std::string& filename, Diffractometer* diffractometer)
    : _diffractometer(std::move(diffractometer))
    , _nFrames(0)
    , _sampleStates()
    , _detectorStates()
    , _fileSize(0)
    , _isOpened(false)
{
    _metadata.add<std::string>("filename", filename);

    _nRows = _diffractometer->detector()->nRows();
    _nCols = _diffractometer->detector()->nCols();
}

IDataReader::~IDataReader() {}

size_t IDataReader::nFrames() const
{
    return _nFrames;
}

size_t IDataReader::nCols() const
{
    return _nCols;
}

size_t IDataReader::nRows() const
{
    return _nRows;
}

const MetaData& IDataReader::metadata() const
{
    return _metadata;
}

MetaData& IDataReader::metadata()
{
    return _metadata;
}

const Diffractometer* IDataReader::diffractometer() const
{
    return _diffractometer;
}

Diffractometer* IDataReader::diffractometer()
{
    return _diffractometer;
}

InstrumentState IDataReader::state(size_t frame) const
{
    assert(frame < _nFrames);

    InstrumentState state(_diffractometer);

    // compute transformations
    const auto& detector_gonio = _diffractometer->detector()->gonio();
    const auto& sample_gonio = _diffractometer->sample().gonio();

    Eigen::Transform<double, 3, Eigen::Affine> detector_trans =
        detector_gonio.affineMatrix(_detectorStates[frame]);
    Eigen::Transform<double, 3, Eigen::Affine> sample_trans =
        sample_gonio.affineMatrix(_sampleStates[frame]);

    state.detectorOrientation = detector_trans.rotation();
    state.sampleOrientation = Eigen::Quaterniond(sample_trans.rotation());

    state.detectorPositionOffset = detector_trans.translation();
    state.samplePosition = sample_trans.translation();

    state.ni = _diffractometer->source().selectedMonochromator().ki().rowVector();
    state.ni.normalize();
    state.wavelength = _diffractometer->source().selectedMonochromator().wavelength();

    return state;
}

std::string IDataReader::basename() const
{
    return fileBasename(_metadata.key<std::string>("filename"));
}

std::string IDataReader::filename() const
{
    return _metadata.key<std::string>("filename");
}

bool IDataReader::isOpened() const
{
    return _isOpened;
}

std::size_t IDataReader::fileSize() const
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
