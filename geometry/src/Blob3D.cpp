#include "Blob3D.h"
#include <stdexcept>
#include <cmath>
#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>
#include <limits>

namespace SX
{
namespace Geometry
{

Blob3D::Blob3D():_m000(0),_m100(0),_m010(0),_m001(0),_m200(0),_m020(0),_m002(0),_m110(0),_m101(0),_m011(0)
,_npoints(0),_minValue(std::numeric_limits<double>::max()),_maxValue(std::numeric_limits<double>::min())

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
	_minValue=b._minValue;
	_maxValue=b._maxValue;
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
		_minValue=b._minValue;
		_maxValue=b._maxValue;
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
	_minValue=m;
	_maxValue=m;
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
	if (m<_minValue)
		_minValue=m;
	if (m>_maxValue)
		_maxValue=m;
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
	_minValue=(_minValue < b._minValue ? _minValue : b._minValue);
	_maxValue=(_maxValue > b._maxValue ? _maxValue : b._maxValue);

}

double Blob3D::getMass() const
{
	return _m000;
}
int Blob3D::getComponents() const
{
	return _npoints;
}

double Blob3D::getMinimumMass() const
{
	return _minValue;
}

double Blob3D::getMaximumMass() const
{
	return _maxValue;
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
	os << "Intensity" << _m000 << std::endl;

}
void Blob3D::toEllipsoid(V3D& center, V3D& semi_axes, V3D& v0, V3D& v1, V3D& v2)
{
	if (_m000<1e-7)
		throw std::runtime_error("No mass in Blob");
	// Center of mass
	double xc=_m100/_m000;
	double yc=_m010/_m000;
	double zc=_m001/_m000;
	// Center of the blob
	center(xc,yc,zc);

	// Now compute second moment with respect to center of mass
	double Ixx=(_m020+_m002)/_m000-yc*yc-zc*zc;
	double Iyy=(_m200+_m002)/_m000-xc*xc-zc*zc;
	double Izz=(_m200+_m020)/_m000-xc*xc-yc*yc;
	double Ixy=_m110/_m000-xc*yc;
	double Iyz=_m011/_m000-yc*zc;
	double Ixz=_m101/_m000-xc*zc;

	// Diagonalize the inertia tensor
	double inertia[] ={Ixx,-Ixy,-Ixz,-Ixy, Iyy, -Iyz,-Ixz, -Iyz, Izz};
	gsl_matrix_view m = gsl_matrix_view_array(inertia, 3, 3);

	gsl_vector *val = gsl_vector_alloc (3);
	gsl_matrix *vec = gsl_matrix_alloc (3,3);

	gsl_eigen_symmv_workspace * w = gsl_eigen_symmv_alloc (3);

	gsl_eigen_symmv (&m.matrix, val, vec, w);
	gsl_eigen_symmv_free(w);
    gsl_eigen_symmv_sort(val,vec,GSL_EIGEN_SORT_ABS_ASC);

    // 2.sqrt(Eigenvalues) = semi-axes
	semi_axes[0]= 2.0*sqrt(gsl_vector_get(val, 0));
	semi_axes[1]= 2.0*sqrt(gsl_vector_get(val, 1));
	semi_axes[2]= 2.0*sqrt(gsl_vector_get(val, 2));

	// Now eigenvectors
	gsl_vector_view vec_0 = gsl_matrix_column(vec, 0);
	gsl_vector_view vec_1 = gsl_matrix_column(vec, 1);
	gsl_vector_view vec_2 = gsl_matrix_column(vec, 2);
	//
    v0(gsl_vector_get(&vec_0.vector,0),gsl_vector_get(&vec_0.vector,1),gsl_vector_get(&vec_0.vector,2));
    v1(gsl_vector_get(&vec_1.vector,0),gsl_vector_get(&vec_1.vector,1),gsl_vector_get(&vec_1.vector,2));
    v2(gsl_vector_get(&vec_2.vector,0),gsl_vector_get(&vec_2.vector,1),gsl_vector_get(&vec_2.vector,2));

    //

	return;
}

std::ostream& operator<<(std::ostream& os, const Blob3D& b)
{
	b.printSelf(os);
	return os;
}

} // End namespace Geometry
} // End namspace SX
