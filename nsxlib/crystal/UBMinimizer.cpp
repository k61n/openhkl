#include <cassert>
#include <cmath>
#include <stdexcept>

#include <Eigen/Dense>

#include "../crystal/Peak3D.h"
#include "../crystal/UBMinimizer.h"
#include "../crystal/UBSolution.h"
#include "../instrument/Component.h"
#include "../instrument/ComponentState.h"
#include "../instrument/Detector.h"
#include "../instrument/Gonio.h"
#include "../instrument/Sample.h"
#include "../instrument/Source.h"
#include "../instrument/TransAxis.h"
#include "../instrument/RotAxis.h"
#include "../mathematics/MatrixOperations.h"
#include "../mathematics/Minimizer.h"
#include "../utils/Units.h"

namespace nsx {

UBMinimizer::UBMinimizer(const UBSolution& initialState): 
    _solution(initialState),
    _refineSource(false),
    _refineSample(initialState._sampleOffset.size(), false),
    _refineDetector(initialState._detectorOffset.size(), false)
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
        const Eigen::RowVector3d q0 = _peaks[i].first.getQ();
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

    // todo...
    #if 0
    if (status) {
        params = minimizer.params();

        // Create a vector to calculate final residuals
        Eigen::VectorXd fvec(values());
        // Calculate final residuals
        residuals(params, fvec);

        // The MSE is computed as SSE/dof where dof is the number of degrees of freedom
        auto&& fixed_parameters = _solution.fixedParameters();
        const int nFixed = std::count(fixed_parameters.begin(), fixed_parameters.end(), true);
        int nFreeParameters=_solution.inputs()-nFixed;
        double mse = fvec.squaredNorm()/(values()-nFreeParameters);
        int removed = 0;
        Eigen::MatrixXd jac = minimizer.jacobian();
        Eigen::MatrixXd JtJ = jac.transpose()*jac;

        for (unsigned int i = 9; i < fixed_parameters.size(); ++i) {
            if (fixed_parameters[i]) {
                removeColumn(JtJ, i-removed);
                removeRow(JtJ, i-removed);
                removed++;
            }
        }
        // The covariance matrix
        Eigen::MatrixXd covariance = JtJ.inverse();
        // rescale by mean-squared error to get estimate of variance-covariance in parameters
        covariance *= mse;
        _solution.update(params, covariance);        
    }
    nsx::UBSolution(initialState).apply();

    // debugging only
    #ifndef NDEBUG
    std::cout << "status is " << minimizer.getStatusStr()
              << " after " << minimizer.numIterations() << " iterations" << std::endl;
    #endif
    #endif

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
        params.addParameter(&_solution._cellParameters(i));
    }

    if (_refineSource) {
        params.addParameter(&_solution._sourceOffset);
    }

    for (auto i = 0; i < _solution._sampleOffset.size(); ++i) {
        if (_refineSample[i]) {
            params.addParameter(&_solution._sampleOffset[i]);
        }
    }

    for (auto i = 0; i < _solution._detectorOffset.size(); ++i) {
        if (_refineDetector[i]) {
            params.addParameter(&_solution._detectorOffset[i]);
        }
    }

    return params;
}

} // end namespace nsx
