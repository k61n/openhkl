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
#include "base/geometry/AABB.h"
#include "base/geometry/DirectVector.h"
#include "base/geometry/ReciprocalVector.h"
#include "base/mask/BoxMask.h"
#include "base/mask/EllipseMask.h"
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
#include "core/image/GradientFilter.h"
#include "core/image/GradientFilterFactory.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentStateSet.h"
#include "core/instrument/InterpolatedState.h"
#include "core/instrument/Monochromator.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "core/loader/HDF5DataReader.h"
#include "core/loader/IDataReader.h"
#include "core/loader/NexusDataReader.h"
#include "core/loader/PlainTextReader.h"
#include "core/loader/RawDataReader.h"
#include "core/loader/TiffDataReader.h"
#include "core/peak/Peak3D.h"

#include <Eigen/src/Core/Matrix.h>
#include <H5Cpp.h>
#include <gsl/gsl_histogram.h>

#include <filesystem>
#include <memory>
#include <regex>
#include <stdexcept>

namespace ohkl {

DataSet::DataSet(const std::string& dataset_name, Diffractometer* diffractometer)
    : _diffractometer{diffractometer}
    , _states(nullptr)
    , _total_histogram(nullptr)
    , _buffered(false)
    , _all_frames_buffered(true)
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
        case DataFormat::TIFF: _reader.reset(new TiffDataReader); break;
        case DataFormat::PLAINTEXT: _reader.reset(new PlainTextReader); break;
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

void DataSet::addDataFile(const std::string& filename, const DataFormat& datafmt)
{
    // if reader not set yet, initialize a proper reader
    if (_reader)
        throw std::runtime_error("DataSet '" + _name + "': DataReader is already set.");

    setReader(datafmt, filename);
}

void DataSet::setImageReaderParameters(const DataReaderParameters& params)
{
    _dataformat = params.data_format;

    if (!_reader)
        setReader(_dataformat);

    switch (_dataformat) {
        case DataFormat::RAW: {
            RawDataReader& rawreader = *static_cast<RawDataReader*>(_reader.get());
            rawreader.setParameters(params);
            break;
        }
        case DataFormat::TIFF: {
            TiffDataReader& tiffreader = *static_cast<TiffDataReader*>(_reader.get());
            tiffreader.setParameters(params);
            break;
        }
        case DataFormat::PLAINTEXT: {
            PlainTextReader& textreader = *static_cast<PlainTextReader*>(_reader.get());
            textreader.setParameters(params);
            break;
        }
        default: {
            throw std::runtime_error("DataSet '" + _name + "': invalid data format");
        }
    }

    ohklLog(Level::Info, "DataSet '" + _name + "': DataReaderParameters set.");
}

void DataSet::addFrame(const std::string& filename, const DataFormat& format)
{
    if (!_reader)
        setReader(format);

    if (_dataformat != format)
        throw std::runtime_error("DataSet:addFrame: format does not match existing images");

    if (!std::filesystem::exists(filename))
        throw std::runtime_error("DataSet::addFrame: " + filename + " does not exist");

    switch (format) {
        case DataFormat::TIFF: {
            TiffDataReader& tiffreader = *static_cast<TiffDataReader*>(_reader.get());
            tiffreader.addFrame(filename);
            break;
        }
        case DataFormat::RAW: {
            RawDataReader& rawreader = *static_cast<RawDataReader*>(_reader.get());
            rawreader.addFrame(filename);
            break;
        }
        case DataFormat::PLAINTEXT: {
            PlainTextReader& textreader = *static_cast<PlainTextReader*>(_reader.get());
            textreader.addFrame(filename);
            break;
        }
        default: throw std::runtime_error("DataSet::addFrame: unrecognised data format");
    }
}

Eigen::MatrixXi DataSet::frame(const std::size_t idx) const
{
    if (_buffered) {
        if (_frame_buffer.at(idx))
            return *_frame_buffer.at(idx);
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

void DataSet::addBoxMask(const AABB& aabb)
{
    addMask(new BoxMask(aabb));
}

void DataSet::addEllipseMask(const AABB& aabb)
{
    addMask(new EllipseMask(aabb));
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
                peak->setRejectionFlag(RejectionFlag::Masked, true);
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

Eigen::MatrixXd DataSet::gradientFrame(std::size_t idx, GradientFilterType kernel) const
{
    ohklLog(Level::Debug, "Computing gradient of frame ", idx);
    std::string filter_type = GradientFilterStrings.at(kernel);
    GradientFilterFactory factory;
    GradientFilter* filter = factory.create(filter_type);
    filter->setImage(transformedFrame((idx)));
    filter->filter();
    return filter->filteredImage();
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
    dRange();
}

InstrumentStateList& DataSet::instrumentStates()
{
    return _states->instrumentStates();
}

void DataSet::adjustDirectBeam(double x_offset, double y_offset)
{
    ohklLog(Level::Info, "DataSet::adjustDirectBeam: offset (", x_offset, ", ", y_offset, ")");
    double x_coord = x_offset + static_cast<double>(nCols()) / 2.0;
    double y_coord = y_offset + static_cast<double>(nRows()) / 2.0;
    ohklLog(Level::Info, "DataSet::adjustDirectBeam: position (", x_coord, ", ", y_coord, ")");
    DirectVector direct = detector().pixelPosition(x_coord, y_coord);
    for (auto& state : instrumentStates())
        state.adjustKi(direct);
    ohklLog(Level::Info, "DataSet::adjustDirectBeam: finished");
}

void DataSet::initHistograms(std::size_t nbins)
{
    double max_count = maxCount();
    for (int i = 0; i < nFrames(); ++i) {
        gsl_histogram* h = nullptr;
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


void DataSet::dRange()
{
    const auto* detector = _diffractometer->detector();
    const InstrumentStateList& states = _states->getInstrumentStateList();
    auto state = InterpolatedState::interpolate(states, 0);
    DirectVector from(state.samplePosition);

    // Work out if the direct beam intersects the detector
    Eigen::RowVector3d kf = state.ki().rowVector();
    const DetectorEvent beam = detector->constructEvent(
        from, ReciprocalVector(kf * state.detectorOrientation), 0, false);
    bool beam_on_detector = false;
    if (beam.px >= 0 && beam.px <= nCols() && beam.py >= 0 && beam.py <= nRows())
        beam_on_detector = true;

    // Compute q vectors and d of corners of detector

    std::vector<std::pair<int, int>> corners;
    corners.push_back({0, 0});
    corners.push_back({0, nCols()});
    corners.push_back({nRows(), 0});
    corners.push_back({nRows(), nCols()});
    _d_min = 10000;
    _d_max = 0;
    double eps = 2.0e-2;

    for (const auto& [y, x] : corners) {
        DirectVector pos(detector->pixelPosition(x, y));
        ReciprocalVector q = state.sampleQ(pos);
        double modq = q.rowVector().norm();
        double d;
        if (modq < eps)
            d = 50.0;
        else
            d = 1.0 / modq;
        _d_min = std::min(d, _d_min);
        if (!beam_on_detector)
            _d_max = std::max(d, _d_max);
        else
            _d_max = 50;
    }
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

void DataSet::removeAllMasks()
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
    _all_frames_buffered = bufferAll;
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
