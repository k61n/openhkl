#include "Blob2D.h"
#include <stdexcept>
#include <cmath>


namespace SX
{
namespace Geometry
{

Blob2D::Blob2D():_m00(0),_m10(0),_m01(0),_m20(0),_m02(0),_m11(0),_npoints(0)
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
}

double Blob2D::getMass() const
{
	return _m00;
}
int Blob2D::getComponents() const
{
	return _npoints;
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
	double Ixx=_m02/_m00-yc*yc;
	double Iyy=_m20/_m00-xc*xc;
	double Ixy=-(_m11/_m00-xc*yc);
	// Diagonalize the second moment tensor [[Ixx,Ixy],[Ixy,Iyy]]
	double b=Ixx+Iyy;
	double delta=b*b-4.0*(Ixx*Iyy-Ixy*Ixy);
	if (delta<0)
		throw std::runtime_error("Can't diagonalize Blob second moment tensor");
	// The semi_axis are twice the square-root of the eigenvalues
	delta=sqrt(delta);
	s_a=2.0*sqrt(0.5*(b+delta));
	s_b=2.0*sqrt(0.5*(b-delta));
	// Now compute the angle between the major axis and the x-axis
	// First eigenvector is (Ixy,s_a-Ixx)
	angle=atan2(s_a-Ixx,Ixy);
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
