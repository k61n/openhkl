//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/raw/IDataReader.cpp
//! @brief     Implements class IDataReader
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/raw/IDataReader.h"

#include "base/utils/Path.h"
#include "base/utils/Units.h"
#include "core/detector/Detector.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "core/raw/DataKeys.h"

#include <cassert>
#include <stdexcept>

namespace nsx {

IDataReader::IDataReader(const std::string& filename, Diffractometer* diffractometer)
    : _diffractometer(diffractometer)
    , _nFrames(0)
    , _nRows(0)
    , _nCols(0)
    , _sampleStates()
    , _detectorStates()
    , _isOpened(false)
    , _filename(filename)
{

    _nRows = _diffractometer->detector()->nRows();
    _nCols = _diffractometer->detector()->nCols();
}

IDataReader::~IDataReader() = default;

bool IDataReader::initRead()
{
    if(!_diffractometer) {
        throw std::runtime_error("RawDataReader: No Diffractometer available.");
    }

    if(!_dataset_out) {
        throw std::runtime_error("RawDataReader: No DataSet available for output.");
    }

    // Ensure that there is at least one monochromator
    if (_dataset_out->diffractometer()->source().nMonochromators() == 0) {
        Monochromator mono(nsx::kw_monochromatorDefaultName);
        _dataset_out->diffractometer()->source().addMonochromator(mono);
        nsxlog(Level::Warning, __FUNCTION__,
               ": Source had no monochromators; hence, a default monochromator is created.");
    }

    return true;
}

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

bool IDataReader::isOpened() const
{
    return _isOpened;
}

const std::vector<std::vector<double>>& IDataReader::sampleStates() const
{
    return _sampleStates;
}

const std::vector<std::vector<double>>& IDataReader::detectorStates() const
{
    return _detectorStates;
}

void IDataReader::setDataSet(DataSet* dataset_ptr) {
    // Disallow resetting the destination DataSet
    if(!_dataset_out) {
        if (dataset_ptr)
            _dataset_out = dataset_ptr;
        else // Disallow setting the destination DataSet to a null pointer
            throw std::invalid_argument("DataReader: Cannot set the destination DataSet to null");
    }
    else
        throw std::invalid_argument("DataReader: Cannot reset the destination DataSet");

}

void IDataReader::checkInit() {
    if (!isInitialized)
        throw std::runtime_error("Reader is not initialized");
}

} // namespace nsx
