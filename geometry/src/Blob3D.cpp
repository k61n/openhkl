#include "Blob3D.h"
#include <stdexcept>
#include <cmath>

namespace SX
{
namespace Geometry
{

Blob3D::Blob3D():_m000(0),_m100(0),_m010(0),_m001(0),_m200(0),_m020(0),_m002(0),_m110(0),_m101(0),_m011(0),_npoints(0)
{
}

Blob3D::Blob3D(const Blob3D& b)
{
	_m000=b._m000;
	_m100=b._m100;
	_m010=b._m010;
	_m001=b._m001;
	_m200=b._m200;
	_m020=b._m020;
	_m002=b._m002;
	_m110=b._m110;
	_m101=b._m101;
	_m011=b._m011;
	_npoints=b._npoints;
}

Blob3D& Blob3D::operator=(const Blob3D& b)
{
	if (this!=&b)
	{
		_m000=b._m000;
		_m100=b._m100;
		_m010=b._m010;
		_m001=b._m001;
		_m200=b._m200;
		_m020=b._m020;
		_m002=b._m002;
		_m110=b._m110;
		_m101=b._m101;
		_m011=b._m011;
		_npoints=b._npoints;
	}
	return *this;
}


Blob3D::Blob3D(double x, double y,double z, double m)
{
	_m000=m;
	double mx=m*x;
	double my=m*y;
	double mz=m*z;
	_m100=mx;
	_m010=my;
	_m001=mz;
	_m200=mx*x;
	_m020=my*y;
	_m002=mz*z;
	_m110=mx*y;
	_m101=mx*z;
	_m011=my*z;
	_npoints=1;
}

void Blob3D::addPoint(double x, double y, double z, double m)
{
	_m000+=m;
	double mx=m*x;
	double my=m*y;
	double mz=m*z;
	_m100+=mx;
	_m010+=my;
	_m001+=mz;
	_m200+=mx*x;
	_m020+=my*y;
	_m002+=mz*z;
	_m110+=mx*y;
	_m101+=mx*z;
	_m011+=my*z;
	_npoints++;
}

void Blob3D::merge(const Blob3D& b)
{
	_m000+=b._m000;
	_m100+=b._m100;
	_m010+=b._m010;
	_m001+=b._m001;
	_m200+=b._m200;
	_m020+=b._m020;
	_m002+=b._m002;
	_m110+=b._m110;
	_m101+=b._m101;
	_m011+=b._m011;
	_npoints+=b._npoints;
}

double Blob3D::getMass() const
{
	return _m000;
}
int Blob3D::getComponents() const
{
	return _npoints;
}

V3D Blob3D::getCenterOfMass() const
{
	if (_m000<1e-7)
		throw std::runtime_error("No mass in Blob");

	return V3D(_m100/_m000,_m010/_m000,_m001/_m000);
}


void Blob3D::printSelf(std::ostream& os) const
{
	os << "Blob center:" << _m100/_m000 << "," << _m010/_m000 << "," << _m001/_m000 << std::endl;
	os << "Blob #points:" << _npoints << std::endl;

}


std::ostream& operator<<(std::ostream& os, const Blob3D& b)
{
	b.printSelf(os);
	return os;
}

} // End namespace Geometry
} // End namspace SX
