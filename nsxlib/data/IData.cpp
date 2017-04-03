#include <utility>
#include <stdexcept>
#include <memory>
#include <vector>
#include <cmath>

#include "../utils/Units.h"
#include "../utils/erf_inv.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>

#include "IData.h"
#include "IDataReader.h"

#include "../instrument/Detector.h"
#include "../instrument/Gonio.h"
#include "../instrument/Monochromator.h"
#include "../instrument/Sample.h"
#include "../instrument/Source.h"

#include "H5Cpp.h"
#include "blosc_filter.h"
#include "blosc.h"

#include "../crystal/PeakIntegrator.h"
#include "../geometry/Ellipsoid.h"
#include "../geometry/IntegrationRegion.h"

#include "IFrameIterator.h"
#include "BasicFrameIterator.h"
#include "ThreadedFrameIterator.h"


namespace SX {
namespace Data {

using Eigen::Matrix3d;
using boost::filesystem::path;
using SX::Instrument::InstrumentState;

DataSet::DataSet(IDataReader* reader, const std::shared_ptr<Diffractometer>& diffractometer):
    _isOpened(false),
    _filename(reader->getFilename()),
    _nFrames(0),
    _nrows(0),
    _ncols(0),
    _diffractometer(diffractometer),
    _metadata(std::unique_ptr<MetaData>(new MetaData())),
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

    _metadata = std::unique_ptr<MetaData>(new MetaData(_reader->getMetadata()));
    _nFrames = _metadata->getKey<int>("npdone");

    // Getting Scan parameters for the detector
    _states.resize(_nFrames);

    for (unsigned int i=0;i<_nFrames;++i) {
        _states[i] = _reader->getState(i);
    }
}

std::unique_ptr<IFrameIterator> DataSet::getIterator(int idx)
{
    // use default frame iterator if one hasn't been set
    if ( !_iteratorCallback) {
        _iteratorCallback = [] (DataSet& data, int index) {
            return new BasicFrameIterator(data, static_cast<unsigned int>(index));
            //return new ThreadedFrameIterator(data, index);
        };
    }
    return std::unique_ptr<IFrameIterator>(_iteratorCallback(*this, idx));
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
    path pathname(_filename);
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

std::shared_ptr<Diffractometer> DataSet::getDiffractometer() const
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

std::set<sptrPeak3D>& DataSet::getPeaks()
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
//    for (auto&& ptr : _peaks) {
//        ptr->unlinkData();
//    }
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

const ComponentState& DataSet::getDetectorState(size_t frame) const
{
    if (frame > (_states.size()-1)) {
        throw std::runtime_error("Error when returning detector state: invalid frame value");
    }
    return _states[frame].detector;
}

const ComponentState& DataSet::getSampleState(size_t frame) const
{
    if (frame > (_states.size()-1)) {
        throw std::runtime_error("Error when returning sample state: invalid frame value");
    }
    return _states[frame].sample;
}

const ComponentState& DataSet::getSourceState(size_t frame) const
{
    if (frame>(_states.size()-1)) {
        throw std::runtime_error("Error when returning source state: invalid frame value");
    }
    return _states[frame].source;
}


const std::vector<SX::Instrument::InstrumentState>& DataSet::getInstrumentStates() const
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
        RowMatrixi frame(getFrame(offset[0]));
        dset.write(frame.data(), H5::PredType::NATIVE_INT32, memspace, space);
    }

    // Saving the scans parameters (detector, sample and source)
    H5::Group scanGroup(dataGroup.createGroup("Scan"));

    // Write detector states
    H5::Group detectorGroup(scanGroup.createGroup("Detector"));

    std::vector<std::string> names=_diffractometer->getDetector()->getGonio()->getPhysicalAxesNames();
    hsize_t nf[1]={_nFrames};
    H5::DataSpace scanSpace(1,nf);
    RowMatrixd vals(names.size(),_nFrames);

    for (unsigned int i = 0; i < _states.size(); ++i) {
        const std::vector<double>& v = _states[i].detector.getValues();

        for (unsigned int j = 0; j < names.size(); ++j) {
            vals(j,i) = v[j] / SX::Units::deg;
        }
    }

    for (unsigned int j = 0; j < names.size(); ++j) {
        H5::DataSet detectorScan(detectorGroup.createDataSet(names[j], H5::PredType::NATIVE_DOUBLE, scanSpace));
        detectorScan.write(&vals(j,0), H5::PredType::NATIVE_DOUBLE, scanSpace, scanSpace);
    }

