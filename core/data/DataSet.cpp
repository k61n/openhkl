//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/data/DataSet.cpp
//! @brief     Implements class DataSet
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/data/DataSet.h"
#include "base/parser/BloscFilter.h"
#include "base/utils/Logger.h"
#include "base/utils/Path.h"
#include "base/utils/ProgressHandler.h"
#include "base/utils/Units.h" // deg
#include "core/detector/Detector.h"
#include "core/detector/DetectorEvent.h"
#include "core/experiment/ExperimentExporter.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Monochromator.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "core/raw/DataKeys.h"

#include <H5Cpp.h>

namespace nsx {

DataSet::DataSet(std::shared_ptr<IDataReader> reader)
    : _nFrames{0}
    , _nrows{0}
    , _ncols{0}
    , _reader{std::move(reader)}
{
    _nrows = detector().nRows();
    _ncols = detector().nCols();
    _nFrames = _reader->metadata().key<int>(nsx::at_frameCount);

    _metadata.setMap(_reader->metadata().map());

    double wav = _metadata.key<double>(nsx::at_wavelength);
    _reader->diffractometer()->source().selectedMonochromator().setWavelength(wav);

    // Getting Scan parameters for the detector
    _states.reserve(_nFrames);

    for (unsigned int i = 0; i < _nFrames; ++i)
        _states.push_back(_reader->state(i));
}

DataSet::~DataSet() { }

int DataSet::dataAt(const std::size_t x, const std::size_t y, const std::size_t z) const
{
    // Check that the voxel is inside the limit of the data
    if (z >= _nFrames || y >= _ncols || x >= _nrows) {
        throw std::runtime_error
        ("DataSet '" + _name + "': "
         + "Out-of-bound access (x = " + std::to_string(x) + ", "
         + "y = " + std::to_string(y) + ", z = " + std::to_string(z)
         + ")");
    }

    return frame(z)(x, y);
}

Eigen::MatrixXi DataSet::frame(const std::size_t idx) const
{
    return _reader->data(idx);
}

void DataSet::open()
{
    _reader->open();
}

void DataSet::close()
{
    _reader->close();
}

std::size_t DataSet::nFrames() const
{
    return _nFrames;
}

std::size_t DataSet::nCols() const
{
    return _ncols;
}

std::size_t DataSet::nRows() const
{
    return _nrows;
}

const InstrumentStateList& DataSet::instrumentStates() const
{
    return _states;
}

InstrumentStateList& DataSet::instrumentStates()
{
    return _states;
}

void DataSet::addMask(IMask* mask)
{
    _masks.insert(mask);
}

void DataSet::removeMask(IMask* mask)
{
    if (_masks.find(mask) != _masks.end())
        _masks.erase(mask);
}

const std::set<IMask*>& DataSet::masks() const
{
    return _masks;
}

void DataSet::maskPeaks(std::vector<Peak3D*>& peaks) const
{
    for (const auto& peak : peaks) {
        // peak belongs to another dataset
        if (peak->dataSet().get() != this)
            continue;

        peak->setMasked(false);
        for (const auto& m : _masks) {
            // If the background of the peak intercept the mask, unselected the peak
            if (m->collide(peak->shape())) {
                peak->setMasked(true);
                peak->setRejectionFlag(RejectionFlag::Masked);
                break;
            }
        }
    }
}

ReciprocalVector DataSet::computeQ(const DetectorEvent& ev) const
{
    const auto& state = _states.interpolate(ev._frame);
    const auto& detector_position = DirectVector(detector().pixelPosition(ev._px, ev._py));
    return state.sampleQ(detector_position);
}

Eigen::MatrixXd DataSet::transformedFrame(std::size_t idx) const
{
    Eigen::ArrayXXd new_frame = frame(idx).cast<double>();
    new_frame -= detector().baseline();
    new_frame /= detector().gain();
    return new_frame;
}

const IDataReader* DataSet::reader() const
{
    return _reader.get();
}

IDataReader* DataSet::reader()
{
    return _reader.get();
}

const Detector& DataSet::detector() const
{
    return *_reader->diffractometer()->detector();
}

void DataSet::setName(const std::string name)
{
    if (name.empty())
        return;

    const std::string invalid_chars{"\\/"};
    const std::size_t sep = name.find_first_of(invalid_chars);
    if (sep != std::string::npos)
        throw std::invalid_argument(
            "DataSet name '" + name + "' must not include the characters " + invalid_chars);

    _name = name;
}

std::string DataSet::name() const
{
    if (!_name.empty())
        return _name;

    throw std::runtime_error("DataSet has no name yet");
}

const nsx::MetaData& DataSet::metadata() const
{
    return _metadata;
}

nsx::MetaData& DataSet::metadata()
{
    return _metadata;
}

} // namespace nsx
