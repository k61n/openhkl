#include <cassert>
#include <cmath>
#include <stdexcept>

#include <Eigen/Dense>

#include "../crystal/Peak3D.h"
#include "../crystal/UBMinimizer.h"
#include "../instrument/Component.h"
#include "../instrument/ComponentState.h"
#include "../instrument/Detector.h"
#include "../instrument/Gonio.h"
#include "../instrument/Sample.h"
#include "../instrument/TransAxis.h"
#include "../instrument/RotAxis.h"
#include "../mathematics/Minimizer.h"
#include "../utils/EigenMatrixOp.h"
#include "../utils/Units.h"

namespace nsx {

UBFunctor::UBFunctor() : LMFunctor<double>(), _peaks(0), _detector(nullptr), _sample(nullptr),_source(nullptr), _fixedParameters()
{
}

UBFunctor::UBFunctor(const UBFunctor& other) : LMFunctor<double>(other)
{
    _peaks = other._peaks;
    _detector = other._detector;
    _sample = other._sample;
    _source = other._source;
    _fixedParameters = other._fixedParameters;
}

UBFunctor& UBFunctor::operator=(const UBFunctor& other)
{
    if (this != &other) {
        LMFunctor<double>::operator=(other);
        _peaks = other._peaks;
        _detector = other._detector;
        _sample = other._sample;
        _source = other._source;
        _fixedParameters = other._fixedParameters;
    }
    return *this;
}

int UBFunctor::operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const
{
    if (!_detector || !_sample || !_source) {
        throw std::runtime_error("A detector, sample and source must be specified prior to calculate residuals.");
    }

    // First 9 parameters are UB matrix
    int naxes=9;

    // Parameter 9 is offset in wavelength
    auto& mono = _source->getSelectedMonochromator();
    mono.setOffset(x[naxes++]);

    // Then n parameters for the detector
    auto sgonio=_sample->getGonio();
    if (sgonio) {
        for (unsigned int i = 0; i < sgonio->getNAxes(); ++i) {
            sgonio->getAxis(i)->setOffset(x[naxes++]);
        }
    }

    // finally, n parameters for the sample
    auto dgonio=_detector->getGonio();
    if (dgonio)	{
        for (unsigned int i=0;i<dgonio->getNAxes();++i) {
            dgonio->getAxis(i)->setOffset(x[naxes++]);
        }
    }

    #pragma omp parallel for
    for (unsigned int i=0; i<_peaks.size();++i)	{
        Eigen::RowVector3d qVector=_peaks[i].first.getQ();
        Eigen::RowVector3d hkl=_peaks[i].second;
        fvec(3*i)   = (x[0]*hkl[0] + x[3]*hkl[1] + x[6]*hkl[2] - qVector[0]);
        fvec(3*i+1) = (x[1]*hkl[0] + x[4]*hkl[1] + x[7]*hkl[2] - qVector[1]);
        fvec(3*i+2) = (x[2]*hkl[0] + x[5]*hkl[1] + x[8]*hkl[2] - qVector[2]);
    }
    return 0;
}

void UBFunctor::addPeak(const Peak3D& peak, const Eigen::RowVector3d& hkl)
{
    _peaks.push_back(std::make_pair(peak, hkl));
}

void UBFunctor::clearPeaks()
{
    _peaks.clear();
}

int UBFunctor::inputs() const
{
    // 9 UB parameters + wavelength
    int nInputs=10;

    if (_detector && _detector->hasGonio()) {
        auto gonio = _detector->getGonio();
        nInputs += gonio->getNAxes();
    }
    if (_sample && _sample->hasGonio()) {
        auto gonio = _sample->getGonio();
        nInputs += gonio->getNAxes();
    }
    return nInputs;
}

int UBFunctor::values() const
{
    return 3*_peaks.size();
}

void UBFunctor::setDetector(const std::shared_ptr<Detector>& detector)
{
    _detector = detector;
}

void UBFunctor::setSample(const std::shared_ptr<Sample>& sample)
{
    _sample = sample;
}
void UBFunctor::setSource(const std::shared_ptr<Source>& source)
{
    _source = source;
}

void UBFunctor::resetParameters()
{
    auto& mono = _source->getSelectedMonochromator();
    mono.setOffset(0.0);
    auto dgonio=_detector->getGonio();
    if (dgonio) {
        for (unsigned int i = 0; i < dgonio->getNAxes(); ++i) {
            dgonio->getAxis(i)->setOffset(0.0);
        }
    }
    auto sgonio = _sample->getGonio();
    if (sgonio) {
        for (unsigned int i = 0; i < sgonio->getNAxes(); ++i) {
            sgonio->getAxis(i)->setOffset(0.0);
        }
    }
}

void UBFunctor::refineParameter(unsigned int idx, bool refine)
{
    if (!_detector || !_sample || !_source) {
        throw std::runtime_error("A detector, sample and source must be specified prior to fixing parameters.");
    }
    if (idx >= static_cast<unsigned int>(inputs())) {
        return;
    }

    bool fixed = !refine;

    if (fixed) {
        _fixedParameters.insert(idx);
    } else {
        auto it = _fixedParameters.find(idx);
        if (it != _fixedParameters.end()) {
            _fixedParameters.erase(it);
        }
    }
    unsigned int ii = idx-9;
    if (ii == 0) {
        auto& mono = _source->getSelectedMonochromator();
        mono.setOffsetFixed(fixed);
        return;
    }
    ii--;
    if (_sample->hasGonio()) {
        if (ii < _sample->getGonio()->getNAxes()) {
            _sample->getGonio()->getAxis(ii)->setOffsetFixed(fixed);
        }
        ii-=_sample->getGonio()->getNAxes();
    }
    if (_detector->hasGonio() && ii < _detector->getGonio()->getNAxes()) {
        _detector->getGonio()->getAxis(ii)->setOffsetFixed(fixed);
    }
}

UBMinimizer::UBMinimizer() : _functor(UBFunctor()), _solution(), _start(), _minimizer()
{
}

UBMinimizer::~UBMinimizer()
{
}

void UBMinimizer::addPeak(const Peak3D& peak, const Eigen::RowVector3d& hkl)
{
    _functor.addPeak(peak,hkl);
}

void UBMinimizer::clearPeaks()
{
    _functor.clearPeaks();
}

void UBMinimizer::resetParameters()
{
    _functor.resetParameters();
}

void UBMinimizer::setDetector(const std::shared_ptr<Detector>& detector)
{
    _functor.setDetector(detector);
}

void UBMinimizer::setSource(const std::shared_ptr<Source>& source)
{
    _functor.setSource(source);
}

void UBMinimizer::refineParameter(unsigned int idx, bool refine)
{
    _functor.refineParameter(idx, refine);
}


void UBMinimizer::setSample(const std::shared_ptr<Sample>& sample)
{
    _functor.setSample(sample);
}

int UBMinimizer::run(unsigned int maxIter)
{
    int nParams=_functor.inputs();
    Eigen::VectorXd x=Eigen::VectorXd::Zero(nParams);

    for (auto&& it: _start) {
        x[it.first] = it.second;
    }

    _minimizer.initialize(nParams, _functor.values());
    _minimizer.setParams(x);
    _minimizer.set_f(_functor);

    _minimizer.setxTol(1e-10);
    _minimizer.setfTol(1e-10);
    _minimizer.setgTol(1e-10);

    bool status = _minimizer.fit(maxIter);

    if (status) {
        x = _minimizer.params();

        std::vector<bool> fParams(x.size(),false);
        for (auto&& it : _functor._fixedParameters) {
            fParams[it] = true;
        }
        // Create a vector to calculate final residuals
        Eigen::VectorXd fvec(_functor.values());
        // Calculate final residuals
        _functor(x,fvec);

        // The MSE is computed as SSE/dof where dof is the number of degrees of freedom
        int nFreeParameters=_functor.inputs()-_functor._fixedParameters.size();
        double mse = fvec.squaredNorm()/(_functor.values()-nFreeParameters);
        int removed = 0;
        Eigen::MatrixXd jac = _minimizer.jacobian();
        Eigen::MatrixXd JtJ = jac.transpose()*jac;

        for (unsigned int i=0;i<fParams.size();++i) {
            if (fParams[i]) {
                removeColumn(JtJ,i-removed);
                removeRow(JtJ,i-removed);
                removed++;
            }
        }
        // The covariance matrix
        Eigen::MatrixXd covariance = JtJ.inverse();
        covariance *= mse;

        _solution = UBSolution(_functor._detector, _functor._sample,_functor._source, x, covariance, fParams);
    }
    // debugging only
    std::cout << "status is " << _minimizer.getStatusStr()
              << " after " << _minimizer.numIterations() << " iterations" << std::endl;

    return status;
}

const UBSolution& UBMinimizer::getSolution() const
{
    return _solution;
}

void UBMinimizer::setStartingUBMatrix(const Eigen::Matrix3d& ub)
{
    setStartingValue(0,ub(0,0));
    setStartingValue(1,ub(0,1));
    setStartingValue(2,ub(0,2));
    setStartingValue(3,ub(1,0));
    setStartingValue(4,ub(1,1));
    setStartingValue(5,ub(1,2));
    setStartingValue(6,ub(2,0));
    setStartingValue(7,ub(2,1));
    setStartingValue(8,ub(2,2));
}

void UBMinimizer::setMinimizer(const Minimizer& minimizer)
{
    _minimizer = minimizer;
}

void UBMinimizer::setStartingValue(unsigned int idx, double value)
{
    if (idx >=static_cast<unsigned int>(_functor.inputs())) {
        return;
    }
    _start[idx] = value;
}

void UBMinimizer::unsetStartingValue(unsigned int idx)
{
    auto it = _start.find(idx);
    if(it != _start.end()) {
        _start.erase (it);
    }
}

UBSolution::UBSolution() : _detector(nullptr), _sample(nullptr),_source(nullptr),_sourceOffset(0),_sigmaSourceOffset(0)
{
}

UBSolution::UBSolution(std::shared_ptr<Detector> detector,
                       std::shared_ptr<Sample> sample,
                       std::shared_ptr<Source> source,
                       const Eigen::VectorXd& values,
                       const Eigen::MatrixXd& cov,
                       std::vector<bool> fixedParameters):

