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
#include "TransAxis.h"
#include "RotAxis.h"
#include "Units.h"

namespace SX
{

namespace Crystal
{

void removeRow(Eigen::MatrixXd& matrix, unsigned int rowToRemove)
{
    unsigned int numRows = matrix.rows()-1;
    unsigned int numCols = matrix.cols();

    if( rowToRemove < numRows )
        matrix.block(rowToRemove,0,numRows-rowToRemove,numCols) = matrix.block(rowToRemove+1,0,numRows-rowToRemove,numCols);

    matrix.conservativeResize(numRows,numCols);
}

void removeColumn(Eigen::MatrixXd& matrix, unsigned int colToRemove)
{
    unsigned int numRows = matrix.rows();
    unsigned int numCols = matrix.cols()-1;

    if( colToRemove < numCols )
        matrix.block(0,colToRemove,numRows,numCols-colToRemove) = matrix.block(0,colToRemove+1,numRows,numCols-colToRemove);

    matrix.conservativeResize(numRows,numCols);
}


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
		for (unsigned int i=0;i<dgonio->nAxes();++i)
			dgonio->getAxis(i)->setOffset(x[naxes++]);
	}
	auto sgonio=_sample->getGonio();
	if (sgonio)
	{
		for (unsigned int i=0;i<sgonio->nAxes();++i)
			sgonio->getAxis(i)->setOffset(x[naxes++]);
	}

	for (unsigned int i=0; i<_peaks.size();++i)
	{
		Eigen::RowVector3d qVector=_peaks[i].getQ();
		const Eigen::RowVector3d& hkl=_peaks[i].getMillerIndices();
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
		nInputs += _detector->nAxes();

	if (_sample)
		nInputs += _sample->nAxes();

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
		for (unsigned int i=0;i<dgonio->nAxes();++i)
			dgonio->getAxis(i)->setOffset(0.0);
	}
	auto sgonio=_sample->getGonio();
	if (sgonio)
	{
		for (unsigned int i=0;i<sgonio->nAxes();++i)
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
	if (ii<_detector->nAxes())
	{
		_detector->getGonio()->getAxis(ii)->setOffsetFixed(true);
		return;
	}
	ii-=_detector->nAxes();
	if (ii<_sample->nAxes())
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
	minimizer.parameters.xtol=1e-11;
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

		// The MSE is computed as SSE/dof where dof is the number of degrees of freedom
		int nFreeParameters=_functor.inputs()-_functor._fixedParameters.size();
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

		// A copy of the CMINPACK FJAC matrix
		Eigen::MatrixXd fjac=minimizer.fjac.block(0,0,_functor.inputs(),_functor.inputs());

		// The R * P^t matrix
		Eigen::MatrixXd RPt = fjac.triangularView<Eigen::Upper>()*(minimizer.permutation.toDenseMatrix().cast<double>().transpose());

		Eigen::MatrixXd JtJ = RPt.transpose()*RPt;

		// Remove the fixed parameters before inverting J^t * J
		for (unsigned int i=0;i<fParams.size();++i)
		{
			if (fParams[i])
			{
				removeColumn(JtJ,i);
				removeRow(JtJ,i);
			}
		}

		// The covariance matrix
		Eigen::MatrixXd covariance=JtJ.inverse();

	    covariance *= mse;

	    std::cout << std::endl << covariance <<std::endl;

	    _solution = UBSolution(_functor._detector, _functor._sample, x, covariance, fParams);
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

UBSolution::UBSolution(SX::Instrument::Detector* detector,SX::Instrument::Sample* sample,const Eigen::VectorXd& values,const Eigen::MatrixXd& cov,const std::vector<bool>& fixedParameters)
: _detector(detector), _sample(sample), _fixedParameters(fixedParameters)
{
	_ub  << values(0),values(1),values(2),values(3),values(4), values(5), values(6),values(7),values(8);

	_covub = cov.block(0,0,9,9);

	unsigned int idx = 9;
	_detectorOffsets = values.segment(idx,_detector->nAxes());
	_sigmaDetectorOffsets = Eigen::VectorXd(_detector->nAxes());

	idx+=_detector->nAxes();
	_sampleOffsets = values.segment(idx,_sample->nAxes());
	_sigmaSampleOffsets = Eigen::VectorXd(_sample->nAxes());

	idx = 9;
	for (unsigned int i=0;i<_detector->nAxes();++i)
	{
		if (_detector->getGonio()->getAxis(i)->hasOffsetFixed())
			_sigmaDetectorOffsets[i] = 0.0;
		else
		{
			_sigmaDetectorOffsets[i] = std::sqrt(cov(idx,idx));
			idx++;
		}
	}

	for (unsigned int i=0;i<_sample->nAxes();++i)
	{
		if (_sample->getGonio()->getAxis(i)->hasOffsetFixed())
			_sigmaSampleOffsets[i] = 0.0;
		else
		{
			_sigmaSampleOffsets[i] = std::sqrt(cov(idx,idx));
			idx++;
		}
	}

}

UBSolution::UBSolution(const UBSolution& other)
{
	_detector = other._detector;
	_sample = other._sample;
	_ub = other._ub;
	_covub = other._covub;
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
		_covub = other._covub;
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
	os<<solution._ub<< std::endl;
	os<<"UB error:" << std::endl;
	os<<solution._covub << std::endl;
//	os<<solution._sigmaub << std::endl;
	os<<"Detector offsets: " << std::endl;
	auto detectorG=solution._detector->getGonio();
	for (unsigned int i=0;i<detectorG->nAxes();++i)
	{
		os << detectorG->getAxis(i)->getLabel() << " ";
		SX::Instrument::Axis* axis=detectorG->getAxis(i);
		if (dynamic_cast<SX::Instrument::TransAxis*>(axis))
			os << solution._detectorOffsets[i]/SX::Units::mm << "(" << solution._sigmaDetectorOffsets[i]/SX::Units::mm << ") mm " << std::endl;
		else if (dynamic_cast<SX::Instrument::RotAxis*>(axis))
			os << solution._detectorOffsets[i]/SX::Units::deg << "(" << solution._sigmaDetectorOffsets[i]/SX::Units::deg << ") deg " << std::endl;
	}
	os <<std::endl;
	os<<"Sample offsets:" << std::endl;
	auto sampleG=solution._sample->getGonio();
	for (unsigned int i=0;i<sampleG->nAxes();++i)
	{
		os << sampleG->getAxis(i)->getLabel() << " ";
		SX::Instrument::Axis* axis=sampleG->getAxis(i);
		if (dynamic_cast<SX::Instrument::TransAxis*>(axis))
			os << solution._sampleOffsets[i]/SX::Units::mm << "(" << solution._sigmaSampleOffsets[i]/SX::Units::mm << ") mm " << std::endl;
		else if (dynamic_cast<SX::Instrument::RotAxis*>(axis))
			os << solution._sampleOffsets[i]/SX::Units::deg << "(" << solution._sigmaSampleOffsets[i]/SX::Units::deg << ") deg " << std::endl;
	}
	os<<std::endl;

	return os;
}


} // end namespace Crystal

} // end namespace SX
