#include "Blob2D.h"
#include <stdexcept>
#include <cmath>
#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>
#include <limits>

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
	double Ixx=_m02/_m00-yc*yc;
	double Iyy=_m20/_m00-xc*xc;
	double Ixy=_m11/_m00-xc*yc;
	// Diagonalize the second moment tensor [[Ixx,Ixy],[Ixy,Iyy]]
	double inertia[] ={Ixx,-Ixy,-Ixy,Iyy};
	gsl_matrix_view m = gsl_matrix_view_array(inertia, 2, 2);

	gsl_vector *val = gsl_vector_alloc (2);
	gsl_matrix *vec = gsl_matrix_alloc (2,2);

	gsl_eigen_symmv_workspace * w = gsl_eigen_symmv_alloc (2);

	gsl_eigen_symmv (&m.matrix, val, vec, w);
	gsl_eigen_symmv_free(w);
	gsl_eigen_symmv_sort(val,vec,GSL_EIGEN_SORT_ABS_ASC);

	// 2.sqrt(Eigenvalues) = semi-axes
	s_a= 2.0*sqrt(gsl_vector_get(val, 0));
	s_b= 2.0*sqrt(gsl_vector_get(val, 1));

	// Now get second eigenvector
	gsl_vector_view vec_1 = gsl_matrix_column(vec, 1);
	//
	double v1x=gsl_vector_get(&vec_1.vector,0);
	double v1y=gsl_vector_get(&vec_1.vector,1);
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
