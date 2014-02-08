#include "Blob2D.h"
#include <stdexcept>
#include <cmath>
#include <limits>
#include <Eigen/Eigenvalues>
#include <Eigen/Dense>


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

V2D<double> Blob2D::getCenterOfMass() const
{
	if (_m00<1e-7)
		throw std::runtime_error("No mass in Blob");

	return V2D<double>(_m10/_m00,_m01/_m00);
}

void Blob2D::toEllipse(double& xc, double& yc, double& s_a, double& s_b, double& angle) const
{
	if (_m00<1e-7)
		throw std::runtime_error("No mass in Blob");
	// Center of mass
	xc=_m10/_m00;
	yc=_m01/_m00;
	// Now compute second moment with respect to center of mass
	double Ixx=_m20/_m00-xc*xc;
	double Iyy=_m02/_m00-yc*yc;
	double Ixy=_m11/_m00-xc*yc;

	SelfAdjointEigenSolver<MatrixXd> solver;
	MatrixXd inertia(2,2);
	inertia(0,0)=Ixx; inertia(0,1)=Ixy;inertia(1,0)=Ixy; inertia(1,1)=Iyy;
	solver.compute(inertia);
	s_a=sqrt(std::abs(solver.eigenvalues()[0]));
	s_b=sqrt(std::abs(solver.eigenvalues()[1]));

	double v1x=(solver.eigenvectors().col(0))(0);
	double v1y=(solver.eigenvectors().col(0))(1);
	angle=atan2(v1y,v1x);

}

void Blob2D::printSelf(std::ostream& os) const
{
	os << "Blob center:" << _m10/_m00 << "," << _m01/_m00 << std::endl;
	os << "Blob #points:" << _npoints << std::endl;
	double a,b,c,d,e;
	toEllipse(a,b,c,d,e);
	os << "Blob bounding ellipse:" << a << " " << b << " " << c << " " << d << " " << e << std::endl;

}


std::ostream& operator<<(std::ostream& os, const Blob2D& b)
{
	b.printSelf(os);
	return os;
}

} // End namespace Geometry
} // End namspace SX
