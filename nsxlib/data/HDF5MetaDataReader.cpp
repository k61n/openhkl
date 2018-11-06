#include <memory>
#include <stdexcept>

#include "blosc.h"

#include "BloscFilter.h"
#include "Detector.h"
#include "Diffractometer.h"
#include "EigenToVector.h"
#include "Gonio.h"
#include "HDF5MetaDataReader.h"
#include "Sample.h"
#include "Source.h"
#include "Units.h"

namespace nsx {

HDF5MetaDataReader::HDF5MetaDataReader(const std::string& filename, sptrDiffractometer diffractometer)
    :IDataReader(filename,diffractometer),
      _dataset(nullptr),
      _space(nullptr),
      _memspace(nullptr)
{
    H5::Group infoGroup, experimentGroup, detectorGroup, sampleGroup;

    try {
        _file = std::unique_ptr<H5::H5File>(new H5::H5File(filename.c_str(), H5F_ACC_RDONLY));
        infoGroup = _file->openGroup("/Info");
        experimentGroup = _file->openGroup("/Experiment");
        detectorGroup =_file->openGroup("/Data/Scan/Detector");
        sampleGroup =_file->openGroup("/Data/Scan/Sample");
    } catch (H5::Exception& e) {
        std::string what = e.getDetailMsg();
        throw std::runtime_error(what);
    }
    
    // Read the info group and store in metadata    
    int ninfo=infoGroup.getNumAttrs();
    for (int i=0;i<ninfo;++i) {
        H5::Attribute attr=infoGroup.openAttribute(i);
        H5::DataType typ=attr.getDataType();
        std::string value;
        attr.read(typ,value);
        _metadata.add<std::string>(attr.getName(),value);
    }

    // Read the experiment group and store all int and double attributes in metadata
    int nexps=experimentGroup.getNumAttrs();
    for (int i=0;i<nexps;++i) {
        H5::Attribute attr=experimentGroup.openAttribute(i);
        H5::DataType typ=attr.getDataType();
        if (typ==H5::PredType::NATIVE_INT32) {
            int value;
            attr.read(typ,&value);
            _metadata.add<int>(attr.getName(),value);
        }
        if (typ==H5::PredType::NATIVE_DOUBLE) {
            double value;
            attr.read(typ,&value);
            _metadata.add<double>(attr.getName(),value);
        }
    }

    _nFrames=_metadata.key<int>("npdone");

    // Getting Scan parameters for the detector
    auto axes = _diffractometer->detector()->gonio()->axes();

    Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> dm(axes.size(),_nFrames);
    for (size_t i = 0; i < axes.size(); ++i) {
        auto axis = axes[i];
        if (axis->physical()) {
            try {
                H5::DataSet dset = detectorGroup.openDataSet(axis->name());
                H5::DataSpace space(dset.getSpace());
                hsize_t dim = space.getSimpleExtentNdims();
                if (dim != 1) {
                    throw std::runtime_error("Read HDF5, problem reading detector scan parameters, dimension of array should be 1");
                }
                std::vector<hsize_t> dims(dim), maxdims(dim);
                space.getSimpleExtentDims(&dims[0], &maxdims[0]);
                if (dims[0] != _nFrames) {
                    throw std::runtime_error("Read HDF5, problem reading detector scan parameters, different array length to npdone");
                }
                dset.read(&dm(i,0),H5::PredType::NATIVE_DOUBLE,space,space);
            } catch(...) {
                throw std::runtime_error("Coud not read "+axis->name()+" HDF5 dataset");
            }
        } else {
            dm.row(i) = Eigen::VectorXd::Zero(_nFrames);
        }
    }

    // Use natural units internally (rad)
    dm*=deg;

    _detectorStates.resize(_nFrames);

    for (unsigned int i=0;i<_nFrames;++i) {
        _detectorStates[i] = eigenToVector(dm.col(i));
    }

    // Getting Scan parameters for the sample
    axes =_diffractometer->sample()->gonio()->axes();

    dm.resize(axes.size(),_nFrames);
    for (size_t i = 0; i <axes.size(); ++i) {
        auto axis = axes[i];
        if (axis->physical()) {
            try {
                H5::DataSet dset = sampleGroup.openDataSet(axis->name());
                H5::DataSpace space(dset.getSpace());
                hsize_t dim = space.getSimpleExtentNdims();
                if (dim != 1) {
                    throw std::runtime_error("Read HDF5, problem reading sample scan parameters, dimension of array should be 1");
                }
                std::vector<hsize_t> dims(dim), maxdims(dim);
                space.getSimpleExtentDims(&dims[0], &maxdims[0]);
                if (dims[0] != _nFrames) {
                    throw std::runtime_error("Read HDF5, problem reading sample scan parameters, different array length to npdone");
                }
                dset.read(&dm(i,0),H5::PredType::NATIVE_DOUBLE,space,space);
            } catch(...) {
                throw std::runtime_error("Coud not read "+axis->name()+" HDF5 dataset");
            }
        } else {
            dm.row(i) = Eigen::VectorXd::Zero(_nFrames);
        }
    }

    // Use natural units internally (rad)
    dm*=deg;

    _sampleStates.resize(_nFrames);
    for (unsigned int i=0;i<_nFrames;++i) {
        _sampleStates[i] = eigenToVector(dm.col(i));
    }

    _file->close();

}

HDF5MetaDataReader::~HDF5MetaDataReader()
{
    if (_isOpened) {
        close();
    }
}

void HDF5MetaDataReader::open()
{
    if (_isOpened) {
        return;
    }

    try {
        _file = std::unique_ptr<H5::H5File>(new H5::H5File(_metadata.key<std::string>("filename").c_str(), H5F_ACC_RDONLY));
    } catch(...) {
        if (_file) {
            _file.reset();
        }
        throw;
    }

    // handled automaticall by HDF5 blosc filter
    blosc_init();
    blosc_set_nthreads(4);

    // Register blosc filter dynamically with HDF5
    char *version, *date;
    int r = register_blosc(&version, &date);
    if (r<=0) {
        throw std::runtime_error("Problem registering BLOSC filter in HDF5 library");
    }

    // Create new data set
    try {
        _dataset = std::unique_ptr<H5::DataSet>(new H5::DataSet(_file->openDataSet("/Data/Counts")));
        // Dataspace of the dataset /counts
        _space = std::unique_ptr<H5::DataSpace>(new H5::DataSpace(_dataset->getSpace()));
    } catch(...) {
        throw;
    }
    // Get rank of data
    const hsize_t ndims=_space->getSimpleExtentNdims();
    std::vector<hsize_t> dims(ndims), maxdims(ndims);

    // Get dimensions of data
    _space->getSimpleExtentDims(&dims[0], &maxdims[0]);
    _nFrames=dims[0];
    _nRows=dims[1];
    _nCols=dims[2];

    // Size of one hyperslab
    hsize_t  count[3];
    count[0] = 1;
    count[1] = _nRows;
    count[2] = _nCols;
    _memspace = std::unique_ptr<H5::DataSpace>(new H5::DataSpace(3,count,nullptr));
    _isOpened = true;

    // reported by valgrind
    free(version);
    free(date);
}

void HDF5MetaDataReader::close()
{
    if (!_isOpened) {
        return;
    }
    _file->close();
    _space->close();
    _memspace->close();
    _dataset->close();
    _space.reset();
    _memspace.reset();
    _dataset.reset();
    _file.reset();
    _isOpened = false;
}

} // end namespace nsx
