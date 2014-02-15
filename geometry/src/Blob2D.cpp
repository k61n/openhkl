#include "Blob2D.h"
#include <stdexcept>
#include <cmath>
#include <limits>
#include <boost/math/special_functions/erf.hpp>
#include <Eigen/Eigenvalues>



using Eigen::MatrixXd;
using Eigen::SelfAdjointEigenSolver;

namespace SX
{
namespace Geometry
{

Blob2D::Blob2D():_m00(0),_m10(0),_m01(0),_m20(0),_m02(0),_m11(0)
,_npoints(0),_minValue(std::numeric_limits<double>::max()),_maxValue(std::numeric_limits<double>::min())
{
}

Blob2D::Blob2D(const Blob2D& b)
{
	_m00=b._m00;
	_m10=b._m10;
	_m01=b._m01;
	_m20=b._m20;
	_m02=b._m02;
	_m11=b._m11;
	_npoints=b._npoints;
	_minValue=b._minValue;
	_maxValue=b._maxValue;
}

Blob2D& Blob2D::operator=(const Blob2D& b)
{
	if (this!=&b)
	{
		_m00=b._m00;
		_m10=b._m10;
		_m01=b._m01;
		_m20=b._m20;
		_m02=b._m02;
		_m11=b._m11;
		_npoints=b._npoints;
		_minValue=b._minValue;
		_maxValue=b._maxValue;
	}
	return *this;
}


Blob2D::Blob2D(double x, double y, double m)
{
	_m00=m;
	double mx=m*x;
	double my=m*y;
	_m10=mx;
	_m01=my;
	_m20=mx*x;
	_m02=my*y;
	_m11=mx*y;
	_npoints=1;
	_minValue=m;
	_maxValue=m;
}

void Blob2D::addPoint(double x, double y, double m)
{
	_m00+=m;
	double mx=m*x;
	double my=m*y;
	_m10+=mx;
	_m01+=my;
	_m20+=mx*x;
	_m02+=my*y;
	_m11+=mx*y;
	_npoints++;
	if (m<_minValue)
		_minValue=m;
	if (m>_maxValue)
		_maxValue=m;
}

void Blob2D::merge(const Blob2D& b)
{
	_m00+=b._m00;
	_m10+=b._m10;
	_m01+=b._m01;
	_m20+=b._m20;
	_m02+=b._m02;
	_m11+=b._m11;
	_npoints+=b._npoints;
	_minValue=(_minValue < b._minValue ? _minValue : b._minValue);
	_maxValue=(_maxValue > b._maxValue ? _maxValue : b._maxValue);
}

double Blob2D::getMass() const
{
	return _m00;
}
int Blob2D::getComponents() const
{
	return _npoints;
}

double Blob2D::getMinimumMass() const
{
	return _minValue;
}

double Blob2D::getMaximumMass() const
{
	return _maxValue;
}

Eigen::Vector2d Blob2D::getCenterOfMass() const
{
	if (_m00<1e-7)
		throw std::runtime_error("No mass in Blob");
	Eigen::Vector2d result;
	result << _m10/_m00, _m01/_m00;
	return result;
}

void Blob2D::toEllipse(double confidence,Eigen::Vector2d& center, Eigen::Vector2d& eigenvalues,Eigen::Matrix2d& eigenvectors) const
{
	if (_m00<1e-7)
		throw std::runtime_error("No mass in Blob");
	// Center of mass
	double xc=_m10/_m00;
	double yc=_m01/_m00;
	// Now compute second moment with respect to center of mass
	double Ixx=_m20/_m00-xc*xc;
	double Iyy=_m02/_m00-yc*yc;
	double Ixy=_m11/_m00-xc*yc;

	SelfAdjointEigenSolver<MatrixXd> solver;
	MatrixXd inertia(2,2);
	inertia << Ixx, Ixy, Ixy, Iyy;
	solver.compute(inertia);
	//
	center<< xc,yc;
	// Multiplicating factor from confidence to sigma.
	double factor=sqrt(2.0)*boost::math::erf_inv(confidence);
	eigenvalues(0)=sqrt(std::abs(solver.eigenvalues()[0]))*factor;
	eigenvalues(1)=sqrt(std::abs(solver.eigenvalues()[1]))*factor;
	//
	eigenvectors=solver.eigenvectors();
}

void Blob2D::printSelf(std::ostream& os) const
{
	os << "Blob center:" << _m10/_m00 << "," << _m01/_m00 << std::endl;
	os << "Blob mass:" << _m00 << std::endl;
	os << "Blob #points:" << _npoints << std::endl;

}


std::ostream& operator<<(std::ostream& os, const Blob2D& b)
{
	b.printSelf(os);
	return os;
}

} // End namespace Geometry
} // End namspace SX
