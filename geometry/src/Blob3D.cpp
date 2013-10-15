#include <stdexcept>
#include <cmath>
#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>
#include <limits>

#include "Blob3D.h"

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
void Blob3D::toEllipsoid(V3D& center, V3D& semi_axes, V3D& v0, V3D& v1, V3D& v2) const
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
//    gsl_eigen_symmv_sort(val,vec,GSL_EIGEN_SORT_ABS_ASC);

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

bool Blob3D::intersectionWithPlane(double a, double b, double c, double d, V3D& center, V3D& semi_axes, V3D& axis1, V3D& axis2) const
{

	// The blob ellipsoid parameters
	V3D blob_semi_axes;
	V3D blob_center;
	V3D blob_axis1, blob_axis2, blob_axis3;

	// Get the blob ellipsoid parameters
	this->toEllipsoid(blob_center, blob_semi_axes, blob_axis1, blob_axis2, blob_axis3);
	std::cout<<"ELLIPSOID CENTER "<<blob_center<<std::endl;
	std::cout<<"ELLIPSOID SEMI AXES "<<blob_semi_axes<<std::endl;
	std::cout<<"ELLIPSOID AXIS1 "<<blob_axis1<<std::endl;
	std::cout<<"ELLIPSOID AXIS2 "<<blob_axis2<<std::endl;
	std::cout<<"ELLIPSOID AXIS3 "<<blob_axis3<<std::endl;

	// The vector normal to the plane
	V3D normal(a,b,c);
	double norm = normal.normalize();

	// This is the rotation matrix to align the normal to the plane with the z-axis
	Matrix33<double> R;
    V3D k_cp_n=UnitZ.cross_prod(normal);
    // Check whether the normal to the plane is colinear with the z-axis
    // If so, the rotation matrix is just the identity matrix
    if (k_cp_n.nullVector() == 1)
    {
    	R.identity();
    }
    // Otherwise, define the matrix as the Gibbs matrix
    else
    {
    	// Some intermediates variables useful for defining the Gibbs matrix
    	double k_ip_n = UnitZ.scalar_prod(normal);
    	double cd = cos(k_ip_n);
    	double sd = sin(k_ip_n);
    	double one_minus_cd = 1.0-cd;

        double vx = k_cp_n.x();
        double vy = k_cp_n.y();
        double vz = k_cp_n.z();

        double fx = vx*one_minus_cd;
        double fy = vy*one_minus_cd;
        double fz = vz*one_minus_cd;

        double sx = vx*sd;
        double sy = vy*sd;
        double sz = vz*sd;

    	R.set(cd+vx*fx,vy*fx+sz,vz*fx-sy, vx*fy-sz,cd+vy*fy,vz*fy+sx, vx*fz+sy,vy*fz-sx,cd+vz*fz);

    }

    // This is the intersection between the plane and the axis normal to the plane that passes through the origin
    V3D rp = normal*(d/norm);

    std::cout<<"rp"<<rp<<std::endl;

    // The difference vector between the aformentionned point and the center of the blob ellipsoid
    V3D u = rp-blob_center;

    // The matrix of the blob ellipsoid axis vectors
    Matrix33<double> Q;
    Q.set(blob_axis1.x(), blob_axis2.x(), blob_axis3.x(),
    	  blob_axis1.y(), blob_axis2.y(), blob_axis3.y(),
    	  blob_axis1.z(), blob_axis2.z(), blob_axis3.z());

    // The matrix of the blob ellipsoid semi axis
    double a2 = blob_semi_axes.x()*blob_semi_axes.x();
    double b2 = blob_semi_axes.y()*blob_semi_axes.y();
    double c2 = blob_semi_axes.z()*blob_semi_axes.z();
    Matrix33<double> E(1.0/a2, 0.0, 0.0, 0.0, 1.0/b2, 0.0, 0.0, 0.0, 1.0/c2);
    E = (Q.transpose()*E)*Q;

    std::cout<<"Q MATRIX"<<std::endl;
    std::cout<<Q<<std::endl;

    std::cout<<"E MATRIX"<<std::endl;
    std::cout<<E<<std::endl;

    Matrix33<double> M = E*R;
    Matrix33<double> RM = R.transpose()*M;

    std::cout<<"RM MATRIX"<<std::endl;
    std::cout<<RM<<std::endl;

    V3D v = E*u;
    V3D w = v*R + u*M;

    double r1r2 = RM(0,0)*RM(1,1);

    double den = 2.0*(r1r2 - RM(0,1)*RM(0,1));

    // The center of the intersection ellipse in the frame where z=0
    center[0] = (w[1]*RM(0,0)-w[0]*RM(1,1))/den;
    center[1] = (w[0]*RM(0,1)-w[1]*RM(0,0))/den;
    center[2] = 0.0;
    // The center of the intersection ellipse in the original frame
    center = R*center + rp;

    double phi = 0.5*atan2(2.0*RM(0,1),RM(1,1)-RM(0,0));

    double cphi = cos(phi);
    double sphi = sin(phi);

    // The axes of the intersection ellipse in the frame where z =0
    axis1[0] = cphi;
    axis1[1] = sphi;
    axis1[2] = 0.0;

    axis2[0] = -sphi;
    axis2[1] =  cphi;
    axis2[2] =  0.0;

    // The axes of the intersection ellipse in the original frame
    axis1 = R*axis1;
    axis2 = R*axis2;

    double cphi2 = cphi*cphi;
    double sphi2 = sphi*sphi;
    double r4s2phi = 2.0*RM(0,1)*cphi*sphi;

    // The value of the semi axes of the intersection ellipse
    semi_axes[0] = sqrt(1.0/(RM(0,0)*cphi2 + RM(1,1)*sphi2-r4s2phi));
    semi_axes[1] = sqrt(1.0/(RM(0,0)*sphi2 + RM(1,1)*cphi2+r4s2phi));

	return true;
}

std::ostream& operator<<(std::ostream& os, const Blob3D& b)
{
	b.printSelf(os);
	return os;
}

} // End namespace Geometry
} // End namspace SX
