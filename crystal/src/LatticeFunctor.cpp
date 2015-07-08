#include <cmath>

#include <Eigen/Geometry>

#include "Detector.h"
#include "Error.h"
#include "Gonio.h"
#include "LatticeFunctor.h"
#include "Sample.h"
#include "Source.h"
#include "UnitCell.h"

namespace SX
{

namespace Crystal
{

LatticeFunctor::LatticeFunctor()
: Utils::LMFunctor<double>(),
  _peaks(0),
  _detector(nullptr),
  _sample(nullptr),
  _source(nullptr),
  _constrainedCellParams(),
  _fixedCellParams(),
  _fixedInstrParams()
{
}

LatticeFunctor::LatticeFunctor(const LatticeFunctor& other) : Utils::LMFunctor<double>(other)
{
	_peaks = other._peaks;
	_detector = other._detector;
	_sample = other._sample;
	_source = other._source;
	_constrainedCellParams = other._constrainedCellParams;
	_fixedCellParams = other._fixedCellParams;
	_fixedInstrParams = other._fixedInstrParams;

}

LatticeFunctor& LatticeFunctor::operator=(const LatticeFunctor& other)
{
	if (this != &other)
	{
		Utils::LMFunctor<double>::operator=(other);
		_peaks = other._peaks;
		_detector = other._detector;
		_sample = other._sample;
		_source = other._source;
		_constrainedCellParams = other._constrainedCellParams;
		_fixedCellParams = other._fixedCellParams;
		_fixedInstrParams = other._fixedInstrParams;
	}
	return *this;
}

LatticeFunctor::~LatticeFunctor()
{
}

int LatticeFunctor::operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const
{
	if (!_detector || !_sample || !_source)
		throw SX::Kernel::Error<LatticeFunctor>("A detector, sample and source must be specified prior to calculate residuals.");

	// The parameters 0,1,2,3,4,5 are the six cell parameters
	Eigen::VectorXd xlocal(x.segment(0,6));

	// Set the fixed cell parameters
	for (const auto& p : _fixedCellParams)
		xlocal[p.first] = p.second;

	// Set the cell constraints
	for (const auto& p : _constrainedCellParams)
		xlocal[p.first] = x[p.second];

	UnitCell uc(xlocal[0],xlocal[1],xlocal[2],xlocal[3],xlocal[4],xlocal[5]);

	Eigen::Matrix3d B=uc.getBusingLevyB();

	// The parameters 6,7,8 are the three angles that will define U matrix through quaternion formalism
	// 6 = phi angle of spherical coordinates
	// 7 = theta angle of spherical coordinates
	// 8 = omega angle of quaternion
	double sThetacPhi=sin(x[6])*cos(x[7]);
	double sThetasPhi=sin(x[6])*sin(x[7]);
	double cPhi = cos(x[6]);
	double omegaOver2 = x[8]/2.0;
	double cOmegaOver2 = cos(omegaOver2);
	double sOmegaOver2 = sin(omegaOver2);
	Eigen::Quaterniond quat(cOmegaOver2,sOmegaOver2*sThetacPhi,sOmegaOver2*sThetasPhi,sOmegaOver2*cPhi);
	Eigen::Matrix3d U(quat.toRotationMatrix());
	U.transposeInPlace();

	Eigen::Matrix3d BU = B*U;

	// The other parameters are for the instrument offset
	int naxes=9;

	// Parameter 9 is offset in wavelength
	_source->setOffset(x[naxes++]);

	// Then n parameters for the detector
	auto sgonio=_sample->getGonio();
	if (sgonio)
	{
		for (unsigned int i=0;i<sgonio->getNAxes();++i)
			sgonio->getAxis(i)->setOffset(x[naxes++]);
	}

	// finally, n parameters for the sample
	auto dgonio=_detector->getGonio();
	if (dgonio)
	{
		for (unsigned int i=0;i<dgonio->getNAxes();++i)
			dgonio->getAxis(i)->setOffset(x[naxes++]);
	}

	for (unsigned int i=0; i<_peaks.size();++i)
	{
		Eigen::RowVector3d qVector=_peaks[i].getQ();
		const Eigen::RowVector3d& hkl=_peaks[i].getMillerIndices();
		fvec(3*i)   = (BU(0,0)*hkl[0] + BU(0,1)*hkl[1] + BU(0,2)*hkl[2] - qVector[0]);
		fvec(3*i+1) = (BU(1,0)*hkl[0] + BU(1,1)*hkl[1] + BU(1,2)*hkl[2] - qVector[1]);
		fvec(3*i+2) = (BU(2,0)*hkl[0] + BU(2,1)*hkl[1] + BU(2,2)*hkl[2] - qVector[2]);
	}

	return 0;
}

int LatticeFunctor::inputs() const
{
	// 10 = 6 lattice parameters + 3 U matrix quaternion parameters + wavelength
	int nInputs=10;

	if (_detector)
		nInputs += _detector->getNAxes();

	if (_sample)
		nInputs += _sample->getNAxes();

	return nInputs;
}

int LatticeFunctor::values() const
{
	return 3*_peaks.size();
}

} // end namespace Crystal

} // end namespace SX
