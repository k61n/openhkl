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
#include "base/utils/Path.h" // splitFileExtension
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
#include "core/loader/RawDataReader.h"


#include <H5Cpp.h>

namespace nsx {

DataSet::DataSet(const std::string& dataset_name, Diffractometer* diffractometer)
    : _diffractometer{diffractometer}
{
    setName(dataset_name);
    if (!diffractometer)
        nsxlog(Level::Warning, "DataSet '", dataset_name, "' has no diffractometer.");
    if (_diffractometer && _diffractometer->detector()) {
        datashape[0] = 0;
        datashape[1] = nRows();
        datashape[2] = nCols();
    }
}

DataSet::DataSet(std::shared_ptr<IDataReader> reader)
    : _reader{std::move(reader)}
{
    _metadata.setMap(_reader->metadata().map());

    double wav = _metadata.key<double>(nsx::at_wavelength);
    _reader->diffractometer()->source().selectedMonochromator().setWavelength(wav);

    // Getting Scan parameters for the detector
    _states.reserve(_nFrames);

    for (unsigned int i = 0; i < _nFrames; ++i)
        _states.push_back(_reader->state(i));
}

DataSet::~DataSet() { }

void DataSet::_setReader(const DataFormat dataformat, const std::string& filename) {
    nsxlog(Level::Debug, "Initializing a DataReader for the format ", dataformat);

    switch(dataformat) {
    case DataFormat::NSX:
        _reader.reset(new HDF5DataReader(filename, _diffractometer));
        break;
    case DataFormat::RAW:
        // NOTE: RawDataReader needs a list of frame files which should be given later
        _reader.reset(new RawDataReader("::RawDataFile::", diffractometer));
        break;
    case DataFormat::NEXUS:
        _reader.reset(new NexusDataReader(filename, _diffractometer));
        break;
    default:
        throw std::invalid_argument("Data format is not recognized.");
    }

    _dataformat = dataformat;
    _reader->setDataSet(this);
    _reader->initRead();

    // Update the monochromator wavelength
    diffractometer()->source().selectedMonochromator().setWavelength(wavelength());

    // Compute the instrument states for all frames
    const std::size_t nframes = nFrames();
    _states.reserve(nframes);

    for (unsigned int i = 0; i < nframes; ++i)
        _states.push_back(_reader->state(i));
}

void DataSet::addDataFile(const std::string& filename, const std::string& extension) {
{
    // if reader not set yet, initialize a proper reader
    if (!_reader) {
        const std::string ext = lowerCase(extension);
        DataFormat datafmt {DataFormat::Unknown};
        if (ext == "nsx" || ext == "hdf") {
            datafmt = DataFormat::NSX;
        } else if (ext == "nxs") {
            datafmt = DataFormat::NEXUS;
        } else if (ext == "raw") {
            datafmt = DataFormat::RAW;
        }
        _setReader(datafmt, filename);

    } else {
        throw std::runtime_error("DataReader is already set.");
    }
}

void DataSet::addRawFrame(const std::string& rawfilename,
                          const RawDataReaderParameters* const params) {
    if (!_reader)
        _setReader(DataFormat::RAW);

    // prevent mixing data formats
    if (_dataformat != DataFormat::RAW)
        throw std::runtime_error("To read a raw frame, data format must be raw.");

    RawDataReader& rawreader = *static_cast<RawDataReader*>(_reader.get());
    if (params)
        rawreader.setParameters(params);

    rawreader.addFrame(rawfilename);
}

int DataSet::dataAt(const std::size_t x, const std::size_t y, const std::size_t z) const
{
    const std::size_t nframes = nFrames(), ncols = nCols(), nrows = nRows();
    // Check that the voxel is inside the limit of the data
    if (z >= nframes || y >= ncols || x >= nrows) {
        throw std::runtime_error
        ("DataSet '" + _name + "': Out-of-bound access ("
         + "x = " + std::to_string(x) + "/" + std::to_string(nrows)
         + ", y = " + std::to_string(y) + "/" + std::to_string(ncols)
         + ", z = " + std::to_string(z) + "/" + std::to_string(nframes)
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

inline
std::size_t DataSet::nFrames() const
{
    return metadata().key<int>(nsx::at_frameCount);
}

inline
std::size_t DataSet::nCols() const
{
    return detector().nCols();
}

inline
std::size_t DataSet::nRows() const
{
    return detector().nRows();
}

inline
double DataSet::wavelength() const
{
    return _metadata.key<double>(nsx::at_wavelength);
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

inline
const nsx::MetaData& DataSet::metadata() const
{
    return _metadata;
}

inline
nsx::MetaData& DataSet::metadata()
{
    return _metadata;
}

} // namespace nsx
