#include "UBMinimizer.h"

#include <Eigen/Dense>

#include "Component.h"
#include "ComponentState.h"
#include "Detector.h"
#include "Gonio.h"
#include "Error.h"
#include "Gonio.h"
#include "Peak3D.h"
#include "Sample.h"

namespace SX
{

namespace Crystal
{


UBFunctor::UBFunctor() : Functor<double>(), _peaks(0), _detector(nullptr), _sample(nullptr), _fixedParameters()
{
}

UBFunctor::UBFunctor(const UBFunctor& other)
{
	_peaks = other._peaks;
	_detector = other._detector;
	_sample = other._sample;
	_fixedParameters = other._fixedParameters;
}

UBFunctor& UBFunctor::operator=(const UBFunctor& other)
{
	if (this != &other)
	{
		_peaks = other._peaks;
		_detector = other._detector;
		_sample = other._sample;
		_fixedParameters = other._fixedParameters;
	}
	return *this;
}

UBFunctor::~UBFunctor() {
}

int UBFunctor::operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const
{
	if (!_detector || !_sample)
		throw SX::Error::CrystalError<UBFunctor>("A detector and a sample must be specified prior to calculate residuals.");

	int naxes=9;
	auto dgonio=_detector->getGonio();
	if (dgonio)
	{
		for (unsigned int i=0;i<dgonio->numberOfAxes();++i)
			dgonio->getAxis(i)->setOffset(x[naxes++]);
	}
	auto sgonio=_sample->getGonio();
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

int UBFunctor::inputs() const
{

	int nInputs=9;

	if (_detector)
		nInputs += _detector->numberOfAxes();

	if (_sample)
		nInputs += _sample->numberOfAxes();

	return nInputs;
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

void UBFunctor::resetParameters()
{
	auto dgonio=_detector->getGonio();
	if (dgonio)
	{
		for (unsigned int i=0;i<dgonio->numberOfAxes();++i)
			dgonio->getAxis(i)->setOffset(0.0);
	}
	auto sgonio=_sample->getGonio();
	if (sgonio)
	{
		for (unsigned int i=0;i<sgonio->numberOfAxes();++i)
			sgonio->getAxis(i)->setOffset(0.0);
	}
}

void UBFunctor::setFixedParameters(unsigned int idx)
{
	if (!_detector || !_sample)
		throw SX::Error::CrystalError<UBFunctor>("A detector and a sample must be specified prior to fixing parameters.");

	if (idx>=static_cast<unsigned int>(inputs()))
		return;

	_fixedParameters.insert(idx);

	unsigned int ii=idx-9;
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

UBMinimizer::UBMinimizer() : _functor(UBFunctor()), _solution(), _start()
{

}

void UBMinimizer::addPeak(const Peak3D& peak)
{
	_functor.addPeak(peak);
}

void UBMinimizer::resetParameters()
{
	_functor.resetParameters();
}

void UBMinimizer::setDetector(SX::Instrument::Detector* detector)
{
	_functor.setDetector(detector);
}

void UBMinimizer::setFixedParameters(unsigned int idx)
{
	_functor.setFixedParameters(idx);
}


void UBMinimizer::setSample(SX::Instrument::Sample* sample)
{
	_functor.setSample(sample);
}

int UBMinimizer::run(unsigned int maxIter)
{

	int nParams=_functor.inputs();
	Eigen::VectorXd x=Eigen::VectorXd::Zero(nParams);

	for (auto it=_start.begin();it!=_start.end();++it)
		x[it->first] = it->second;


	typedef Eigen::NumericalDiff<UBFunctor> NumDiffType;
	NumDiffType numdiff(_functor);
	Eigen::LevenbergMarquardt<NumDiffType> minimizer(numdiff);
	minimizer.parameters.xtol=1e-10;
	minimizer.parameters.maxfev=maxIter;

	int status = minimizer.minimize(x);


	if (status==1)
	{
		std::vector<bool> fParams(x.size(),false);
		for (auto it : _functor._fixedParameters)
			fParams[it] = true;

		// Create a vector to calculate final residuals
		Eigen::VectorXd fvec(_functor.values());
		// Calculate final residuals
		_functor(x,fvec);
		// Sum of squared residuals
		double sse = fvec.squaredNorm();

		int freeParameters=_functor.inputs()-_functor._fixedParameters.size();
		// Divide by the DOF
		sse /= (_functor.values()-freeParameters);

		// Covariance matrix only for non-fixed parameters

	    Eigen::MatrixXd covMatrix = (minimizer.fjac.transpose()*minimizer.fjac).block(0,0,freeParameters,freeParameters).inverse();

	    // Sigma for non-fixed parameters
	    Eigen::VectorXd partialsigmas = (sse*covMatrix.diagonal()).cwiseSqrt();
	    Eigen::VectorXd sigmas(x.size());

	    int count=0;
	    for (int i=0;i<sigmas.size();++i)
	    {
	    	if (!fParams[i])
	    		sigmas[i]=partialsigmas[count++];
	    	else
	    		sigmas[i]=0.0;
	    }

	    _solution = UBSolution(_functor._detector, _functor._sample, x, sigmas, fParams);
	}

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

void UBMinimizer::setStartingValue(unsigned int idx, double value)
{
	if (idx >=static_cast<unsigned int>(_functor.inputs()))
		return;
	_start[idx] = value;
}

void UBMinimizer::unsetStartingValue(unsigned int idx)
{
	auto it = _start.find(idx);
	if(it != _start.end())
		_start.erase (it);
}

UBSolution::UBSolution() : _detector(nullptr), _sample(nullptr)
{
}

UBSolution::UBSolution(SX::Instrument::Detector* detector,SX::Instrument::Sample* sample,const Eigen::VectorXd& values,const Eigen::VectorXd& sigmas,const std::vector<bool>& fixedParameters)
: _detector(detector), _sample(sample), _fixedParameters(fixedParameters)
{
	unsigned int idx = 0;

	_ub  << values(0),values(1),values(2),values(3),values(4), values(5), values(6),values(7),values(8);
	_sigmaub << sigmas(0),sigmas(1),sigmas(2),sigmas(3),sigmas(4), sigmas(5), sigmas(6), sigmas(7), sigmas(8);

	idx += 9;
	_detectorOffsets = values.segment(idx,_detector->numberOfAxes());
	_sigmaDetectorOffsets = sigmas.segment(idx,_detector->numberOfAxes());

	idx+=_detector->numberOfAxes();
	_sampleOffsets = values.segment(idx,_sample->numberOfAxes());
	_sigmaSampleOffsets = sigmas.segment(idx,_sample->numberOfAxes());
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

std::ostream& operator<<(std::ostream& os, const UBSolution& solution)
{
	os<<"UB matrix:"<<std::endl;
	os<<solution._ub<<"\n\n";
	os<<"Detector offsets:"<<std::endl;
	os<<solution._detectorOffsets<<"\n\n";
	os<<"Sample offsets:"<<std::endl;
	os<<solution._sampleOffsets<<std::endl;

	return os;
}


} // end namespace Crystal

} // end namespace SX
