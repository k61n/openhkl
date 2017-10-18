#include <cmath>
#include <stdexcept>
#include <utility>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>

#include "blosc_filter.h"
#include "blosc.h"

#include "H5Cpp.h"

#include "../crystal/Peak3D.h"
#include "../crystal/PeakIntegrator.h"
#include "../crystal/SpaceGroup.h"
#include "../crystal/UnitCell.h"
#include "../data/BasicFrameIterator.h"
#include "../data/DataSet.h"
#include "../data/IDataReader.h"
#include "../data/IFrameIterator.h"
#include "../data/ThreadedFrameIterator.h"
#include "../geometry/Ellipsoid.h"
#include "../geometry/IntegrationRegion.h"
#include "../geometry/AABB.h"
#include "../instrument/Detector.h"
#include "../instrument/Diffractometer.h"
#include "../instrument/Gonio.h"
#include "../instrument/Monochromator.h"
#include "../instrument/Sample.h"
#include "../instrument/Source.h"
#include "../mathematics/ErfInv.h"
#include "../mathematics/MathematicsTypes.h"
#include "../utils/ProgressHandler.h"
#include "../utils/Units.h"

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
    _peaks(),
    _fileSize(0),
    _masks(),
    _background(0.0),
    _reader(reader)
{
    if ( !boost::filesystem::exists(_filename.c_str())) {
        throw std::runtime_error("IData, file: " + _filename + " does not exist");
    }

    _nrows = _diffractometer->getDetector()->getNRows();
    _ncols = _diffractometer->getDetector()->getNCols();

    _metadata = uptrMetaData(new MetaData(_reader->getMetadata()));
    _nFrames = _metadata->getKey<int>("npdone");

    // Getting Scan parameters for the detector
    _states.resize(_nFrames);

    for (unsigned int i=0;i<_nFrames;++i) {
        _states[i] = _reader->getState(i);
    }
}

uptrIFrameIterator DataSet::getIterator(int idx)
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
    clearPeaks();
    blosc_destroy();
}

std::string DataSet::getBasename() const
{
    boost::filesystem::path pathname(_filename);
    return pathname.filename().string();
}


int DataSet::dataAt(unsigned int x, unsigned int y, unsigned int z)
{
    // Check that the voxel is inside the limit of the data
    if (z>=_nFrames || y>=_ncols || x>=_nrows) {
        return 0;
    }
    return getFrame(z)(x,y);
}

Eigen::MatrixXi DataSet::getFrame(std::size_t idx)
{
    return _reader->getData(idx);
}

void DataSet::open()
{
    _reader->open();
}

void DataSet::close()
{
    _reader->close();
}

const std::string& DataSet::getFilename() const
{
    return _filename;
}

sptrDiffractometer DataSet::getDiffractometer() const
{
    return _diffractometer;
}

MetaData*  DataSet::getMetadata() const
{
    return _metadata.get();
}

std::size_t DataSet::getNFrames() const
{
    return _nFrames;
}

std::size_t DataSet::getNCols() const
{
    return _ncols;
}

std::size_t DataSet::getNRows() const
{
    return _nrows;
}

PeakSet& DataSet::getPeaks()
{
    return _peaks;
}

void DataSet::addPeak(const sptrPeak3D& peak)
{
    _peaks.insert(peak);
    maskPeak(peak);
}

void DataSet::clearPeaks()
{
    _peaks.clear();
}

InstrumentState DataSet::getInterpolatedState(double frame) const
{
    if (frame>(_states.size()-1) || frame<0) {
        throw std::runtime_error("Error when interpolating state: invalid frame value");
    }

    const std::size_t idx = std::size_t(std::lround(std::floor(frame)));
    const std::size_t next = std::min(idx+1, _states.size()-1);
    const double t = frame-idx;

    const auto& nextState = _states[next];
    const auto& prevState = _states[idx];

    return prevState.interpolate(nextState, t);
}

const std::vector<InstrumentState>& DataSet::getInstrumentStates() const
{
    return _states;
}

bool DataSet::removePeak(const sptrPeak3D& peak)
{
    auto&& it=_peaks.find(peak);

    if (it == _peaks.end()) {
        return false;
    }
    _peaks.erase(it);
    return true;
}

bool DataSet::isOpened() const
{
    return _isOpened;
}

