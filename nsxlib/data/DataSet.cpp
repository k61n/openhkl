#include <cmath>
#include <stdexcept>
#include <utility>
#include <vector>

#include "blosc.h"

#include "H5Cpp.h"

#include "AABB.h"
#include "BasicFrameIterator.h"
#include "BloscFilter.h"
#include "CrystalTypes.h"
#include "ConvolutionKernel.h"
#include "Convolver.h"
#include "DataSet.h"
#include "Detector.h"
#include "DetectorEvent.h"
#include "Diffractometer.h"
#include "Ellipsoid.h"
#include "ErfInv.h"
#include "Gonio.h"
#include "IDataReader.h"
#include "IFrameIterator.h"
#include "IntegrationRegion.h"
#include "MathematicsTypes.h"
#include "Monochromator.h"
#include "Path.h"
#include "Peak3D.h"
#include "PeakFilter.h"
#include "PeakIntegrator.h"
#include "ProgressHandler.h"
#include "ReciprocalVector.h"
#include "Sample.h"
#include "Source.h"
#include "SpaceGroup.h"
#include "ThreadedFrameIterator.h"
#include "UnitCell.h"
#include "Units.h"

namespace nsx {

DataSet::DataSet(std::shared_ptr<IDataReader> reader, const sptrDiffractometer& diffractometer):
    _isOpened(false),
    _filename(reader->getFilename()),
    _nFrames(0),
    _nrows(0),
    _ncols(0),
    _diffractometer(diffractometer),
    _metadata(uptrMetaData(new MetaData())),
    _data(),
    _states(),
    _fileSize(0),
    _masks(),
    _background(0.0),
    _reader(reader)
{
    if (!fileExists(_filename)) {
        throw std::runtime_error("IData, file: " + _filename + " does not exist");
    }

    _nrows = _diffractometer->getDetector()->getNRows();
    _ncols = _diffractometer->getDetector()->getNCols();

    _metadata = uptrMetaData(new MetaData(_reader->getMetadata()));
    _nFrames = _metadata->getKey<int>("npdone");

    double wav = _metadata->getKey<double>("wavelength");
    _diffractometer->getSource()->getSelectedMonochromator().setWavelength(wav);

    // Getting Scan parameters for the detector
    _states.resize(_nFrames);

    for (unsigned int i=0;i<_nFrames;++i) {
        _states[i] = _reader->getState(i);
    }
}

uptrIFrameIterator DataSet::iterator(int idx)
{
    // use default frame iterator if one hasn't been set
    if ( !_iteratorCallback) {
        _iteratorCallback = [] (DataSet& data, int index) {
            return new BasicFrameIterator(data, static_cast<unsigned int>(index));
            //return new ThreadedFrameIterator(data, index);
        };
    }
    return uptrIFrameIterator(_iteratorCallback(*this, idx));
}

void DataSet::setIteratorCallback(FrameIteratorCallback callback)
{
    _iteratorCallback = std::move(callback);
}

DataSet::~DataSet()
{
    blosc_destroy();
}

int DataSet::dataAt(unsigned int x, unsigned int y, unsigned int z)
{
    // Check that the voxel is inside the limit of the data
    if (z>=_nFrames || y>=_ncols || x>=_nrows) {
        return 0;
    }
    return frame(z)(x,y);
}

Eigen::MatrixXi DataSet::frame(std::size_t idx)
{
    return _reader->getData(idx);
}

Eigen::MatrixXi DataSet::convolvedFrame(std::size_t idx, sptrConvolutionKernel kernel)
{
    Eigen::MatrixXi autoscaled_data;

    Eigen::MatrixXi frame_data = _reader->getData(idx);

    int nrows = int(frame_data.rows());
    int ncols = int(frame_data.cols());

    int maxData = frame_data.maxCoeff();

    nsx::RealMatrix result;

    if (kernel) {

        // set up convolver
        Convolver convolver;
        convolver.setKernel(kernel->matrix());

        // compute the convolution
        result = convolver.apply(frame_data.cast<double>());
    }
    else {
        result = frame_data.cast<double>();
    }

    double minVal = result.minCoeff();
    double maxVal = result.maxCoeff();
    result.array() -= minVal;
    result.array() *= static_cast<double>(maxData)/(maxVal-minVal);
    autoscaled_data = result.cast<int>();

    return autoscaled_data;
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

sptrDiffractometer DataSet::diffractometer() const
{
    return _diffractometer;
}

MetaData*  DataSet::metadata() const
{
    return _metadata.get();
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

InstrumentState DataSet::interpolatedState(double frame) const
{
    if (frame>(_states.size()-1) || frame<0) {
        throw std::runtime_error("Error when interpolating state: invalid frame value: " + std::to_string(frame));
    }

    const std::size_t idx = std::size_t(std::lround(std::floor(frame)));
    const std::size_t next = std::min(idx+1, _states.size()-1);
    const double t = frame-idx;

    const auto& nextState = _states[next];
    const auto& prevState = _states[idx];

    return prevState.interpolate(nextState, t);
}

const std::vector<InstrumentState>& DataSet::instrumentStates() const
{
    return _states;
}

std::vector<InstrumentState>& DataSet::instrumentStates()
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

void DataSet::saveHDF5(const std::string& filename) //const
{
    blosc_init();
    blosc_set_nthreads(4);

    hsize_t dims[3] = {_nFrames, _nrows,_ncols};
    hsize_t chunk[3] = {1, _nrows,_ncols};
    hsize_t count[3] = {1, _nrows,_ncols};

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
        IntMatrix current_frame(frame(offset[0]));
        dset.write(current_frame.data(), H5::PredType::NATIVE_INT32, memspace, space);
    }

    // Saving the scans parameters (detector, sample and source)
    H5::Group scanGroup(dataGroup.createGroup("Scan"));

    // Write detector states
    H5::Group detectorGroup(scanGroup.createGroup("Detector"));

    std::vector<std::string> names=_diffractometer->getDetector()->getGonio()->getPhysicalAxesNames();
    hsize_t nf[1]={_nFrames};
    H5::DataSpace scanSpace(1,nf);
    RealMatrix vals(names.size(),_nFrames);

    const auto& detectorStates = _reader->detectorStates();

    for (unsigned int i = 0; i < detectorStates.size(); ++i) {
        auto&& v = detectorStates[i].values();

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

    const auto& sampleStates = _reader->sampleStates();

    for (unsigned int i = 0; i < sampleStates.size(); ++i) {
        auto&& v = sampleStates[i].values();

        for (unsigned int j = 0; j < samplenames.size(); ++j) {
            valsSamples(j,i) = v[j]/deg;
        }
    }

    for (unsigned int j = 0; j < samplenames.size(); ++j) {
        H5::DataSet sampleScan(sampleGroup.createDataSet(samplenames[j], H5::PredType::NATIVE_DOUBLE, scanSpace));
        sampleScan.write(&valsSamples(j,0), H5::PredType::NATIVE_DOUBLE, scanSpace, scanSpace);
    }

    #if 0
    // Write source states
    H5::Group sourceGroup(scanGroup.createGroup("Source"));
    std::vector<std::string> sourcenames = _diffractometer->getSource()->getGonio()->getPhysicalAxesNames();
    RealMatrix valsSources(sourcenames.size(),_nFrames);

    for (unsigned int i = 0; i < _states.size(); ++i) {
        auto v = _states[i].source.values();

        // pad with zeros if necessary
        if (v.size() < sourcenames.size()) {
            Eigen::ArrayXd w(sourcenames.size());
            w.setZero();
            for (auto i = 0; i < v.size(); ++i) {
                w(i) = v(i);
            }
            v = w;
        }

        for (unsigned int j = 0; j < sourcenames.size(); ++j) {
            valsSources(j,i) = v[j] / deg;
        }
    }

    for (unsigned int j = 0; j < sourcenames.size(); ++j) {
        H5::DataSet sourceScan(sourceGroup.createDataSet(sourcenames[j], H5::PredType::NATIVE_DOUBLE, scanSpace));
        sourceScan.write(&valsSources(j,0), H5::PredType::NATIVE_DOUBLE, scanSpace, scanSpace);
    }
    #endif

    const auto& map=_metadata->getMap();

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

    for (const auto& item: map)	{
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
                // shouldn't there be some error handling here?
            }
        }
    }
    file.close();
    // blosc_destroy();
}

void DataSet::addMask(IMask* mask)
{
    _masks.insert(mask);
}

void DataSet::removeMask(IMask* mask)
{
    auto&& p = _masks.find(mask);
    if (p != _masks.end()) {
        _masks.erase(mask);
    }
}

const std::set<IMask*>& DataSet::masks()
{
    return _masks;
}

void DataSet::maskPeaks(PeakList& peaks) const
{
    for (auto peak: peaks) {
        // peak belongs to another dataset
        if (peak->data().get() != this) {
            continue;
        }

        peak->setMasked(false);
        for (auto&& m : _masks) {
            // If the background of the peak intercept the mask, unselected the peak
            if (m->collide(peak->getShape())) {
                peak->setMasked(true);
                break;
            }
        }
    }
}

double DataSet::backgroundLevel(const sptrProgressHandler& progress)
{
    if ( _background > 0.0 ) {
        return _background;
    }

    // we calculate background in local variable bg for thread safety reasons--
    // this method is called from a thread which could be aborted, so we do not want
    // to write to _background until the calculation has been completed
    double bg = 0.0;
    double factor = 1.0 / (_nFrames * _nrows * _ncols);

    if ( progress) {
        progress->setStatus("Computing background level...");
        progress->setProgress(0);
    }

    for (auto it = iterator(0); it->index() != _nFrames; it->advance()) {
        // cast matrix to double (instead of int) -- necessary due to integer overflow!
        // _background += factor * it->cast<double>().sum();
        bg += factor * it->frame().sum();

        if (progress) {
            double done = 100.0 * it->index() / static_cast<double>(_nFrames);
            progress->setProgress(int(done));
        }
    }

    if ( progress ) {
        progress->setProgress(100);
    }

    _background = bg;
    return _background;
}

void DataSet::integratePeaks(const PeakList& peaks, double bkg_begin, double bkg_end, const sptrProgressHandler& handler)
{
    using IntegrationRegion = IntegrationRegion;
    using PeakIntegrator = PeakIntegrator;
    using integrated_peak = std::pair<sptrPeak3D, PeakIntegrator>;

    std::vector<integrated_peak> peak_list;
    const size_t num_peaks = peaks.size();
    peak_list.reserve(num_peaks);

    for (auto&& peak: peaks ) {
        // skip if peak does not belong to this dataset
        if (peak->data().get() != this) {
            continue;
        }
        // todo: n slices probably should not be hard-coded
        IntegrationRegion region(peak->getShape(), bkg_begin, bkg_end, int(bkg_begin*5));
        PeakIntegrator integrator(region, *this);
        peak_list.emplace_back(peak, integrator);
    }

    if (handler) {
        handler->setStatus(("Integrating " + std::to_string(peak_list.size()) + " peaks...").c_str());
        handler->setProgress(0);
    }

    size_t idx = 0;
    int num_frames_done = 0;

    #pragma omp parallel for
    for ( idx = 0; idx < nFrames(); ++idx ) {
        Eigen::MatrixXi current_frame, mask;
        #pragma omp critical
        current_frame = frame(idx);

        mask.resize(nRows(),nCols());
        mask.setConstant(-1);

        for (auto& tup: peak_list ) {
            auto&& integrator = tup.second;
            integrator.getRegion().updateMask(mask, idx);
        }

        for (auto& tup: peak_list ) {
            auto&& integrator = tup.second;
            integrator.step(current_frame, idx, mask);
        }

        if (handler) {
            #pragma omp atomic
            ++num_frames_done;
            double progress = num_frames_done * 100.0 / nFrames();
            handler->setProgress(progress);
        }
    }

    for (auto&& tup: peak_list) {
        auto&& peak = tup.first;
        auto&& integrator = tup.second;
        integrator.end();
        peak->updateIntegration(integrator);      
    }
}

#if 0
double DataSet::getSampleStepSize() const
{
    // TODO(jonathan): we should NOT assume that gonio axis 0 is the one being rotated
    // when we compute 'step' below
    double step = 0.0;

    size_t numFrames = nFrames();
    const auto& ss = instrumentStates();
    size_t numValues = ss[0].sample.values().size();

    for (size_t i = 0; i < numValues; ++i) {
        double dx = ss[numFrames-1].sample.values()[i] - ss[0].sample.values()[i];
        step += dx*dx;
    }

    step = std::sqrt(step);
    step /= (numFrames-1) * 0.05 * deg;

    return step;
}
#endif

std::vector<DetectorEvent> DataSet::getEvents(const std::vector<ReciprocalVector>& sample_qs) const
{
    std::vector<DetectorEvent> events;
    unsigned int scanSize = nFrames();

    std::vector<Eigen::RowVector3d> ki;
    ki.reserve(scanSize);

    std::vector<Eigen::Matrix3d> sample_to_lab;
    sample_to_lab.reserve(scanSize);
    
    for (unsigned int s = 0; s < scanSize; ++s) {
        auto state = interpolatedState(s);
        sample_to_lab.push_back(state.sampleOrientation().transpose());
        ki.push_back(state.ki().rowVector());
    } 

    for (const ReciprocalVector& sample_q : sample_qs) {
        const Eigen::RowVector3d& q_vect = sample_q.rowVector();

        bool sign = (q_vect*sample_to_lab[0] + ki[0]).squaredNorm() > ki[0].squaredNorm();

        for (size_t i = 1; i < scanSize; ++i) {
            const Eigen::RowVector3d kf = q_vect*sample_to_lab[i] + ki[i];
            const bool new_sign = kf.squaredNorm() > ki[i].squaredNorm();

            if (sign != new_sign) {
                sign = new_sign;

                const Eigen::RowVector3d kf0 = q_vect*sample_to_lab[i-1] + ki[i-1];
                const Eigen::RowVector3d kf1 = q_vect*sample_to_lab[i] + ki[i];
                const Eigen::RowVector3d dkf = q_vect*(sample_to_lab[i]-sample_to_lab[i-1]);
        
                double t = 0.5;
                const int max_count = 100;
                Eigen::RowVector3d kf;
                int c;
                
                for (c = 0; c < max_count; ++c) {
                    kf = (1-t)*kf0 + t*kf1;
                    auto ki_interp = (1-t)*ki[i-1] + t*ki[i];
                    const double f = kf.squaredNorm() - ki_interp.squaredNorm();
                    
                    if (std::fabs(f) < 1e-10) {
                        break;
                    }
                    const double df = 2*dkf.dot(kf);
                    t -= f/df;
                }
        
                if (c == max_count || t < 0.0 || t > 1.0) {
                    continue;
                }
        
                t += i-1;
                const InstrumentState& state = interpolatedState(t);
                auto detector = _diffractometer->getDetector();
                auto event = detector->constructEvent(DirectVector(state.samplePosition), ReciprocalVector((kf*state.detectorOrientation)));
                bool accept = event._tof > 0;

                if (accept) {
                    event._frame = t;
                    events.emplace_back(event);
                }
            }
        }        
    }
    return events;
}

} // end namespace nsx
