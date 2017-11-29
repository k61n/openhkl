#include <cassert>
#include <cmath>
#include <stdexcept>

#include <Eigen/Dense>

#include "Component.h"
#include "ComponentState.h"
#include "Detector.h"
#include "Gonio.h"
#include "Peak3D.h"
#include "Sample.h"
#include "Source.h"
#include "TransAxis.h"
#include "RotAxis.h"
#include "MatrixOperations.h"
#include "Minimizer.h"
#include "ReciprocalVector.h"
#include "UBMinimizer.h"
#include "UBSolution.h"
#include "Units.h"

namespace nsx {

UBMinimizer::UBMinimizer(const UBSolution& initialState): 
    _solution(initialState),
    _refineSource(false),
    _refineSample(initialState._sampleOffset.size(), false),
    _sampleID(initialState._sampleOffset.size(), -1),
    _refineDetector(initialState._detectorOffset.size(), false),
    _detectorID(initialState._detectorOffset.size(), -1),
    _ucID(initialState._cellParameters.size(), -1)
{

}

int UBMinimizer::residuals(Eigen::VectorXd &fvec)
{
    const unsigned int npeaks = _peaks.size();

    // update the offsets
    _solution.apply();    

    // get the UB matrix
    Eigen::Matrix3d UB = _solution.ub();

    //#pragma omp parallel for
    for (unsigned int i = 0; i < npeaks; ++i)	{
        auto&& peak = _peaks[i].first;
        auto cell = peak.getActiveUnitCell();
        auto q = peak.getQ();

        const Eigen::RowVector3d q0 = static_cast<const Eigen::RowVector3d&>(q);
        const Eigen::RowVector3d hkl = _peaks[i].second;

        const Eigen::RowVector3d q1 = hkl * UB;
        const Eigen::RowVector3d dq = q1-q0;

        fvec(3*i)   = dq[0];
        fvec(3*i+1) = dq[1];
        fvec(3*i+2) = dq[2];

    }
    return 0;
}

void UBMinimizer::addPeak(const Peak3D& peak, const Eigen::RowVector3d& hkl)
{
    _peaks.push_back(std::make_pair(peak, hkl));
}

int UBMinimizer::run(unsigned int maxIter)
{
    FitParameters params = fitParameters();
    
    auto functor = [this] (Eigen::VectorXd& r) -> int
    {
        return this->residuals(r);
    };

    Minimizer minimizer;

    minimizer.initialize(params, 3*_peaks.size());
    minimizer.set_f(functor);

    minimizer.setxTol(1e-10);
    minimizer.setfTol(1e-10);
    minimizer.setgTol(1e-10);

    bool status = minimizer.fit(maxIter);

    // covariance matrix of the fit parameters
    const double mse = minimizer.meanSquaredError();
    auto&& jac = minimizer.jacobian();
    Eigen::MatrixXd cov = mse*minimizer.covariance();
    updateError(cov);

    return status;
}

const UBSolution& UBMinimizer::solution() const
{
    return _solution;
}

void UBMinimizer::refineSource(bool refine)
{
    _refineSource = refine;
}

void UBMinimizer::refineSample(unsigned int id, bool refine)
{
    _refineSample[id] = refine;
}

void UBMinimizer::refineDetector(unsigned int id, bool refine)
{
    _refineDetector[id] = refine;
}

FitParameters UBMinimizer::fitParameters() 
{
    FitParameters params;

    for (int i = 0; i < 3; ++i) {
        params.addParameter(&_solution._uOffsets(i));
    }

    for (int i = 0; i < _solution._cellParameters.size(); ++i) {
        _ucID[i] = params.addParameter(&_solution._cellParameters(i));
    }

    if (_refineSource) {
        _sourceID = params.addParameter(&_solution._sourceOffset);
    }

    for (auto i = 0; i < _solution._sampleOffset.size(); ++i) {
        if (_refineSample[i]) {
            _sampleID[i] = params.addParameter(&_solution._sampleOffset[i]);
        }
    }

    for (auto i = 0; i < _solution._detectorOffset.size(); ++i) {
        if (_refineDetector[i]) {
            _detectorID[i] = params.addParameter(&_solution._detectorOffset[i]);
        }
    }

    return params;
}

void UBMinimizer::updateError(const Eigen::MatrixXd& cov)
{
    // error in source
    _solution._sigmaSource = _refineSource ? std::sqrt(cov(_sourceID, _sourceID)) : 0.0;
    
    // error in sample offsets
    for (int i = 0; i < _solution._sampleOffset.size(); ++i) {
        int id = _sampleID[i];
        _solution._sigmaSample[i] = _refineSample[i] ? std::sqrt(cov(id, id)) : 0.0;
    }
    
    // error in detector offsets
    for (int i = 0; i < _solution._detectorOffset.size(); ++i) {
        int id = _detectorID[i];
        _solution._sigmaDetector[i] = _refineDetector[i] ? std::sqrt(cov(id, id)) : 0.0;
    }
}

} // end namespace nsx