    // Write sample states
    H5::Group sampleGroup(scanGroup.createGroup("Sample"));
    std::vector<std::string> samplenames=_diffractometer->getSample()->getGonio()->getPhysicalAxesNames();
    RowMatrixd valsSamples(samplenames.size(), _nFrames);

    for (unsigned int i = 0; i < _states.size(); ++i) {
        const std::vector<double>& v = _states[i].sample.getValues();

        for (unsigned int j = 0; j < samplenames.size(); ++j) {
            valsSamples(j,i) = v[j]/SX::Units::deg;
        }
    }

    for (unsigned int j = 0; j < samplenames.size(); ++j) {
        H5::DataSet sampleScan(sampleGroup.createDataSet(samplenames[j], H5::PredType::NATIVE_DOUBLE, scanSpace));
        sampleScan.write(&valsSamples(j,0), H5::PredType::NATIVE_DOUBLE, scanSpace, scanSpace);
    }

    // Write source states
    H5::Group sourceGroup(scanGroup.createGroup("Source"));
    std::vector<std::string> sourcenames = _diffractometer->getSource()->getGonio()->getPhysicalAxesNames();
    RowMatrixd valsSources(sourcenames.size(),_nFrames);

    for (unsigned int i = 0; i < _states.size(); ++i) {
        std::vector<double> v = _states[i].source.getValues();

        while(v.size() < sourcenames.size()) {
            v.emplace_back(0.0);
        }

        for (unsigned int j = 0; j < sourcenames.size(); ++j) {
            valsSources(j,i) = v[j] / SX::Units::deg;
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
            info = boost::any_cast<std::string>(item.second);
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
            value = boost::any_cast<int>(item.second);
            H5::Attribute intAtt(metadatagroup.createAttribute(item.first, H5::PredType::NATIVE_INT32, metaSpace));
            intAtt.write(H5::PredType::NATIVE_INT, &value);
        } catch(...) {
            try {
                double dvalue;
                dvalue = boost::any_cast<double>(item.second);
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

void DataSet::addMask(AABB<double,3>* mask)
{
    _masks.insert(mask);
    maskPeaks();
}

void DataSet::removeMask(AABB<double,3>* mask)
{
    auto&& p = _masks.find(mask);
    if (p != _masks.end()) {
        _masks.erase(mask);
    }
    maskPeaks();
}

const std::set<AABB<double, 3> *>& DataSet::getMasks()
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
        if (m->intercept(peak->getShape())) {
            peak->setMasked(true);
            break;
        }
    }
}

bool DataSet::inMasked(const Eigen::Vector3d& point) const
{
    // Loop over the defined masks and return true if one of them contains the point
    for (auto&& m : _masks) {
        if (m->isInsideAABB(point)) {
            return true;
        }
    }
    // No mask contains the point, return false
    return false;
}

std::vector<PeakCalc> DataSet::hasPeaks(const std::vector<Eigen::Vector3d>& hkls, const Matrix3d& BU)
{
    std::vector<PeakCalc> peaks;
    unsigned int scanSize = static_cast<unsigned int>(_states.size());
    Eigen::Matrix3d UB = BU.transpose();
    auto& mono = _diffractometer->getSource()->getSelectedMonochromator();
    Eigen::Vector3d ki=mono.getKi();
    std::vector<Eigen::Matrix3d> rotMatrices;
    rotMatrices.reserve(scanSize);
    auto gonio = _diffractometer->getSample()->getGonio();
    double wavelength_2 = -0.5 * mono.getWavelength();

    for (unsigned int s=0; s<scanSize; ++s) {
        rotMatrices.push_back(gonio->getHomMatrix(_states[s].sample.getValues()).rotation());
    }

    for (const Eigen::Vector3d& hkl: hkls) {
        // Get q at rest
        Eigen::Vector3d q=UB*hkl;

        double normQ2=q.squaredNorm();
        // y component of q when in Bragg condition y=-sin(theta)*||Q||
        double qy=normQ2*wavelength_2;

        Eigen::Vector3d qi0=rotMatrices[0]*q;
        Eigen::Vector3d qi;

        bool sign = (qi0[1] > qy);
        bool found = false;
        unsigned int i;

        for (i = 1; i < scanSize; ++i) {
            qi=rotMatrices[i]*q;
            bool sign2=(qi[1] > qy);
            if (sign != sign2) {
                found = true;
                break;
            }
            qi0 = qi;
        }

        if (!found) {
            continue;
        }
        double t = (qy-qi0[1]) / (qi[1]-qi0[1]);
        Eigen::Vector3d kf=ki+qi0+(qi-qi0)*t;
        t+=(i-1);

        const InstrumentState& state = getInterpolatedState(t);

        //const ComponentState& dis = state.detector;
        double px,py;
        // If hit detector, new peak
        //const ComponentState& cs=state.sample;
        Eigen::Vector3d from=_diffractometer->getSample()->getPosition(state.sample.getValues());

        double time;
        bool accept=_diffractometer->getDetector()->receiveKf(px,py,kf,from,time,state.detector.getValues());

        if (accept) {
            //peaks.emplace_back(PeakCalc(hkl[0],hkl[1],hkl[2],px,py,t));
            peaks.emplace_back(hkl[0],hkl[1],hkl[2],px,py,t);
        }
    }
    return peaks;
}

double DataSet::getBackgroundLevel(const std::shared_ptr<SX::Utils::ProgressHandler>& progress)
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

void DataSet::integratePeaks(double peak_scale, double bkg_scale, bool update_shape, const std::shared_ptr<Utils::ProgressHandler>& handler)
{
    using Ellipsoid3D = SX::Geometry::Ellipsoid<double, 3>;

    if (handler) {
        handler->setStatus(("Integrating " + std::to_string(getPeaks().size()) + " peaks...").c_str());
        handler->setProgress(0);
    }

    using IntegrationRegion = SX::Geometry::IntegrationRegion;
    using PeakIntegrator = SX::Crystal::PeakIntegrator;
    using integrated_peak = std::pair<sptrPeak3D, PeakIntegrator>;

    std::vector<integrated_peak> peak_list;

    const size_t num_peaks = _peaks.size();

    peak_list.reserve(num_peaks);

    auto getCovar = [](const Ellipsoid3D& ell) -> Eigen::Matrix3d {
        auto&& rs = ell.getRSinv();
        return rs.transpose()*rs;
    };

//    // testing: get average peak shape
    Eigen::Matrix3d avg_peak_shape = Eigen::Matrix3d::Zero();
    unsigned int num_good_peaks = 0;

    for(auto&& p: _peaks) {
        if (p->isMasked() || !p->isSelected()) {
            continue;

        }
        avg_peak_shape += getCovar(p->getShape());
        ++num_good_peaks;
    }

    // too few neighbors to get average shape
    if (num_good_peaks < 1) {
        return;
    }

    avg_peak_shape /= num_good_peaks;

    auto peakRadius = [](const Eigen::Matrix3d& shape) -> double {
//        Eigen::SelfAdjointEigenSolver<Matrix3d> solver;
//        solver.compute(shape);
//        auto vals = solver.eigenvalues();
//        double vol = vals(0)*vals(1)*vals(2);
        static const double factor = std::pow(4.0 * M_PI / 3.0, -2.0);
        const double volume = factor * std::pow(shape.determinant(), -0.5);
        return std::pow(volume, 1.0/3.0);
    };

    const double avg_peak_radius = peakRadius(avg_peak_shape);

    for (auto&& peak: _peaks ) {
//        Eigen::Vector3d center(peak->getShape().getCenter());
//        auto shape = Ellipsoid3D(center, vals, solver.eigenvectors());
//        peak->setShape(shape);
        IntegrationRegion region(peak->getShape(), peak_scale, bkg_scale);
        PeakIntegrator integrator(region, *this);
        peak_list.emplace_back(peak, integrator);
    }

    //progressDialog->setValue(0);
    //progressDialog->setLabelText("Integrating peak intensities...");

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
            auto&& peak = tup.first;
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
    const double confidence = SX::Utils::getConfidence(1.0); // todo: should not be hard coded

    for (auto&& tup: peak_list) {
        auto&& peak = tup.first;
        auto&& integrator = tup.second;
        integrator.end();
        peak->updateIntegration(integrator);

        // peak is too weak
        // todo: p value should probably not be hard-coded
        if (integrator.pValue() > 1e-6) {
            peak->setSelected(false);
            continue;
        }

        // peak profile couldn't be fitted
//        if (!peak->getProfile().goodFit(integrator.getProjectionPeak(), 0.10)) {
//            peak->setSelected(false);
//            continue;
//        }

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

        const double radius = peakRadius(getCovar(new_shape));

        if (std::fabs(radius-avg_peak_radius) > 1.5*avg_peak_radius
                || std::fabs(radius-avg_peak_radius) < 0.5*avg_peak_radius) {
            peak->setSelected(false);
            continue;
        }

        auto old_center = old_shape.getAABBCenter();
        auto new_center = new_shape.getAABBCenter();

        auto lb = new_shape.getLower();
        auto ub = new_shape.getUpper();

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
    step /= (numFrames-1) * 0.05 * SX::Units::deg;

    return step;
}

void SX::Data::DataSet::addPredictedPeaks(double dmin, double dmax, std::shared_ptr<SX::Utils::ProgressHandler> handler)
{
    using SX::Instrument::Sample;

    class compare_fn {
    public:
        auto operator()(const Eigen::RowVector3i a, const Eigen::RowVector3i b) -> bool
        {
            if (a(0) != b(0))
                return a(0) < b(0);

            if (a(1) != b(1))
                return a(1) < b(1);

            return a(2) < b(2);
        }
    };


    int predicted_peaks = 0;

    auto& mono = getDiffractometer()->getSource()->getSelectedMonochromator();
    const double wavelength = mono.getWavelength();
    std::vector<sptrPeak3D> calculated_peaks;

    std::shared_ptr<Sample> sample = getDiffractometer()->getSample();
    unsigned int ncrystals = static_cast<unsigned int>(sample->getNCrystals());

    for (unsigned int i = 0; i < ncrystals; ++i) {
        SX::Crystal::SpaceGroup group(sample->getUnitCell(i)->getSpaceGroup());
        auto cell = sample->getUnitCell(i);
        auto UB = cell->getReciprocalStandardM();

        handler->setStatus("Calculating peak locations...");

        //auto predicted_hkls = sample->getUnitCell(i)->generateReflectionsInSphere(1.5);
        auto predicted_hkls = sample->getUnitCell(i)->generateReflectionsInShell(dmin, dmax, wavelength);

        predicted_peaks += predicted_hkls.size();

        std::vector<SX::Crystal::PeakCalc> peaks = hasPeaks(predicted_hkls, UB);
        calculated_peaks.reserve(peaks.size());

        int current_peak = 0;

        handler->setStatus("Building set of previously found peaks...");

        std::set<sptrPeak3D> found_peaks = getPeaks();
        std::set<Eigen::RowVector3i, compare_fn> found_hkls;


        Eigen::Vector3d lb = {0.0, 0.0, 0.0};
        Eigen::Vector3d ub = {double(getNCols()), double(getNRows()), double(getNFrames())};
        auto&& octree = Octree(lb, ub);

        octree.setMaxDepth(4);
        octree.setMaxStorage(50);

        handler->log("Building peak octree...");

        for (sptrPeak3D p: found_peaks) {
            found_hkls.insert(p->getIntegerMillerIndices());

            if (!p->isSelected() || p->isMasked()) {
                continue;
            }
            octree.addData(&p->getShape());
        }

        handler->log("Done building octree; number of chambers is " + std::to_string(octree.numChambers()));

        handler->setStatus("Adding calculated peaks...");

        int done_peaks = 0;

        #pragma omp parallel for
        for (size_t peak_id = 0; peak_id < peaks.size(); ++peak_id) {
            PeakCalc& p = peaks[peak_id];
            ++current_peak;

            Eigen::RowVector3i hkl(int(std::lround(p._h)), int(std::lround(p._k)), int(std::lround(p._l)));

            // try to find this reflection in the list of peaks, skip if found
            if (std::find(found_hkls.begin(), found_hkls.end(), hkl) != found_hkls.end() ) {
                continue;
            }

            // now we must add it, calculating shape from nearest peaks
             // K is outside the ellipsoid at PsptrPeak3D
            sptrPeak3D new_peak = p.averagePeaks(octree, search_radius);
            //sptrPeak3D new_peak = p.averagePeaks(numor);

            if (!new_peak) {
                continue;
            }

            new_peak->linkData(this);
            new_peak->setSelected(true);
            new_peak->addUnitCell(cell, true);
            new_peak->setObserved(false);

            #pragma omp critical
            calculated_peaks.push_back(new_peak);

            #pragma omp atomic
            ++done_peaks;
            int done = int(std::lround(done_peaks * 100.0 / peaks.size()));

            if ( done != last_done) {
                handler->setProgress(done);
                last_done = done;
            }
        }
    }
    for (sptrPeak3D peak: calculated_peaks) {
        addPeak(peak);
    }
    //qDebug() << "Integrating calculated peaks.";
    integratePeaks(_peakScale, _bkgScale, false, handler);
}

} // end namespace Data
} // end namespace SX
