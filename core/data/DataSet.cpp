//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
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
#include "base/utils/StringIO.h" // lowerCase
#include "base/utils/Units.h" // deg
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


#include <H5Cpp.h>
#include <gsl/gsl_histogram.h>

#include <stdexcept>

namespace ohkl {

DataSet::DataSet(const std::string& dataset_name, Diffractometer* diffractometer)
    : _diffractometer{diffractometer}, _states(nullptr), _total_histogram(nullptr)
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

void DataSet::_setReader(const DataFormat dataformat, const std::string& filename)
{
    ohklLog(Level::Debug, "Initializing a DataReader for the format ", static_cast<int>(dataformat));

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

    _setReader(datafmt, filename);
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
        _setReader(DataFormat::RAW);

    RawDataReader& rawreader = *static_cast<RawDataReader*>(_reader.get());
    rawreader.setParameters(params);

    ohklLog(
        Level::Info,
        "DataSet '" + _name + "': RawDataReader parameters set."); // TODO: log parameter details
}

void DataSet::addRawFrame(const std::string& rawfilename)
{
    if (!_reader)
        _setReader(DataFormat::RAW);

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

void DataSet::addMask(IMask* mask)
{
    _masks.insert(mask);
    _metadata.add<int>(ohkl::at_nMasks, _masks.size());
}

void DataSet::removeMask(IMask* mask)
{
    if (_masks.find(mask) != _masks.end())
        _masks.erase(mask);
    _metadata.add<int>(ohkl::at_nMasks, _masks.size());
}

const std::set<IMask*>& DataSet::masks() const
{
    return _masks;
}

void DataSet::maskPeaks(
    std::vector<Peak3D*>& peaks, std::map<Peak3D*, RejectionFlag>& rejection_map,
    double bkg_end /* = -1.0 */) const
{
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
    if (_total_histogram != nullptr) gsl_histogram_free(_total_histogram);
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

} // namespace ohkl
