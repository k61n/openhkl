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
#include "core/detector/Detector.h"

#include "base/parser/BloscFilter.h"
#include "base/utils/Path.h"
#include "base/utils/ProgressHandler.h"
#include "base/utils/Units.h" // deg
#include "core/detector/Detector.h"
#include "core/detector/DetectorEvent.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Monochromator.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"

#include <H5Cpp.h>

namespace nsx {

DataSet::DataSet(std::shared_ptr<IDataReader> reader)
    : _isOpened{false}
    , _filename{reader->filename()}
    , _nFrames{0}
    , _nrows{0}
    , _ncols{0}
    , _fileSize{0}
    , _background{0.0}
    , _reader{std::move(reader)}
{
    if (!fileExists(_filename))
        throw std::runtime_error("IData, file: " + _filename + " does not exist");

    _nrows = detector().nRows();
    _ncols = detector().nCols();
    _nFrames = _reader->metadata().key<int>("npdone");

    double wav = _reader->metadata().key<double>("wavelength");
    _reader->diffractometer()->source().selectedMonochromator().setWavelength(wav);

    // Getting Scan parameters for the detector
    _states.reserve(_nFrames);

    for (unsigned int i = 0; i < _nFrames; ++i)
        _states.push_back(_reader->state(i));
}

DataSet::~DataSet()
{
    blosc_destroy();
}

int DataSet::dataAt(unsigned int x, unsigned int y, unsigned int z) const
{
    // Check that the voxel is inside the limit of the data
    if (z >= _nFrames || y >= _ncols || x >= _nrows)
        return 0;
    return frame(z)(x, y);
}

Eigen::MatrixXi DataSet::frame(std::size_t idx) const
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

const std::string& DataSet::filename() const
{
    return _filename;
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

bool DataSet::isOpened() const
{
    return _isOpened;
}

std::size_t DataSet::fileSize() const
{
    return _fileSize;
}

void DataSet::saveHDF5(const std::string& filename) // const
{
    blosc_init();
    blosc_set_nthreads(4);

    hsize_t dims[3] = {_nFrames, _nrows, _ncols};
    hsize_t chunk[3] = {1, _nrows, _ncols};
    hsize_t count[3] = {1, _nrows, _ncols};

    H5::H5File file(filename.c_str(), H5F_ACC_TRUNC);
    H5::DataSpace space(3, dims, nullptr);
    H5::DSetCreatPropList plist;

    plist.setChunk(3, chunk);

    char *version, *date;
    int r;
    unsigned int cd_values[7];
    cd_values[4] = 9; // Highest compression level
    cd_values[5] = 1; // Bit shuffling active
    cd_values[6] = BLOSC_BLOSCLZ; // Seem to be the best compromise
                                  // speed/compression for diffraction data

    r = register_blosc(&version, &date);
    if (r <= 0)
        throw std::runtime_error("Problem registering BLOSC filter in HDF5 library");

    // caught by valgrind memcheck
    free(version);
    version = nullptr;
    free(date);
    date = nullptr;
    plist.setFilter(FILTER_BLOSC, H5Z_FLAG_OPTIONAL, 7, cd_values);

    H5::DataSpace memspace(3, count, nullptr);
    H5::Group dataGroup(file.createGroup("/Data"));
    H5::DataSet dset(dataGroup.createDataSet("Counts", H5::PredType::NATIVE_INT32, space, plist));

    hsize_t offset[3];
    offset[0] = 0;
    offset[1] = 0;
    offset[2] = 0;

    using IntMatrix = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
    for (offset[0] = 0; offset[0] < _nFrames; offset[0] += count[0]) {
        space.selectHyperslab(H5S_SELECT_SET, count, offset, nullptr, nullptr);
        // HDF5 requires row-major storage, so copy frame into a row-major matrix
        IntMatrix current_frame(frame(offset[0]));
        dset.write(current_frame.data(), H5::PredType::NATIVE_INT32, memspace, space);
    }

    // Saving the scans parameters (detector, sample and source)
    H5::Group scanGroup(dataGroup.createGroup("Scan"));

    // Write detector states
    H5::Group detectorGroup(scanGroup.createGroup("Detector"));

    hsize_t nf[1] = {_nFrames};
    H5::DataSpace scanSpace(1, nf);

    const auto& detectorStates = _reader->detectorStates();

    const auto& detector_gonio = detector().gonio();
    size_t n_detector_gonio_axes = detector_gonio.nAxes();
    for (size_t i = 0; i < n_detector_gonio_axes; ++i) {
        const auto& axis = detector_gonio.axis(i);
        Eigen::VectorXd values(_nFrames);
        for (size_t j = 0; j < _nFrames; ++j)
            values(j) = detectorStates[j][i] / deg;
        H5::DataSet detectorScan(
            detectorGroup.createDataSet(axis.name(), H5::PredType::NATIVE_DOUBLE, scanSpace));
        detectorScan.write(&values(0), H5::PredType::NATIVE_DOUBLE, scanSpace, scanSpace);
    }

    // Write sample states
    H5::Group sampleGroup(scanGroup.createGroup("Sample"));

    const auto& sampleStates = _reader->sampleStates();

    const auto& sample_gonio = _reader->diffractometer()->sample().gonio();
    size_t n_sample_gonio_axes = sample_gonio.nAxes();

    for (size_t i = 0; i < n_sample_gonio_axes; ++i) {
        const auto& axis = sample_gonio.axis(i);
        Eigen::VectorXd values(_nFrames);
        for (size_t j = 0; j < _nFrames; ++j)
            values(j) = sampleStates[j][i] / deg;
        H5::DataSet sampleScan(
            sampleGroup.createDataSet(axis.name(), H5::PredType::NATIVE_DOUBLE, scanSpace));
        sampleScan.write(&values(0), H5::PredType::NATIVE_DOUBLE, scanSpace, scanSpace);
    }

    const auto& map = _reader->metadata().map();

    // Write all string metadata into the "Info" group
    H5::Group infogroup(file.createGroup("/Info"));
    H5::DataSpace metaSpace(H5S_SCALAR);
    H5::StrType str80(H5::PredType::C_S1, 80);

    for (const auto& item : map) {
        try {
            if (std::holds_alternative<std::string>(item.second)) {
                std::string info = std::get<std::string>(item.second);
                H5::Attribute intAtt(infogroup.createAttribute(item.first, str80, metaSpace));
                intAtt.write(str80, info);
            }
        } catch (const std::exception& ex) {
            std::cerr << "Exception in " << __PRETTY_FUNCTION__ << ": " << ex.what() << std::endl;
        } catch (...) {
            std::cerr << "Uncaught exception in " << __PRETTY_FUNCTION__ << std::endl;
        }
    }

    // Write all other metadata (int and double) into the "Experiment" Group
    H5::Group metadatagroup(file.createGroup("/Experiment"));

    for (const auto& item : map) {
        try {
            if (std::holds_alternative<int>(item.second)) {
                int value = std::get<int>(item.second);
                H5::Attribute intAtt(metadatagroup.createAttribute(
                    item.first, H5::PredType::NATIVE_INT32, metaSpace));
                intAtt.write(H5::PredType::NATIVE_INT, &value);
            } else if (std::holds_alternative<double>(item.second)) {
                double dvalue = std::get<double>(item.second);
                H5::Attribute intAtt(metadatagroup.createAttribute(
                    item.first, H5::PredType::NATIVE_DOUBLE, metaSpace));
                intAtt.write(H5::PredType::NATIVE_DOUBLE, &dvalue);
            }
        } catch (const std::exception& ex) {
            std::cerr << "Exception in " << __PRETTY_FUNCTION__ << ": " << ex.what() << std::endl;
        } catch (...) {
            std::cerr << "Uncaught exception in " << __PRETTY_FUNCTION__ << std::endl;
        }
    }
    file.close();
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

void DataSet::maskPeaks(PeakList& peaks) const
{
    for (auto peak : peaks) {
        // peak belongs to another dataset
        if (peak->dataSet().get() != this)
            continue;

        peak->setMasked(false);
        for (const auto& m : _masks) {
            // If the background of the peak intercept the mask, unselected the peak
            if (m->collide(peak->shape())) {
                peak->setMasked(true);
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

void DataSet::setName(std::string name)
{
    _name = name;
}

std::string DataSet::name() const
{
    std::string name;
    std::string ext;
    std::string data_name = filename();

    size_t sep = data_name.find_last_of("\\/");
    if (sep != std::string::npos)
        data_name = data_name.substr(sep + 1, data_name.size() - sep - 1);

    size_t dot = data_name.find_last_of('.');
    if (dot != std::string::npos) {
        name = data_name.substr(0, dot);
        ext = data_name.substr(dot, data_name.size() - dot);
    } else {
        name = data_name;
        ext = "";
    }

    if (!(_name == ""))
        return _name;
    return name;
}

} // namespace nsx
