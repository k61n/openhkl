//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/data/DataSet.cpp
//! @brief     Implements class DataSet
//!
//! @homepage  https://openhkl.org
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
#include "base/utils/StringIO.h" // lowerCase
#include "base/utils/Units.h" // deg
#include "core/convolve/ConvolverFactory.h"
#include "core/data/DataTypes.h"
#include "core/detector/Detector.h"
#include "core/detector/DetectorEvent.h"
#include "core/experiment/ExperimentExporter.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentStateSet.h"
#include "core/instrument/InterpolatedState.h"
#include "core/instrument/Monochromator.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "core/loader/HDF5DataReader.h"
#include "core/loader/NexusDataReader.h"
#include "core/loader/RawDataReader.h"
#include "core/peak/Peak3D.h"
#include "core/raw/DataKeys.h"


#include <Eigen/src/Core/Matrix.h>
#include <H5Cpp.h>
#include <gsl/gsl_histogram.h>

#include <memory>
#include <stdexcept>

namespace ohkl {

DataSet::DataSet(const std::string& dataset_name, Diffractometer* diffractometer)
    : _diffractometer{diffractometer}
    , _states(nullptr)
    , _total_histogram(nullptr)
    , _buffered(false)
{
    setName(dataset_name);
    if (!_diffractometer)
        ohklLog(Level::Warning, "DataSet '", dataset_name, "' has no diffractometer.");
    if (_diffractometer && _diffractometer->detector()) {
        datashape[0] = nCols();
        datashape[1] = nRows();
        datashape[2] = 0; // nr of frames
    }
    _metadata.add<int>(ohkl::at_nMasks, 0);
}

void DataSet::setReader(const DataFormat dataformat, const std::string& filename)
{
    ohklLog(
        Level::Debug, "Initializing a DataReader for the format ", static_cast<int>(dataformat));

    switch (dataformat) {
        case DataFormat::OHKL: _reader.reset(new HDF5DataReader(filename)); break;
        case DataFormat::NEXUS: _reader.reset(new NexusDataReader(filename)); break;
        case DataFormat::RAW:
            // NOTE: RawDataReader needs a list of frame files which should be given later
            _reader.reset(new RawDataReader);
            break;
        default: throw std::invalid_argument("Data format is not recognized.");
    }

    _dataformat = dataformat;
    _reader->setDataSet(this);
    _reader->initRead();
}

void DataSet::finishRead()
{
    if (!_reader)
        throw std::invalid_argument("DataSet '" + _name + "': Data reader is not set.");

    if (!diffractometer())
        throw std::invalid_argument("DataSet '" + _name + "': Diffractometer is not set.");

    // Update the monochromator wavelength
    diffractometer()->source().selectedMonochromator().setWavelength(wavelength());
}

void DataSet::addDataFile(const std::string& filename, const std::string& extension)
{
    DataFormat datafmt{DataFormat::Unknown};

    // if reader not set yet, initialize a proper reader
    if (!_reader) {
        const std::string ext = lowerCase(extension);

        if (ext == "nsx" || ext == "hdf" || ext == "ohkl")
            datafmt = DataFormat::OHKL;
        else if (ext == "nxs")
            datafmt = DataFormat::NEXUS;
        else if (ext == "raw")
            throw std::runtime_error(
                "DataSet '" + _name + "': Use 'addRawFrame(<filename>)' for reading raw files.");
        else
            throw std::runtime_error("DataSet '" + _name + "': Extension unknown.");

    } else {
        throw std::runtime_error("DataSet '" + _name + "': DataReader is already set.");
    }

    setReader(datafmt, filename);
}

void DataSet::setRawReaderParameters(const RawDataReaderParameters& params)
{
    // if data-format is not set, then set it to raw.
    if (_dataformat == DataFormat::Unknown)
        _dataformat = DataFormat::RAW;

    // prevent mixing data formats
    if (_dataformat != DataFormat::RAW)
        throw std::runtime_error(
            "DataSet '" + _name + "': Cannot set raw parameters since data format is not raw.");

    if (!_reader)
        setReader(DataFormat::RAW);

    RawDataReader& rawreader = *static_cast<RawDataReader*>(_reader.get());
    rawreader.setParameters(params);

    ohklLog(
        Level::Info,
        "DataSet '" + _name + "': RawDataReader parameters set."); // TODO: log parameter details
}

void DataSet::addRawFrame(const std::string& rawfilename)
{
    if (!_reader)
        setReader(DataFormat::RAW);

    // prevent mixing data formats
    if (_dataformat != DataFormat::RAW)
        throw std::runtime_error(
            "DataSet '" + _name + "': To read a raw frame, data format must be raw.");

    RawDataReader& rawreader = *static_cast<RawDataReader*>(_reader.get());

    rawreader.addFrame(rawfilename);
}

int DataSet::dataAt(const std::size_t x, const std::size_t y, const std::size_t z) const
{
    const std::size_t nframes = nFrames(), ncols = nCols(), nrows = nRows();
    // Check that the voxel is inside the limit of the data
    if (z >= nframes || y >= ncols || x >= nrows) {
        throw std::runtime_error(
            "DataSet '" + _name + "': Out-of-bound access (" + "x = " + std::to_string(x) + "/"
            + std::to_string(nrows) + ", y = " + std::to_string(y) + "/" + std::to_string(ncols)
            + ", z = " + std::to_string(z) + "/" + std::to_string(nframes) + ")");
    }

    return frame(z)(x, y);
}

Eigen::MatrixXi DataSet::frame(const std::size_t idx) const
{
    if (_buffered) {
        if (_frame_buffer.at(idx)) {
            return *_frame_buffer.at(idx);
        }
    }
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
    return metadata().key<int>(ohkl::at_frameCount);
}

std::size_t DataSet::nCols() const
{
    return detector().nCols();
}

std::size_t DataSet::nRows() const
{
    return detector().nRows();
}

double DataSet::wavelength() const
{
    return _metadata.key<double>(ohkl::at_wavelength);
}

BitDepth DataSet::bitDepth() const
{
    int bd = _metadata.key<int>(ohkl::at_bitDepth);
    return static_cast<BitDepth>(bd);
}

void DataSet::addMask(IMask* mask)
{
    _masks.emplace_back(mask);
    _metadata.add<int>(ohkl::at_nMasks, _masks.size());
}

void DataSet::removeMask(IMask* mask)
{
    _masks.erase(std::remove(_masks.begin(), _masks.end(), mask), _masks.end());
    _metadata.add<int>(ohkl::at_nMasks, _masks.size());
}

bool DataSet::removeMaskByIndex(std::vector<size_t> idx)
{
    if (_masks.size() == 0)
        return false;
    if (idx.size() == 0)
        return false;
    std::vector<IMask*> masks_to_delete;
    for (auto e : idx)
        masks_to_delete.emplace_back(_masks.at(e));

    for (auto e : masks_to_delete) // now delete
        removeMask(e);

    return true;
}

const std::vector<IMask*>& DataSet::masks() const
{
    return _masks;
}

void DataSet::maskPeaks(
    std::vector<Peak3D*>& peaks, std::map<Peak3D*, RejectionFlag>& rejection_map,
    double bkg_end /* = -1.0 */) const
{
    ohklLog(Level::Info, "DataSet::maskPeaks: Masking ", peaks.size(), " peaks");
    int n_masked = 0;
    for (const auto& peak : peaks) {
        // peak belongs to another dataset
        if (peak->dataSet().get() != this)
            continue;

        peak->setMasked(false);
        // scale the peak shape to the maximum background radius
        Ellipsoid shape = peak->shape();
        if (bkg_end < 0.0)
            shape.scale(peak->bkgEnd());
        else
            shape.scale(bkg_end);
        for (const auto& m : _masks) {
            // If the background of the peak intercepts the mask, unselected the peak
            if (m->collide(shape)) {
                rejection_map.insert_or_assign(peak, peak->rejectionFlag());
                peak->setMasked(true);
                ++n_masked;
                break;
            }
        }
    }
    ohklLog(Level::Info, n_masked, " peaks masked");
}

ReciprocalVector DataSet::computeQ(const DetectorEvent& ev) const
{
    if (!_states)
        throw std::runtime_error("InstrumentStates not set");
    const auto& state = InterpolatedState::interpolate(_states->instrumentStates(), ev.frame);
    const auto& detector_position = DirectVector(detector().pixelPosition(ev.px, ev.py));
    return state.sampleQ(detector_position);
}

Eigen::MatrixXd DataSet::transformedFrame(std::size_t idx) const
{
    ohklLog(Level::Debug, "Transforming (baseline/gain) frame ", idx);
    Eigen::ArrayXXd new_frame = frame(idx).cast<double>();
    new_frame -= detector().baseline();
    new_frame /= detector().gain();
    return new_frame;
}

Eigen::MatrixXd DataSet::gradientFrame(std::size_t idx, GradientKernel kernel, bool realspace) const
{
    ohklLog(Level::Debug, "Computing gradient of frame ", idx);
    ImageGradient grad(transformedFrame(idx));
    grad.compute(kernel, realspace);
    return grad.magnitude();
}

const IDataReader* DataSet::reader() const
{
    return _reader.get();
}

IDataReader* DataSet::reader()
{
    return _reader.get();
}

const Diffractometer* DataSet::diffractometer() const
{
    return _diffractometer;
}

Diffractometer* DataSet::diffractometer()
{
    return _diffractometer;
}

Detector& DataSet::detector()
{
    return *(_diffractometer->detector());
}

const Detector& DataSet::detector() const
{
    return *(_diffractometer->detector());
}

void DataSet::setName(const std::string& name)
{
    if (name.empty()) {
        ohklLog(Level::Warning, "Given name for the DataSet is empty.");
        return;
    }

    const std::string invalid_chars{"\\/"};
    const std::size_t sep = name.find_first_of(invalid_chars);
    if (sep != std::string::npos) {
        ohklLog(
            Level::Warning, "Given name, '", name,
            "' for the DataSet includes disallowed characters.");
        throw std::invalid_argument(
            "DataSet name '" + name + "' " + "must not include the characters " + invalid_chars);
    }

    _name = name;
}

std::string DataSet::name() const
{
    if (!_name.empty())
        return _name;

    throw std::runtime_error("DataSet has no name yet");
}

const ohkl::MetaData& DataSet::metadata() const
{
    return _metadata;
}

ohkl::MetaData& DataSet::metadata()
{
    return _metadata;
}

void DataSet::setInstrumentStates(InstrumentStateSet* states)
{
    _states = states;
}

InstrumentStateList& DataSet::instrumentStates()
{
    return _states->instrumentStates();
}

void DataSet::adjustDirectBeam(double x_offset, double y_offset)
{
    ohklLog(
        Level::Info, "DataSet::adjustDirectBeam: offset (",
        x_offset, ", ", y_offset, ")");
    double x_coord = x_offset + static_cast<double>(nCols()) / 2.0;
    double y_coord = y_offset + static_cast<double>(nRows()) / 2.0;
    ohklLog(
        Level::Info, "DataSet::adjustDirectBeam: position (",
        x_coord, ", ", y_coord, ")");
    DirectVector direct = detector().pixelPosition(x_coord, y_coord);
    for (auto& state : instrumentStates())
        state.adjustKi(direct);
    ohklLog(Level::Info, "DataSet::adjustDirectBeam: finished");
}

void DataSet::initHistograms(std::size_t nbins)
{
    double max_count = maxCount();
    for (int i = 0; i < nFrames(); ++i) {
        gsl_histogram* h;
        _histograms.push_back(h);
    }
    _total_histogram = gsl_histogram_alloc(nbins);
    gsl_histogram_set_ranges_uniform(_total_histogram, 0, max_count);
    for (std::size_t index = 0; index < nFrames(); ++index) {
        _histograms[index] = gsl_histogram_alloc(nbins);
        gsl_histogram_set_ranges_uniform(_histograms[index], 0, max_count);
    }
}

void DataSet::getFrameIntensityHistogram(std::size_t index)
{
    auto image_mat = frame(index);
    for (std::size_t col = 0; col < nCols(); ++col) {
        for (std::size_t row = 0; row < nRows(); ++row) {
            gsl_histogram_increment(_histograms[index], image_mat(row, col));
            gsl_histogram_increment(_total_histogram, image_mat(row, col));
        }
    }
}

void DataSet::getIntensityHistogram(std::size_t nbins)
{
    initHistograms(nbins);
    for (std::size_t index = 0; index < nFrames(); ++index)
        getFrameIntensityHistogram(index);
    // gsl_histogram_fprintf(stdout, _total_histogram, "%g", "%g");
}

void DataSet::clearHistograms()
{
    if (_total_histogram != nullptr)
        gsl_histogram_free(_total_histogram);
    for (auto* hist : _histograms)
        gsl_histogram_free(hist);

    _total_histogram = nullptr;
    _histograms.clear();
}

double DataSet::maxCount()
{
    double max_count = 0;
    for (int i = 0; i < nFrames(); ++i)
        max_count = std::max(max_count, static_cast<double>(frame(i).maxCoeff()));
    return max_count;
}

gsl_histogram* DataSet::getHistogram(int index)
{
    if (index >= _histograms.size())
        return nullptr;
    return _histograms.at(index);
}

gsl_histogram* DataSet::getTotalHistogram()
{
    return _total_histogram;
}

bool DataSet::hasMasks()
{
    return _masks.size() > 0;
}

size_t DataSet::getNMasks()
{
    return _masks.size();
}

void DataSet::removeAllMaks()
{
    if (_masks.size() > 0)
        _masks.clear();
}

void DataSet::initBuffer(bool bufferAll)
{
    // Not initialised in the constructor since there will not be any images present
    if (_buffered)
        return;
    _frame_buffer.clear();
    for (std::size_t frame = 0; frame < nFrames(); ++frame)
        _frame_buffer.push_back(nullptr);
    for (std::size_t idx = 0; idx < nFrames(); ++idx) {
        if (bufferAll)
            _frame_buffer.at(idx) = std::make_unique<Eigen::MatrixXi>(_reader->data(idx));
    }
    _buffered = true;
    ohklLog(Level::Debug, "DataSet::initBuffer: ", _name, " buffered");
}

void DataSet::clearBuffer()
{
    if (!_buffered)
        return;
    for (std::size_t idx = 0; idx < nFrames(); ++idx) {
        _frame_buffer.at(idx).reset();
        _frame_buffer.at(idx) = nullptr;
    }
    _buffered = false;
    ohklLog(Level::Debug, "DataSet::clearBuffer: ", _name, " buffer cleared");
}

} // namespace ohkl
