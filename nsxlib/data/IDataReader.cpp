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

    InstrumentState state;

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

#if 0
void IDataReader::saveHDF5(const std::string& filename)
{
    blosc_init();
    blosc_set_nthreads(4);

    hsize_t dims[3] = {_nFrames, _nRows,_nCols};
    hsize_t chunk[3] = {1, _nRows,_nCols};
    hsize_t count[3] = {1, _nRows,_nCols};

    H5::H5File file(filename.c_str(), H5F_ACC_TRUNC);
    H5::DataSpace space(3, dims, nullptr);
    H5::DSetCreatPropList plist;

    plist.setChunk(3, chunk);

    char *version, *date;
    int r;
    unsigned int cd_values[7];
    cd_values[4] = 9;       // Highest compression level
    cd_values[5] = 1;       // Bit shuffling active
    cd_values[6] = BLOSC_BLOSCLZ; // Seem to be the best compromise speed/compression for diffraction data

    r = register_blosc(&version, &date);
    if (r<=0) {
        throw std::runtime_error("Problem registering BLOSC filter in HDF5 library");
    }

    // caught by valgrind memcheck
    free(version); version = nullptr;
    free(date); date = nullptr;
    plist.setFilter(FILTER_BLOSC, H5Z_FLAG_OPTIONAL, 7, cd_values);

    H5::DataSpace memspace(3, count, nullptr);
    H5::Group dataGroup(file.createGroup("/Data"));
    H5::DataSet dset(dataGroup.createDataSet("Counts", H5::PredType::NATIVE_INT32, space, plist));

    hsize_t offset[3];
    offset[0] = 0;
    offset[1] = 0;
    offset[2] = 0;

    for(offset[0]=0; offset[0] < _nFrames; offset[0] += count[0]) {
        space.selectHyperslab(H5S_SELECT_SET, count, offset, nullptr, nullptr);
        // HDF5 requires row-major storage, so copy frame into a row-major matrix
        IntMatrix frame(getData(offset[0]));
        dset.write(frame.data(), H5::PredType::NATIVE_INT32, memspace, space);
    }

    // Saving the scans parameters (detector, sample and source)
    H5::Group scanGroup(dataGroup.createGroup("Scan"));

    // Write detector states
    H5::Group detectorGroup(scanGroup.createGroup("Detector"));

    std::vector<std::string> names=_diffractometer->getDetector()->getGonio()->getPhysicalAxesNames();
    hsize_t nf[1]={_nFrames};
    H5::DataSpace scanSpace(1,nf);
    RealMatrix vals(names.size(),_nFrames);

    for (unsigned int i = 0; i < _detectorStates.size(); ++i) {
        auto&& v = _detectorStates[i].values();

        for (unsigned int j = 0; j < names.size(); ++j) {
            vals(j,i) = v[j] / deg;
        }
    }

    for (unsigned int j = 0; j < names.size(); ++j) {
        H5::DataSet detectorScan(detectorGroup.createDataSet(names[j], H5::PredType::NATIVE_DOUBLE, scanSpace));
        detectorScan.write(&vals(j,0), H5::PredType::NATIVE_DOUBLE, scanSpace, scanSpace);
    }

    // Write sample states
    H5::Group sampleGroup(scanGroup.createGroup("Sample"));
    std::vector<std::string> samplenames=_diffractometer->getSample()->getGonio()->getPhysicalAxesNames();
    RealMatrix valsSamples(samplenames.size(), _nFrames);

    for (unsigned int i = 0; i < _sampleStates.size(); ++i) {
        auto&& v = _sampleStates[i].values();

        for (unsigned int j = 0; j < samplenames.size(); ++j) {
            valsSamples(j,i) = v[j]/deg;
        }
    }

    for (unsigned int j = 0; j < samplenames.size(); ++j) {
        H5::DataSet sampleScan(sampleGroup.createDataSet(samplenames[j], H5::PredType::NATIVE_DOUBLE, scanSpace));
        sampleScan.write(&valsSamples(j,0), H5::PredType::NATIVE_DOUBLE, scanSpace, scanSpace);
    }

    // Write source states
    H5::Group sourceGroup(scanGroup.createGroup("Source"));
    std::vector<std::string> sourcenames = _diffractometer->getSource()->getGonio()->getPhysicalAxesNames();
    RealMatrix valsSources(sourcenames.size(),_nFrames);

    for (unsigned int i = 0; i < _states.size(); ++i) {
        auto&& v = _sourceStates[i].values();

        for (unsigned int j = 0; j < sourcenames.size(); ++j) {
            valsSources(j,i) = v[j] / deg;
        }
    }

    for (unsigned int j = 0; j < sourcenames.size(); ++j) {
        H5::DataSet sourceScan(sourceGroup.createDataSet(sourcenames[j], H5::PredType::NATIVE_DOUBLE, scanSpace));
        sourceScan.write(&valsSources(j,0), H5::PredType::NATIVE_DOUBLE, scanSpace, scanSpace);
    }

    const auto& map=_metadata.getMap();

    // Write all string metadata into the "Info" group
    H5::Group infogroup(file.createGroup("/Info"));
    H5::DataSpace metaSpace(H5S_SCALAR);
    H5::StrType str80(H5::PredType::C_S1, 80);
    std::string info;

    for (const auto& item: map) {
        std::string info;

        try {
            info = item.second.as<std::string>();
            H5::Attribute intAtt(infogroup.createAttribute(item.first, str80, metaSpace));
            intAtt.write(str80, info);
        } catch(...) {
            // shouldn't there be some error handling here?
        }
    }

    // Write all other metadata (int and double) into the "Experiment" Group
    H5::Group metadatagroup(file.createGroup("/Experiment"));

    for (const auto& item: map) {
        int value;

        try {
            value = item.second.as<int>();
            H5::Attribute intAtt(metadatagroup.createAttribute(item.first, H5::PredType::NATIVE_INT32, metaSpace));
            intAtt.write(H5::PredType::NATIVE_INT, &value);
        } catch(...) {
            try {
                double dvalue;
                dvalue = item.second.as<double>();
                H5::Attribute intAtt(metadatagroup.createAttribute(item.first, H5::PredType::NATIVE_DOUBLE, metaSpace));
                intAtt.write(H5::PredType::NATIVE_DOUBLE, &dvalue);
            } catch(...) {
                // TODO: shouldn't there be some error handling here?
            }
        }
    }
    file.close();
}
#endif 

const std::vector<ComponentState>& IDataReader::sampleStates() const
{
    return _sampleStates;
}

const std::vector<ComponentState>& IDataReader::detectorStates() const
{
    return _detectorStates;
}

} // end namespace nsx