    _detector(std::move(detector)),
    _sample(std::move(sample)),
    _source(std::move(source)),
    _fixedParameters(std::move(fixedParameters))
{
    _ub  << values(0),values(1),values(2),values(3),values(4), values(5), values(6),values(7),values(8);
    _covub = cov.block(0,0,9,9);
    _sourceOffset=values(9);

    unsigned int idx = 10;
    std::size_t nSampleAxes= _sample->hasGonio() ? _sample->getGonio()->getNAxes() : 0;
    _sampleOffsets = values.segment(idx,nSampleAxes);
    _sigmaSampleOffsets = Eigen::VectorXd(nSampleAxes);

    idx+=nSampleAxes;
    std::size_t nDetectorAxes= _detector->hasGonio() ? _detector->getGonio()->getNAxes() : 0;
    _detectorOffsets = values.segment(idx,nDetectorAxes);
    _sigmaDetectorOffsets = Eigen::VectorXd(nDetectorAxes);

    idx = 9;

    auto& mono = _source->getSelectedMonochromator();

    if (mono.isOffsetFixed()) {
        _sigmaSourceOffset=0.0;
    } else {
        _sigmaSourceOffset=std::sqrt(cov(9,9));
        ++idx;
    }
    for (unsigned int i=0;i<nSampleAxes;++i) {
        if (_sample->getGonio()->getAxis(i)->hasOffsetFixed()) {
            _sigmaSampleOffsets[i] = 0.0;
        } else {
            _sigmaSampleOffsets[i] = std::sqrt(cov(idx,idx));
            idx++;
        }
    }
    for (unsigned int i=0;i<nDetectorAxes;++i) {
        if (_detector->getGonio()->getAxis(i)->hasOffsetFixed()) {
            _sigmaDetectorOffsets[i] = 0.0;
        } else {
            _sigmaDetectorOffsets[i] = std::sqrt(cov(idx,idx));
            idx++;
        }
    }
}

UBSolution::UBSolution(const UBSolution& other)
{
    _detector = other._detector;
    _sample = other._sample;
    _source= other._source;
    _ub = other._ub;
    _covub = other._covub;
    _sourceOffset=other._sourceOffset;
    _sigmaSourceOffset=other._sigmaSourceOffset;
    _detectorOffsets = other._detectorOffsets;
    _sigmaDetectorOffsets = other._sigmaDetectorOffsets;
    _sampleOffsets = other._sampleOffsets;
    _sigmaSampleOffsets = other._sigmaSampleOffsets;
    _fixedParameters = other._fixedParameters;
}

UBSolution& UBSolution::operator=(const UBSolution& other)
{
    if (this != &other) {
        _detector = other._detector;
        _sample = other._sample;
        _source = other._source;
        _ub = other._ub;
        _covub = other._covub;
        _sourceOffset= other._sourceOffset;
        _sigmaSourceOffset =other. _sigmaSourceOffset;
        _detectorOffsets = other._detectorOffsets;
        _sigmaDetectorOffsets = other._sigmaDetectorOffsets;
        _sampleOffsets = other._sampleOffsets;
        _sigmaSampleOffsets = other._sigmaSampleOffsets;
        _fixedParameters = other._fixedParameters;
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const UBSolution& solution)
{
    auto& mono = solution._source->getSelectedMonochromator();

    os<<"UB matrix:"<<std::endl;
    os<<solution._ub<< std::endl;
    os << "Wavelength:" << mono.getWavelength() << "("<< solution._sigmaSourceOffset<< ")" << std::endl;
    os<<"Detector offsets: " << std::endl;
    auto detectorG=solution._detector->getGonio();
    for (unsigned int i=0;i<detectorG->getNAxes();++i) {
        os << detectorG->getAxis(i)->getLabel() << " ";
        Axis* axis=detectorG->getAxis(i);
        if (dynamic_cast<TransAxis*>(axis) != nullptr) {
            os << solution._detectorOffsets[i]/mm << "(" << solution._sigmaDetectorOffsets[i]/mm << ") mm " << std::endl;
        } else if (dynamic_cast<RotAxis*>(axis) != nullptr) {
            os << solution._detectorOffsets[i]/deg << "(" << solution._sigmaDetectorOffsets[i]/deg << ") deg " << std::endl;
        }
    }
    os <<std::endl;
    os<<"Sample offsets:" << std::endl;
    auto sampleG=solution._sample->getGonio();
    for (unsigned int i=0;i<sampleG->getNAxes();++i) {
        os << sampleG->getAxis(i)->getLabel() << " ";
        Axis* axis=sampleG->getAxis(i);
        if (dynamic_cast<TransAxis*>(axis) != nullptr) {
            os << solution._sampleOffsets[i]/mm << "(" << solution._sigmaSampleOffsets[i]/mm << ") mm " << std::endl;
        } else if (dynamic_cast<RotAxis*>(axis) != nullptr) {
            os << solution._sampleOffsets[i]/deg << "(" << solution._sigmaSampleOffsets[i]/deg << ") deg " << std::endl;
        }
    }
    os<<std::endl;
    return os;
}

} // end namespace nsx
