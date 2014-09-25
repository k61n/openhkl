#include <Eigen/Dense>

#include "Component.h"
#include "ComponentState.h"
#include "Detector.h"
#include "Peak3D.h"
#include "Sample.h"
#include "UBMinimizer.h"
#include "Gonio.h"

namespace SX
{

namespace Crystal
{

UBFunctor::UBFunctor() : Functor<double>(), _peaks(0), _detector(nullptr), _sample(nullptr), _fixedParameters()
{
}

UBFunctor::~UBFunctor() {
}

int UBFunctor::operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const
{
	if (!_detector || !_sample)
		throw std::runtime_error("class UBFunctor: a detector and a sample must be specified prior to calculate residuals.");

	int naxes=9;
	SX::Instrument::Gonio* dgonio=_detector->getGonio().get();
	if (dgonio)
	{
		for (unsigned int i=0;i<dgonio->numberOfAxes();++i)
			dgonio->getAxis(i)->setOffset(x[naxes++]);
	}
	SX::Instrument::Gonio* sgonio=_sample->getGonio().get();
	if (sgonio)
	{
		for (unsigned int i=0;i<sgonio->numberOfAxes();++i)
			sgonio->getAxis(i)->setOffset(x[naxes++]);
	}

	for (unsigned int i=0; i<_peaks.size();++i)
	{
		Eigen::RowVector3d qVector=_peaks[i].getQ();
		Eigen::RowVector3d hkl=_peaks[i].getMillerIndices();
		fvec(3*i)   = (x[0]*hkl[0] + x[3]*hkl[1] + x[6]*hkl[2] - qVector[0]);
		fvec(3*i+1) = (x[1]*hkl[0] + x[4]*hkl[1] + x[7]*hkl[2] - qVector[1]);
		fvec(3*i+2) = (x[2]*hkl[0] + x[5]*hkl[1] + x[8]*hkl[2] - qVector[2]);
	}

	return 0;
}

void UBFunctor::addPeak(const Peak3D& peak)
{
	_peaks.push_back(peak);
}

//double UBFunctor::calcSSE(const Eigen::VectorXd& x)
//{
//
//	int naxes=9;
//	SX::Instrument::Gonio* dgonio=_detector->getGonio().get();
//	if (dgonio)
//	{
//		for (unsigned int i=0;i<dgonio->numberOfAxes();++i)
//			dgonio->getAxis(i)->setOffset(x[naxes++]);
//	}
//	SX::Instrument::Gonio* sgonio=_sample->getGonio().get();
//	if (sgonio)
//	{
//		for (unsigned int i=0;i<sgonio->numberOfAxes();++i)
//			sgonio->getAxis(i)->setOffset(x[naxes++]);
//	}
//
//	double SSE=0.0;
//	for (unsigned int i=0; i<_peaks.size();++i)
//	{
//		Eigen::RowVector3d qVector=_peaks[i].getQ();
//		Eigen::RowVector3d hkl=_peaks[i].getMillerIndices();
//		double temp1=(x[0]*hkl[0] + x[3]*hkl[1] + x[6]*hkl[2] - qVector[0]);
//		double temp2=(x[1]*hkl[0] + x[4]*hkl[1] + x[7]*hkl[2] - qVector[1]);
//		double temp3=(x[2]*hkl[0] + x[5]*hkl[1] + x[8]*hkl[2] - qVector[2]);
//		SSE+=temp1*temp1+temp2*temp2+temp3*temp3;
//	}
//	return SSE/(values()-inputs());
//}

int UBFunctor::inputs() const
{
	return 9+_detector->numberOfAxes()+_sample->numberOfAxes();
}

int UBFunctor::values() const
{
	return 3*_peaks.size();
}

void UBFunctor::setDetector(SX::Instrument::Detector* detector)
{
	_detector=detector;
}

void UBFunctor::setSample(SX::Instrument::Sample* sample)
{
	_sample=sample;
}

void UBFunctor::reset()
{
	SX::Instrument::Gonio* dgonio=_detector->getGonio().get();
	if (dgonio)
	{
		for (unsigned int i=0;i<dgonio->numberOfAxes();++i)
			dgonio->getAxis(i)->setOffset(0.0);
	}
	SX::Instrument::Gonio* sgonio=_sample->getGonio().get();
	if (sgonio)
	{
		for (unsigned int i=0;i<sgonio->numberOfAxes();++i)
			sgonio->getAxis(i)->setOffset(0.0);
	}
}

void UBFunctor::setParameterFixed(unsigned int i)
{
	if (!_detector || !_sample)
		throw std::runtime_error("class UBFunctor: a detector and a sample must be specified prior to fixing parameters.");

	_fixedParameters.insert(i);

	unsigned int ii=i-9;
	if (ii<_detector->numberOfAxes())
	{
		_detector->getGonio()->getAxis(ii)->setOffsetFixed(true);
		return;
	}
	ii-=_detector->numberOfAxes();
	if (ii<_sample->numberOfAxes())
	{
		_sample->getGonio()->getAxis(ii)->setOffsetFixed(true);
	}

}

UBMinimizer::UBMinimizer(const UBFunctor& functor) : _functor(functor),_numDiff(functor),_minimizer(_numDiff), _solution()
{
	_minimizer.parameters.maxfev = 1000;
    _minimizer.parameters.xtol = 1.0e-10;
}

void UBMinimizer::setMaxIter(unsigned int max)
{
	_minimizer.parameters.maxfev = max;
}

int UBMinimizer::run()
{

	int nParams=_functor.inputs();
	Eigen::VectorXd x(nParams);

	int status = _minimizer.minimize(x);

	if (status==1)
	{
		Eigen::VectorXd fvec(nParams);
		_functor(x,fvec);
		double sse = fvec.squaredNorm();
		sse /= (_functor.values()-_functor.inputs());

	    Eigen::MatrixXd covMatrix = (_minimizer.fjac.transpose()*_minimizer.fjac).inverse();

	    Eigen::VectorXd sigmas = sse*covMatrix.diagonal().cwiseSqrt();

	    std::vector<bool> fParams(sigmas.size(),false);
	    for (auto it : _functor._fixedParameters)
	    	fParams[it] = true;

	    _solution = UBSolution(_functor._detector, _functor._sample, x, sigmas, fParams);
	}

	return status;

}

const UBSolution& UBMinimizer::getSolution() const
{
	return _solution;
}

UBSolution::UBSolution() : _detector(nullptr), _sample(nullptr)
{
}

UBSolution::UBSolution(SX::Instrument::Detector* detector,SX::Instrument::Sample* sample, Eigen::VectorXd values, Eigen::VectorXd sigmas, std::vector<bool> fixedParameters)
: _detector(detector), _sample(sample), _fixedParameters(fixedParameters)
{

	unsigned int idx = 0;
	_ub = Eigen::Map<Eigen::Matrix<double,3,3>>(values.segment(9,idx).data());
	_sigmaub = Eigen::Map<Eigen::Matrix<double,3,3>>(sigmas.segment(9,idx).data());

	idx += 9;
	_detectorOffsets = values.segment(_detector->numberOfAxes(),idx);
	_sigmaDetectorOffsets = sigmas.segment(_detector->numberOfAxes(),idx);

	idx+=_detector->numberOfAxes();
	_sampleOffsets = values.segment(_sample->numberOfAxes(),idx);
	_sigmaSampleOffsets = sigmas.segment(_sample->numberOfAxes(),idx);
}

UBSolution::UBSolution(const UBSolution& other)
{
	_detector = other._detector;
	_sample = other._sample;
	_ub = other._ub;
	_sigmaub = other._sigmaub;
	_detectorOffsets = other._detectorOffsets;
	_sigmaDetectorOffsets = other._sigmaDetectorOffsets;
	_sampleOffsets = other._sampleOffsets;
	_sigmaSampleOffsets = other._sigmaSampleOffsets;
	_fixedParameters = other._fixedParameters;
}

UBSolution& UBSolution::operator=(const UBSolution& other)
{
	if (this != &other)
	{
		_detector = other._detector;
		_sample = other._sample;
		_ub = other._ub;
		_sigmaub = other._sigmaub;
		_detectorOffsets = other._detectorOffsets;
		_sigmaDetectorOffsets = other._sigmaDetectorOffsets;
		_sampleOffsets = other._sampleOffsets;
		_sigmaSampleOffsets = other._sigmaSampleOffsets;
		_fixedParameters = other._fixedParameters;
	}
	return *this;
}

} // end namespace Crystal

} // end namespace SX
