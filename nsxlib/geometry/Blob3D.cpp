#include <cmath>
#include <limits>
#include <stdexcept>
#include <cmath>
#include <Eigen/Eigenvalues>
#include "Blob3D.h"
#include "../utils/erf_inv.h"

using Eigen::Vector3d;
using Eigen::Vector2d;
using Eigen::Matrix3d;
using Eigen::Matrix2d;
using Eigen::SelfAdjointEigenSolver;

static const double minimum_blob_mass = 1e-15;

namespace nsx
{
namespace Geometry
{

Blob3D::Blob3D():_m000(0),_m100(0),_m010(0),_m001(0),_m200(0),_m020(0),_m002(0),_m110(0),_m101(0),_m011(0)
,_npoints(0),_minValue(std::numeric_limits<double>::max()),_maxValue(std::numeric_limits<double>::min())
{
}

Blob3D::Blob3D(const Blob3D& b)
:	_m000(b._m000),
    _m100(b._m100),_m010(b._m010),_m001(b._m001),
    _m200(b._m200),_m020(b._m020),_m002(b._m002),
    _m110(b._m110),_m101(b._m101),_m011(b._m011),
    _npoints(b._npoints),
    _minValue(b._minValue),_maxValue(b._maxValue)
{
}

Blob3D& Blob3D::operator=(const Blob3D& b)
{
    if (this!=&b)
    {
        _m000=b._m000;
        _m100=b._m100;_m010=b._m010;_m001=b._m001;
        _m200=b._m200;_m020=b._m020;_m002=b._m002;
        _m110=b._m110;_m101=b._m101;_m011=b._m011;
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
    _m100=mx;_m010=my;_m001=mz;
    _m200=mx*x;_m020=my*y;_m002=mz*z;
    _m110=mx*y;_m101=mx*z;_m011=my*z;
    _npoints=1;
    _minValue=m;_maxValue=m;
}

void Blob3D::addPoint(double x, double y, double z, double m)
{
    _m000+=m;
    double mx=m*x;double my=m*y;double mz=m*z;
    _m100+=mx;_m010+=my;_m001+=mz;
    _m200+=mx*x;_m020+=my*y;_m002+=mz*z;
    _m110+=mx*y;_m101+=mx*z;_m011+=my*z;
    _npoints++;
    if (m<_minValue)
        _minValue=m;
    if (m>_maxValue)
        _maxValue=m;
}

void Blob3D::merge(const Blob3D& b)
{
    _m000+=b._m000;
    _m100+=b._m100;_m010+=b._m010;_m001+=b._m001;
    _m200+=b._m200;_m020+=b._m020;_m002+=b._m002;
    _m110+=b._m110;_m101+=b._m101;_m011+=b._m011;
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

Eigen::Vector3d Blob3D::getCenterOfMass() const
{
    if (_m000 < minimum_blob_mass)
        throw std::runtime_error("No mass in Blob");
    Vector3d result;
    result << _m100/_m000,_m010/_m000,_m001/_m000;
    return result;
}


void Blob3D::printSelf(std::ostream& os) const
{
    Eigen::Vector3d center;
    center << _m100/_m000, _m010/_m000, _m001/_m000;
    os << "#Blob center: " << center.transpose() << std::endl;
    os << "Mass: " << _m000 << std::endl;
    os << "Points in the blob: " << _npoints << std::endl;

}
void Blob3D::toEllipsoid(double confidence,Vector3d& center, Vector3d& eigenvalues, Matrix3d& eigenvectors) const
{
    if (_m000 < minimum_blob_mass) {
        throw std::runtime_error("No mass in Blob");
    }

    // Center of mass
    double xc=_m100/_m000;
    double yc=_m010/_m000;
    double zc=_m001/_m000;

    // Center of the ellipsoid
    center << xc,yc,zc;

    // Define the variance-covariance tensor
    Matrix3d inertia;
    inertia(0,0)=_m200/_m000-xc*xc;
    inertia(1,1)=_m020/_m000-yc*yc;
    inertia(2,2)=_m002/_m000-zc*zc;
    inertia(0,1)=inertia(1,0)=_m110/_m000-xc*yc;
    inertia(0,2)=inertia(2,0)=_m101/_m000-xc*zc;
    inertia(1,2)=inertia(2,1)=_m011/_m000-yc*zc;

    SelfAdjointEigenSolver<Matrix3d> solver;
    solver.compute(inertia);

    double factor=sqrt(2.0)*nsx::Utils::erf_inv(confidence);
    // This is the Gaussian sigma along three directions
    // (fabs is a safe-guard against very small negative eigenvalues due to precision errors)
    eigenvalues <<  sqrt(std::fabs(solver.eigenvalues()[0]))*factor,
                    sqrt(std::fabs(solver.eigenvalues()[1]))*factor,
                    sqrt(std::fabs(solver.eigenvalues()[2]))*factor;

    // Now eigenvectors
    eigenvectors = solver.eigenvectors();

    return;
}

bool Blob3D::intersectionWithPlane(double a, double b, double c, double d, Vector3d& center, Vector3d& semi_axes, Vector3d& axis1, Vector3d& axis2, double confidence) const
{

    // The blob ellipsoid parameters
    Vector3d blob_semi_axes;
    Vector3d blob_center;
    Matrix3d eigvect;

    // Get the blob ellipsoid parameters
    this->toEllipsoid(confidence, blob_center, blob_semi_axes, eigvect);

    // The vector normal to the plane
    Vector3d normal;
    normal << a,b,c;
    double norm = normal.norm();
    normal/=norm;
    // This is the rotation matrix to align the normal to the plane with the z-axis
    Matrix3d R=Matrix3d::Identity();
    Vector3d k_cp_n=Vector3d::UnitZ().cross(normal);
    // Check whether the normal to the plane is collinear with the z-axis
    // If so, the rotation matrix is just the identity matrix
    if (k_cp_n.isZero(1e-5))
    {
        R.Identity();
    }
    // Otherwise, define the matrix as the Gibbs matrix
    else
    {
        k_cp_n.normalize();
        // Some intermediates variables useful for defining the Gibbs matrix
        double cd = Vector3d::UnitZ().dot(normal);
        double sd = sqrt(1.0-cd*cd);
        double one_minus_cd = 1.0-cd;

        double vx = k_cp_n(0);
        double vy = k_cp_n(1);
        double vz = k_cp_n(2);

        double fx = vx*one_minus_cd;
        double fy = vy*one_minus_cd;
        double fz = vz*one_minus_cd;

        double sx = vx*sd;
        double sy = vy*sd;
        double sz = vz*sd;

        R << cd+vx*fx, vx*fy-sz, vx*fz+sy,
             vy*fx+sz, cd+vy*fy, vy*fz-sx,
             vz*fx-sy, vz*fy+sx, cd+vz*fz;

    }

    // This is the intersection between the plane and the axis normal to the plane that passes through the origin
    Vector3d rp = normal*(d/norm);

    // The difference vector between the aforementionned point and the center of the blob ellipsoid
    Vector3d u = rp-blob_center;

    // The matrix of the blob ellipsoid semi-axis
    double a2 = std::pow(blob_semi_axes(0),2);
    double b2 = std::pow(blob_semi_axes(1),2);
    double c2 = std::pow(blob_semi_axes(2),2);
    Matrix3d E;
    E << 1.0/a2, 0.0, 0.0,
         0.0, 1.0/b2, 0.0,
         0.0, 0.0, 1.0/c2;
    E = eigvect*E*eigvect.transpose();

    Matrix3d M = E*R;
    Matrix3d RM = R.transpose()*M;

    Vector3d v = E*u;
    Vector3d w = v.transpose()*R + u.transpose()*M;

    double Aq = RM(0,0);
    double Bq = RM(0,1);
    double Cq = RM(1,1);
    double Dq = w(0)/2.0;
    double Eq = w(1)/2.0;
    double Fq = u.transpose()*v - 1.0;
    Matrix3d AQ;
    AQ <<Aq,Bq,Dq,
         Bq,Cq,Eq,
         Dq,Eq,Fq;

    double detAq = AQ.determinant();

    if (std::abs(detAq) <= 1.0E-7)
    {
        return false;
    }

    if ((Aq+Cq)*detAq > 0.0)
    {
        return false;
    }

    double detA33 = Aq*Cq - Bq*Bq;

    if (detA33 <= 0.0)
    {
        return false;
    }

    //
    SelfAdjointEigenSolver<Matrix2d> solver;
    Matrix2d A33;
    A33 << Aq, Bq,
           Bq, Cq;
    solver.compute(A33);

    double fact = -detAq/detA33;

    semi_axes(0) = sqrt(fact/solver.eigenvalues()[0]);
    semi_axes(1) = sqrt(fact/solver.eigenvalues()[1]);
    Vector2d vec_0=solver.eigenvectors().col(0);
    Vector2d vec_1=solver.eigenvectors().col(1);

    center << (Bq*Eq - Cq*Dq)/detA33,
              (Dq*Bq - Aq*Eq)/detA33,
               0.0;

    // The center of the intersection ellipse in the original frame
    center = R*center + rp;

    axis1 << vec_0(0),
             vec_0(1),
             0.0;

    axis2 << vec_1(0),
             vec_1(1),
             0.0;

    // The axes of the intersection ellipse in the original frame
    axis1 = R*axis1;
    axis2 = R*axis2;

    return true;
}

std::ostream& operator<<(std::ostream& os, const Blob3D& b)
{
    b.printSelf(os);
    return os;
}

} // End namespace Geometry
} // End namspace SX

