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

UBMinimizer::UBMinimizer() : Minimizer<double>(), _peaks(0), _detector(nullptr), _sample(nullptr)
{
}

UBMinimizer::~UBMinimizer() {
}

int UBMinimizer::operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const
{
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

void UBMinimizer::addPeak(const Peak3D& peak)
{
	_peaks.push_back(peak);
}

double UBMinimizer::calcSSE(const Eigen::VectorXd& x)
{
	double SSE=0.0;
	for (unsigned int i=0; i<_peaks.size();++i)
	{
		Eigen::RowVector3d qVector=_peaks[i].getQ();
		Eigen::RowVector3d hkl=_peaks[i].getMillerIndices();
		double temp1=(x[0]*hkl[0] + x[3]*hkl[1] + x[6]*hkl[2] - qVector[0]);
		double temp2=(x[1]*hkl[0] + x[4]*hkl[1] + x[7]*hkl[2] - qVector[1]);
		double temp3=(x[2]*hkl[0] + x[5]*hkl[1] + x[8]*hkl[2] - qVector[2]);
		SSE+=temp1*temp1+temp2*temp2+temp3*temp3;
	}
	return SSE/(values()-inputs());
}

int UBMinimizer::inputs() const
{
	return 9+_detector->numberOfAxes()+_sample->numberOfAxes();
}

int UBMinimizer::values() const
{
	return 3*_peaks.size();
}

void UBMinimizer::setDetector(SX::Instrument::Detector* detector)
{
	_detector=detector;
}

void UBMinimizer::setSample(SX::Instrument::Sample* sample)
{
	_sample=sample;
}

void UBMinimizer::reset()
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

void UBMinimizer::setParameterFixed(int i)
{
	int ii=i-9;
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

} // end namespace Crystal

} // end namespace SX
