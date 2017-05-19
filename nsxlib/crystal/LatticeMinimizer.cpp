#include <algorithm>
#include <map>

#include <unsupported/Eigen/NonLinearOptimization>
#include <unsupported/Eigen/NumericalDiff>

#include "../crystal/LatticeMinimizer.h"
#include "../crystal/Peak3D.h"
#include "../instrument/Axis.h"
#include "../instrument/Detector.h"
#include "../instrument/Gonio.h"
#include "../instrument/Sample.h"
#include "../instrument/Source.h"
#include "../utils/EigenMatrixOp.h"

namespace nsx {

LatticeMinimizer::LatticeMinimizer() : _functor(LatticeFunctor()), _solution(), _start()
{

}

void LatticeMinimizer::addPeak(const Peak3D& peak)
{
    _functor._peaks.push_back(peak);
}

void LatticeMinimizer::clearPeaks()
{
    _functor._peaks.clear();
}

void LatticeMinimizer::setDetector(std::shared_ptr<Detector> detector)
{
    _functor._detector=detector;
}

void LatticeMinimizer::setSample(std::shared_ptr<Sample> sample)
{
    _functor._sample=sample;
}
void LatticeMinimizer::setSource(std::shared_ptr<Source> source)
{
    _functor._source=source;
}

void LatticeMinimizer::setConstraint(unsigned int idx, unsigned int target, double factor)
{
//	if (idx>=6 || target>=6)
//		return;
    _functor._constraints.insert(std::make_tuple(idx,target,factor));
}

void LatticeMinimizer::setConstant(unsigned int idx, double value)
{
//	if (idx>=6)
//		return;
    _functor._constants[idx] = value;
}

void LatticeMinimizer::resetOffsets()
{
    auto& mono = _functor._source->getSelectedMonochromator();
    mono.setOffset(0.0);
    auto dgonio=_functor._detector->getGonio();
    if (dgonio)
    {
        for (unsigned int i=0;i<dgonio->getNAxes();++i)
            dgonio->getAxis(i)->setOffset(0.0,true);
    }
    auto sgonio=_functor._sample->getGonio();
    if (sgonio)
    {
        for (unsigned int i=0;i<sgonio->getNAxes();++i)
            sgonio->getAxis(i)->setOffset(0.0,true);
    }
}

void LatticeMinimizer::setStartingValue(unsigned int idx, double value, bool constant)
{
    if (idx >=static_cast<unsigned int>(_functor.inputs()))
        return;
    _start[idx] = value;

    if (constant)
        setConstant(idx,value);
}

void LatticeMinimizer::unsetStartingValue(unsigned int idx)
{
    auto it = _start.find(idx);
    if(it != _start.end())
        _start.erase (it);
}

void LatticeMinimizer::setStartingLattice(double a, double b, double c, double alpha, double beta, double gamma, bool constant)
{
    setStartingValue(0,a, constant);
    setStartingValue(1,b, constant);
    setStartingValue(2,c, constant);
    setStartingValue(3,alpha, constant);
    setStartingValue(4,beta, constant);
    setStartingValue(5,gamma, constant);
}

const LatticeSolution& LatticeMinimizer::getSolution() const
{
    return _solution;
}

int LatticeMinimizer::run(unsigned int maxIter)
{

    int nParams=_functor.inputs();
    Eigen::VectorXd x=Eigen::VectorXd::Zero(nParams);

    for (auto it=_start.begin();it!=_start.end();++it)
        x[it->first] = it->second;

    typedef Eigen::NumericalDiff<LatticeFunctor> NumDiffType;
    NumDiffType numdiff(_functor);
    Eigen::LevenbergMarquardt<NumDiffType> minimizer(numdiff);
    minimizer.parameters.xtol=1e-11;
    minimizer.parameters.maxfev=maxIter;

    int status = minimizer.minimize(x);

    if (status==1)
    {

        std::vector<bool> fParams(x.size(),false);

        for (const auto& p: _functor._constants)
            fParams[p.first] = true;

        for (const auto& p: _functor._constraints)
            fParams[std::get<0>(p)] = true;

        // Create a vector to calculate final residuals
        Eigen::VectorXd fvec(_functor.values());
        // Calculate final residuals
        _functor(x,fvec);

        int nFixedParameters(std::count(fParams.begin(),fParams.end(),true));

        // The MSE is computed as SSE/dof where dof is the number of degrees of freedom
        int nFreeParameters=_functor.inputs()-nFixedParameters;
        double mse = fvec.squaredNorm()/(_functor.values()-nFreeParameters);

        // Computation of the covariance matrix
        // The covariance matrix is obtained from the estimated jacobian computed through a QR decomposition
        // (see for example:
        // 		- http://en.wikipedia.org/wiki/Non-linear_least_squares
        // 		- https://github.com/scipy/scipy/blob/v0.14.0/scipy/optimize/minpack.py#L256
        // 		- http://osdir.com/ml/python.scientific.user/2005-03/msg00067.html)

        // CMinPack does not provide directly the jacobian but the so called FJAC that is NOT the jacobian
        // FJAC is an output M by N array where M is the number of observations (_functor.values()) and N the number of parameters (_functor.inputs()).
        // The upper N by N submatrix of FJAC contains the upper triangular matrix R with diagonal elements of nonincreasing magnitude such that
        //
        //		P^t * (J^t * J) * P = R^t * R,
        //
        // where P is a permutation matrix and J is the final calculated jacobian
        // From (J^t * J) we can get directly get the covariance matrix C using the formula
        //
        // 		C = (J^t * J)^-1

        // The upper N*N block of the FJAC matrix
        Eigen::MatrixXd fjac=minimizer.fjac.block(0,0,_functor.inputs(),_functor.inputs());

        // The R * P^t matrix
        Eigen::MatrixXd RPt = fjac.triangularView<Eigen::Upper>()*(minimizer.permutation.toDenseMatrix().cast<double>().transpose());

        Eigen::MatrixXd JtJ = RPt.transpose()*RPt;

        // Remove the fixed parameters before inverting J^t * J
        int removed=0;

        for (unsigned int i=0;i<fParams.size();++i)
        {
            if (fParams[i])
            {
                removeColumn(JtJ,i-removed);
                removeRow(JtJ,i-removed);
                removed++;
            }
        }

        // The covariance matrix
        Eigen::MatrixXd covariance=JtJ.inverse();

        covariance *= mse;

        _solution = LatticeSolution(_functor._detector, _functor._sample,_functor._source, x, covariance, fParams);
    }

    return status;
}

} // end namespace nsx
