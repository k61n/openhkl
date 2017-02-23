//#include <utility>
//#include <stdexcept>
//#include <memory>
//
//#include "../utils/Units.h"
//
//#include <boost/filesystem.hpp>
//#include <boost/filesystem/operations.hpp>
//
//
//#include "H5Cpp.h"
//
//#include "blosc.h"
//
//#include "../data/blosc_filter.h"
//#include "../data/Data.h"
//#include "../data/BasicFrameIterator.h"
//#include "../data/IFrameIterator.h"
//#include "../data/ThreadedFrameIterator.h"
//#include "../instrument/Detector.h"
//#include "../instrument/Gonio.h"
//#include "../instrument/Sample.h"
//#include "../instrument/Source.h"
//#include "../geometry/Ellipsoid.h"
//
//namespace SX {
//
//namespace Data {
//
//using Eigen::Matrix3d;
//using boost::filesystem::path;
//
//Data::Data(IDataReader& dataReader)
//: _dataReader(dataReader),
//  _peaks(),
//  _masks(),
//  _background(0.0) {
//}
//
//std::unique_ptr<IFrameIterator> Data::getIterator(int idx)
//{
//    // use default frame iterator if one hasn't been set
//    if ( !_iteratorCallback) {
//        _iteratorCallback = [] (Data& data, int index) {
//            return new BasicFrameIterator(data, static_cast<unsigned int>(index));
//        };
//    }
//    return std::unique_ptr<IFrameIterator>(_iteratorCallback(*this, idx));
//}
//
//void Data::setIteratorCallback(FrameIteratorCallback callback)
//{
//    _iteratorCallback = std::move(callback);
//}
//
//Data::~Data()
//{
//    clearPeaks();
//    blosc_destroy();
//}
//
//int Data::dataAt(size_t x, size_t y, size_t z)
//{
//    // Check that the voxel is inside the limit of the data
//    if (z>=_dataReader.getNFrames() || y>=_dataReader.getNCols() || x>=_dataReader.getNRows()) {
//        return 0;
//    }
//    return _dataReader.getData(z)(x,y);
//}
//
//std::set<sptrPeak3D>& Data::getPeaks()
//{
//    return _peaks;
//}
//
//void Data::addPeak(const sptrPeak3D& peak)
//{
//    _peaks.insert(peak);
//    maskPeak(peak);
//}
//
//void Data::clearPeaks()
//{
//    for (auto&& ptr : _peaks) {
//        ptr->unlinkData();
//    }
//    _peaks.clear();
//}
//
//bool Data::removePeak(const sptrPeak3D& peak)
//{
//    auto&& it=_peaks.find(peak);
//
//    if (it == _peaks.end()) {
//        return false;
//    }
//    _peaks.erase(it);
//    return true;
//}
//
//
//void Data::addMask(AABB<double,3>* mask)
//{
//    _masks.insert(mask);
//    maskPeaks();
//}
//
//
//void Data::removeMask(AABB<double,3>* mask)
//{
//    auto&& p = _masks.find(mask);
//    if (p != _masks.end()) {
//        _masks.erase(mask);
//    }
//    maskPeaks();
//}
//
//const std::set<AABB<double, 3> *>& Data::getMasks()
//{
//    return _masks;
//}
//
//void Data::maskPeaks() const
//{
//    for (auto&& p : _peaks) {
//        maskPeak(p);
//    }
//}
//
//void Data::maskPeak(sptrPeak3D peak) const
//{
//    peak->setMasked(false);
//    for (auto&& m : _masks) {
//        // If the background of the peak intercept the mask, unselected the peak
//        if (m->intercept(peak->getBackground())) {
//            peak->setMasked(true);
//            break;
//        }
//    }
//}
//
//bool Data::inMasked(const Eigen::Vector3d& point) const
//{
//    // Loop over the defined masks and return true if one of them contains the point
//    for (auto&& m : _masks) {
//        if (m->isInsideAABB(point)) {
//            return true;
//        }
//    }
//    // No mask contains the point, return false
//    return false;
//}
//
//std::vector<PeakCalc> IData::hasPeaks(const std::vector<Eigen::Vector3d>& hkls, const Matrix3d& BU)
//{
//    std::vector<PeakCalc> peaks;
//    unsigned int scanSize = static_cast<unsigned int>(_states.size());
//    Eigen::Matrix3d UB = BU.transpose();
//    Eigen::Vector3d ki=_diffractometer->getSource()->getKi();
//    std::vector<Eigen::Matrix3d> rotMatrices;
//    rotMatrices.reserve(scanSize);
//    auto gonio = _diffractometer->getSample()->getGonio();
//    double wavelength_2 = -0.5 * _diffractometer->getSource()->getWavelength();
//
//    for (unsigned int s=0; s<scanSize; ++s) {
//        rotMatrices.push_back(gonio->getHomMatrix(_states[s].sample.getValues()).rotation());
//    }
//
//    for (const Eigen::Vector3d& hkl: hkls) {
//        // Get q at rest
//        Eigen::Vector3d q=UB*hkl;
//
//        double normQ2=q.squaredNorm();
//        // y component of q when in Bragg condition y=-sin(theta)*||Q||
//        double qy=normQ2*wavelength_2;
//
//        Eigen::Vector3d qi0=rotMatrices[0]*q;
//        Eigen::Vector3d qi;
//
//        bool sign = (qi0[1] > qy);
//        bool found = false;
//        unsigned int i;
//
//        for (i = 1; i < scanSize; ++i) {
//            qi=rotMatrices[i]*q;
//            bool sign2=(qi[1] > qy);
//            if (sign != sign2) {
//                found = true;
//                break;
//            }
//            qi0 = qi;
//        }
//
//        if (!found) {
//            continue;
//        }
//        double t = (qy-qi0[1]) / (qi[1]-qi0[1]);
//        Eigen::Vector3d kf=ki+qi0+(qi-qi0)*t;
//        t+=(i-1);
//
//        ComponentState dis=getInterpolatedState(_diffractometer->getDetector(),t);
//        double px,py;
//        // If hit detector, new peak
//        ComponentState cs=getInterpolatedState(_diffractometer->getSample(),t);
//        Eigen::Vector3d from=_diffractometer->getSample()->getPosition(cs.getValues());
//
//        double time;
//        bool accept=_diffractometer->getDetector()->receiveKf(px,py,kf,from,time,dis.getValues());
//
//        if (accept) {
//            peaks.emplace_back(hkl[0],hkl[1],hkl[2],px,py,t);
//        }
//    }
//    return peaks;
//}
//
//
//double Data::getBackgroundLevel(const std::shared_ptr<SX::Utils::ProgressHandler>& progress)
//{
//    if ( _background > 0.0 ) {
//        return _background;
//    }
//
//    // we calculate background in local variable bg for thread safety reasons--
//    // this method is called from a thread which could be aborted, so we do not want
//    // to write to _background until the calculation has been completed
//    double bg = 0.0;
//    size_t nFrames = _dataReader.getNFrames();
//    size_t nRows = _dataReader.getNRows();
//    size_t nCols = _dataReader.getNCols();
//    double factor = 1.0 / (nFrames * nRows * nCols);
//
//    if ( progress) {
//        progress->setStatus("Computing background level...");
//        progress->setProgress(0);
//    }
//
//    for (auto it = getIterator(0); it->index() != nFrames; it->advance()) {
//        // cast matrix to double (instead of int) -- necessary due to integer overflow!
//        // _background += factor * it->cast<double>().sum();
//        bg += factor * it->getFrame().sum();
//
//        if (progress) {
//            double done = 100.0 * it->index() / static_cast<double>(nFrames);
//            progress->setProgress(int(done));
//        }
//    }
//
//    if ( progress ) {
//        progress->setProgress(100);
//    }
//
//    _background = bg;
//    return _background;
//}
//
//void Data::integratePeaks(const std::shared_ptr<Utils::ProgressHandler>& handler)
//{
//    if (handler) {
//        handler->setStatus(("Integrating " + std::to_string(getPeaks().size()) + " peaks...").c_str());
//        handler->setProgress(0);
//    }
//
//    std::set<sptrPeak3D>& peaks = getPeaks();
//
//    for (auto&& peak: peaks ) {
//        peak->framewiseIntegrateBegin();
//    }
//
//    //progressDialog->setValue(0);
//    //progressDialog->setLabelText("Integrating peak intensities...");
//
//    size_t idx = 0;
//    int num_frames_done = 0;
//
//    size_t nFrames = _dataReader.getNFrames();
//
//    #pragma omp parallel for
//    for ( idx = 0; idx < nFrames; ++idx ) {
//        Eigen::MatrixXi frame;
//        #pragma omp critical
//        frame = _dataReader.getData(idx);
//
//        for ( auto& peak: peaks ) {
//            peak->framewiseIntegrateStep(frame, idx);
//        }
//
//        if (handler) {
//            #pragma omp atomic
//            ++num_frames_done;
//            double progress = num_frames_done * 100.0 / nFrames;
//            handler->setProgress(progress);
//        }
//    }
//
//    for (auto&& peak: peaks) {
//        peak->framewiseIntegrateEnd();
//    }
//}
//
//IDataReader& Data::getDataReader() {
//    return _dataReader;
//}
//
//} // end namespace Data
//
//} // end namespace SX