std::size_t DataSet::getFileSize() const
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
        IntMatrix frame(getFrame(offset[0]));
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

    for (unsigned int i = 0; i < _states.size(); ++i) {
        const std::vector<double>& v = _states[i].detector.getValues();

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

    for (unsigned int i = 0; i < _states.size(); ++i) {
        const std::vector<double>& v = _states[i].sample.getValues();

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
        std::vector<double> v = _states[i].source.getValues();

        while(v.size() < sourcenames.size()) {
            v.emplace_back(0.0);
        }

        for (unsigned int j = 0; j < sourcenames.size(); ++j) {
            valsSources(j,i) = v[j] / deg;
        }
    }

    for (unsigned int j = 0; j < sourcenames.size(); ++j) {
        H5::DataSet sourceScan(sourceGroup.createDataSet(sourcenames[j], H5::PredType::NATIVE_DOUBLE, scanSpace));
        sourceScan.write(&valsSources(j,0), H5::PredType::NATIVE_DOUBLE, scanSpace, scanSpace);
    }

    const auto& map=_metadata->getMap();

    // Write all string metadata into the "Info" group
    H5::Group infogroup(file.createGroup("/Info"));
    H5::DataSpace metaSpace(H5S_SCALAR);
    H5::StrType str80(H5::PredType::C_S1, 80);
    std::string info;

    for (const auto& item: map) {
        std::string info;

        try {
            info = item.second.cast<std::string>();
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
            value = item.second.cast<int>();
            H5::Attribute intAtt(metadatagroup.createAttribute(item.first, H5::PredType::NATIVE_INT32, metaSpace));
            intAtt.write(H5::PredType::NATIVE_INT, &value);
        } catch(...) {
            try {
                double dvalue;
                dvalue = item.second.cast<double>();
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
    maskPeaks();
}

void DataSet::removeMask(IMask* mask)
{
    auto&& p = _masks.find(mask);
    if (p != _masks.end()) {
        _masks.erase(mask);
    }
    maskPeaks();
}

const std::set<IMask*>& DataSet::getMasks()
{
    return _masks;
}

void DataSet::maskPeaks() const
{
    for (auto&& p : _peaks) {
        maskPeak(p);
    }
}

void DataSet::maskPeak(sptrPeak3D peak) const
{
    peak->setMasked(false);
    for (auto&& m : _masks) {
        // If the background of the peak intercept the mask, unselected the peak
        if (m->collide(peak->getShape())) {
            peak->setMasked(true);
            break;
        }
    }
}

PeakList DataSet::hasPeaks(const std::vector<Eigen::RowVector3d>& hkls, const Eigen::Matrix3d& BU)
{
    std::vector<Eigen::RowVector3d> qs;

    for (auto hkl: hkls) {
        qs.emplace_back(hkl*BU);
    }

    std::vector<DetectorEvent> events = getEvents(qs);
    PeakList peaks;
    auto detector = getDiffractometer()->getDetector();

    Eigen::Matrix3d BUI = BU.inverse();

    for (auto event: events) {
        Eigen::Vector3d p = event.detectorPosition();
        Eigen::RowVector3d hkl = getQ(p).transpose() * BUI;

        sptrPeak3D peak(new Peak3D);
        // this sets the center of the ellipse with a dummy value for radius
        peak->setShape(Ellipsoid(p, 1.0));
        peaks.emplace_back(peak);
    }

    return peaks;
}

std::vector<DetectorEvent> DataSet::getEvents(const std::vector<Eigen::RowVector3d>& qs) const
{
    std::vector<DetectorEvent> events;
    unsigned int scanSize = static_cast<unsigned int>(_states.size());

    auto detector = getDiffractometer()->getDetector();
    auto& mono = _diffractometer->getSource()->getSelectedMonochromator();

    const Eigen::RowVector3d ki = mono.getKi().transpose();
    std::vector<Eigen::Matrix3d> rotMatrices;
    rotMatrices.reserve(scanSize);
    auto gonio = _diffractometer->getSample()->getGonio();
    double wavelength_2 = -0.5 * mono.getWavelength();

    for (unsigned int s=0; s<scanSize; ++s) {
        // todo: do we need to transpose here??
        rotMatrices.push_back(gonio->getHomMatrix(_states[s].sample.getValues()).rotation().transpose());
    } 

    for (const Eigen::RowVector3d& q: qs) {
        bool sign = (q*rotMatrices[0] + ki).squaredNorm() > ki.squaredNorm();

        for (int i = 1; i < scanSize; ++i) {
            const Eigen::RowVector3d kf = q*rotMatrices[i] + ki;
            const bool new_sign = kf.squaredNorm() > ki.squaredNorm();

            if (sign != new_sign) {
                sign = new_sign;

                const Eigen::RowVector3d kf0 = q*rotMatrices[i-1] + ki;
                const Eigen::RowVector3d kf1 = q*rotMatrices[i] + ki;
                //const Eigen::RowVector3d dkf = kf1-kf0;
                const Eigen::RowVector3d dkf = q*(rotMatrices[i]-rotMatrices[i-1]);
        
                const double a = dkf.squaredNorm();
                const double b = 2 * kf0.dot(dkf);
                const double c = kf0.squaredNorm() - ki.squaredNorm();
                const double discr = b*b - 4*a*c;
        
                double t = 0.5;
                const int max_count = 100;
                Eigen::RowVector3d kf;
                
                for (int c = 0; c < max_count; ++c) {
                    kf = (1-t)*kf0 + t*kf1;
                    const double f = kf.squaredNorm() - ki.squaredNorm();
                    
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
                const InstrumentState& state = getInterpolatedState(t);
        
                //const ComponentState& dis = state.detector;
                double px,py;
                // If hit detector, new peak
                //const ComponentState& cs=state.sample;
                Eigen::Vector3d from=_diffractometer->getSample()->getPosition(state.sample.getValues());
        
                double time;
                bool accept=_diffractometer->getDetector()->receiveKf(px,py,kf,from,time,state.detector.getValues());
        
                if (accept) {
                    events.emplace_back(detector.get(), px, py, t, state.detector.getValues());
                }
            }
        }        
    }
    return events;
}

double DataSet::getBackgroundLevel(const sptrProgressHandler& progress)
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

    for (auto it = getIterator(0); it->index() != _nFrames; it->advance()) {
        // cast matrix to double (instead of int) -- necessary due to integer overflow!
        // _background += factor * it->cast<double>().sum();
        bg += factor * it->getFrame().sum();

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

void DataSet::integratePeaks(const PeakSet& peaks, double peak_scale, double bkg_scale, bool update_shape, const sptrProgressHandler& handler)
{
    if (handler) {
        handler->setStatus(("Integrating " + std::to_string(getPeaks().size()) + " peaks...").c_str());
        handler->setProgress(0);
    }

    using IntegrationRegion = IntegrationRegion;
    using PeakIntegrator = PeakIntegrator;
    using integrated_peak = std::pair<sptrPeak3D, PeakIntegrator>;

    std::vector<integrated_peak> peak_list;

    const size_t num_peaks = peaks.size();

    peak_list.reserve(num_peaks);


    auto peakRadius = [](const Eigen::Matrix3d& shape) -> double {
        return std::pow(shape.determinant(), -1.0/6.0);
    };


//    // testing: get average peak shape
    Eigen::Matrix3d avg_peak_shape = Eigen::Matrix3d::Zero();
    unsigned int num_good_peaks = 0;
    double avg_peak_radius = 0.0;
    double peak_radius_std = 0.0;


    for(auto&& p: peaks) {
        if (p->isMasked() || !p->isSelected()) {
            continue;

        }
        double radius = peakRadius(p->getShape().metric());
        avg_peak_shape += p->getShape().metric();
        avg_peak_radius += radius;
        peak_radius_std += radius*radius;
        ++num_good_peaks;
    }

    // too few neighbors to get average shape
    if (num_good_peaks < 1) {
        return;
    }

    avg_peak_shape /= num_good_peaks;
    avg_peak_radius /= num_good_peaks;

    const double var = (peak_radius_std - avg_peak_radius*avg_peak_radius) / (num_good_peaks-1);
    peak_radius_std = std::sqrt(var);

    std::cout << "avg radius: " << avg_peak_radius << std::endl;
    std::cout << "std. dev:   " << peak_radius_std << std::endl;

    for (auto&& peak: peaks ) {
        IntegrationRegion region(peak->getShape(), peak_scale, bkg_scale);
        PeakIntegrator integrator(region, *this);
        peak_list.emplace_back(peak, integrator);
    }

    size_t idx = 0;
    int num_frames_done = 0;

    #pragma omp parallel for
    for ( idx = 0; idx < getNFrames(); ++idx ) {
        Eigen::MatrixXi frame, mask;
        #pragma omp critical
        frame = getFrame(idx);

        mask.resize(getNRows(), getNCols());
        mask.setZero();

        for (auto& tup: peak_list ) {
            auto&& integrator = tup.second;
            integrator.getRegion().updateMask(mask, idx);
        }

        for (auto& tup: peak_list ) {
            auto&& peak = tup.first;
            auto&& integrator = tup.second;
            integrator.step(frame, idx, mask);
        }

        if (handler) {
            #pragma omp atomic
            ++num_frames_done;
            double progress = num_frames_done * 100.0 / getNFrames();
            handler->setProgress(progress);
        }
    }

    // testing: don't update shape?!
    // update_shape = false;
    const double confidence = getConfidence(1.0); // todo: should not be hard coded

    for (auto&& tup: peak_list) {
        auto&& peak = tup.first;
        auto&& integrator = tup.second;
        integrator.end();
        peak->updateIntegration(integrator);

        if (!update_shape) {
            continue;
        }

        // update the peak shape
        auto&& maybe_shape = integrator.getBlobShape(confidence);

        // could not get shape (peak too weak?)
        if (maybe_shape.isNothing()) {
            peak->setSelected(false);
            continue;
        }

        auto&& new_shape = maybe_shape.get();
        auto&& old_shape = peak->getShape();
        Eigen::RowVector3d hkl_old, hkl_new;

        const double radius = peakRadius(new_shape.metric());
        const double volume = 4.0*M_PI/3.0 * radius*radius*radius;

        if (volume < 1.0) {
            peak->setSelected(false);
            continue;
        }

        if (std::fabs(radius-avg_peak_radius) > 3.5*peak_radius_std) {
            peak->setSelected(false);
            continue;
        }

        auto aabb = new_shape.aabb();
        auto lb = aabb.lower();
        auto ub = aabb.upper();

        // not enough mass to determine ellipse
        if (std::isnan((ub-lb).squaredNorm())) {
            peak->setSelected(false);
            continue;
        }

        // outside of frame
        if (lb[0] < 0.0 || lb[1] < 0.0 || lb[2] < 0.0) {
            peak->setSelected(false);
            continue;
        }
        if (ub[0] > _ncols-1 || ub[1] > _nrows-1 || ub[2] > _nFrames-1) {
            peak->setSelected(false);
            continue;
        }

        peak->getMillerIndices(hkl_old);
        peak->setShape(new_shape);
        peak->getMillerIndices(hkl_new);

        // indices disagree
        if ( (hkl_old-hkl_new).squaredNorm() > 1e-6 ) {
            peak->setShape(old_shape);
            peak->setSelected(false);
        }
    }
}

void DataSet::removeDuplicatePeaks()
{
    class compare_fn {
    public:
        auto operator()(const Eigen::RowVector3i& a, const Eigen::RowVector3i& b) const -> bool
        {
            if (a(0) != b(0))
                return a(0) < b(0);

            if (a(1) != b(1))
                return a(1) < b(1);

            return a(2) < b(2);
        }
    };

    auto sample = getDiffractometer()->getSample();
    unsigned int ncrystals = static_cast<unsigned int>(sample->getNCrystals());

    for (unsigned int i = 0; i < ncrystals; ++i) {
        auto cell = sample->getUnitCell(i);

        std::map<Eigen::RowVector3i, sptrPeak3D, compare_fn> hkls;

        for (auto&& peak: _peaks) {
            Eigen::RowVector3d hkl;
            Eigen::RowVector3i hkl_int;

            if (!peak->getMillerIndices(*cell, hkl)) {
                continue;
            }

            for (auto i = 0; i < 3; ++i) {
                hkl_int(i) = std::lround(hkl(i));
            }

            auto it = hkls.find(hkl_int);

            if (it == hkls.end()) {
                hkls[hkl_int] = peak;
            }
            else {
                it->second->setSelected(false);
                peak->setSelected(false);
            }
        }
    }
}

double DataSet::getSampleStepSize() const
{
    // TODO(jonathan): we should NOT assume that gonio axis 0 is the one being rotated
    // when we compute 'step' below
    double step = 0.0;

    size_t numFrames = getNFrames();
    const auto& ss = getInstrumentStates();
    size_t numValues = ss[0].sample.getValues().size();

    for (size_t i = 0; i < numValues; ++i) {
        double dx = ss[numFrames-1].sample.getValues()[i] - ss[0].sample.getValues()[i];
        step += dx*dx;
    }

    step = std::sqrt(step);
    step /= (numFrames-1) * 0.05 * deg;

    return step;
}

Eigen::Vector3d DataSet::getQ(const Eigen::Vector3d& pix) const
{
    auto source = _diffractometer->getSource();
    auto sample = _diffractometer->getSample();

    double frame = pix[2];

    if (frame > getNFrames()-1) {
        frame = getNFrames()-1;
    }
    if (frame < 0) {
        frame = 0.0;
    }

    double wavelength = source->getSelectedMonochromator().getWavelength();
    auto state = getInterpolatedState(frame);

    DetectorEvent event(_diffractometer->getDetector().get(), pix[0], pix[1], frame, state.detector.getValues());

    // otherwise scattering point is deducted from the sample
    Eigen::Vector3d q = event.getQ(wavelength, state.sample.getPosition());
    return state.sample.transformQ(q);
}

} // end namespace nsx
