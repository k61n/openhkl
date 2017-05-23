#include "OBB.h"

namespace nsx {

OBB::OBB() : IShape()
{
}

OBB::OBB(const OBB& other) : IShape(other)
{
    _eigenVal=other._eigenVal;
    _TRSinv=other._TRSinv;
    updateAABB();
}

OBB::OBB(const AABB& aabb)
{
    _TRSinv=HomMatrix::Identity();
    _TRSinv.block(0,3,3,1)=-aabb.getAABBCenter();
    updateAABB();
}

OBB& OBB::operator=(const OBB& other)
{
    if (this!=&other)
    {
        IShape::operator=(other);
        _eigenVal=other._eigenVal;
        _TRSinv=other._TRSinv;
        updateAABB();
    }
    return *this;
}

OBB::OBB(const Eigen::Vector3d& center, const Eigen::Vector3d& eigenvalues, const Eigen::Matrix3d& eigenvectors)
: IShape(),
  _eigenVal(eigenvalues)
{
    // Define the inverse scale matrix from the eigenvalues
    Eigen::DiagonalMatrix<double,4> Sinv;
    for (unsigned int i=0;i<3;++i)
        Sinv.diagonal()[i]=1.0/eigenvalues[i];
    Sinv.diagonal()[3]=1.0;

    // Now prepare the R^-1.T^-1 (rotation,translation)
    _TRSinv=HomMatrix::Constant(0.0);
    _TRSinv(3,3)=1.0;
    for (unsigned int i=0;i<3;++i)
        _TRSinv.block(i,0,1,3)=eigenvectors.col(i).transpose().normalized();
    _TRSinv.block(0,3,3,1)=-_TRSinv.block(0,0,3,3)*center;

    // Finally compute (TRS)^-1 by left-multiplying (TR)^-1 by S^-1
    _TRSinv=Sinv*_TRSinv;
    updateAABB();
}

IShape* OBB::clone() const
{
    return new OBB(*this);
}

bool OBB::collide(const IShape& other) const
{
    if (this->intercept(other))
        return other.collide(*this);
    return false;
}

bool OBB::collide(const AABB& aabb) const
{
    return collideOBBAABB(*this,aabb);
}

bool OBB::collide(const Ellipsoid& other) const
{
    return collideOBBEllipsoid(*this,other);
}

bool OBB::collide(const OBB& other) const
{
    return collideOBBOBB(*this,other);
}

bool OBB::collide(const Sphere& other) const
{
    return collideOBBSphere(*this,other);
}

const Eigen::Vector3d& OBB::getExtents() const
{
    return _eigenVal;
}

const HomMatrix& OBB::getInverseTransformation() const
{
    return _TRSinv;
}

bool OBB::isInside(const HomVector& point) const
{
    HomVector p=_TRSinv*point;

    for(unsigned int i=0; i<3; ++i)
    {
        if (p[i] < -1 || p[i] > 1)
            return false;
    }

    return true;
}

void OBB::rotate(const Eigen::Matrix3d& eigenvectors)
{
    // Reconstruct S
    Eigen::DiagonalMatrix<double,4> S;
    for (unsigned int i=0;i<3;++i)
        S.diagonal()[i]=_eigenVal[i];
    S.diagonal()[3]=1.0;

    _TRSinv=S*_TRSinv;

    // Construct the inverse of the new rotation matrix
    HomMatrix Rnewinv=HomMatrix::Zero();
    Rnewinv(3,3) = 1.0;
    for (unsigned int i=0;i<3;++i)
        Rnewinv.block(i,0,1,3)=eigenvectors.col(i).transpose().normalized();
    _TRSinv=Rnewinv*_TRSinv;

    // Reconstruct Sinv
    for (unsigned int i=0;i<3;++i)
        S.diagonal()[i]=1.0/_eigenVal[i];
    S.diagonal()[3]=1.0;

    // Reconstruct the complete TRS inverse
    _TRSinv = S*_TRSinv;

    // Update the bounds of the AABB
    updateAABB();
}


void OBB::scale(T value)
{
    _eigenVal*=value;
    Eigen::DiagonalMatrix<double,4> Sinv;
    for (unsigned int i=0;i<3;++i)
        Sinv.diagonal()[i]=1.0/value;
    Sinv.diagonal()[3]=1.0;
    _TRSinv=Sinv*_TRSinv;
    updateAABB();
}

void OBB::scale(const Eigen::Vector3d& v)
{
    _eigenVal=_eigenVal.cwiseProduct(v);
    Eigen::DiagonalMatrix<double,4> Sinv;
    for (unsigned int i=0;i<3;++i)
        Sinv.diagonal()[i]=1.0/v[i];
    Sinv.diagonal()[3]=1.0;
    _TRSinv=Sinv*_TRSinv;
    updateAABB();
}

void OBB::translate(const Eigen::Vector3d& t)
{
    HomMatrix tinv=HomMatrix::Zero();
    for (unsigned int i=0;i<3+1;++i)
        tinv(i,i)=1.0;
    tinv.block(0,3,3,1)=-t;
    _TRSinv=_TRSinv*tinv;
    updateAABB();
}

void OBB::updateAABB()
{

    // Reconstruct S
    Eigen::DiagonalMatrix<double,4> S;
    for (unsigned int i=0;i<3;++i)
        S.diagonal()[i]=_eigenVal[i];
    S.diagonal()[3]=1.0;

    // Reconstruct R from TRinv
    HomMatrix TRinv=S*_TRSinv;
    Eigen::Matrix3d R=TRinv.block(0,0,3,3).transpose();

    // Extract T matrix from TRinv
    Eigen::Vector3d Tmat=-R*TRinv.block(0,3,3,1);

    // Calculate the width of the bounding box
    Eigen::Vector3d width=Eigen::Vector3d::Constant(0.0);
    for (unsigned int i=0;i<3;++i)
    {
        for (unsigned int j=0;j<3;++j)
            width[i]+=std::abs(_eigenVal[j]*R(j,i));
    }

    // Update the upper and lower bound of the AABB
    _lowerBound=Tmat-width;
    _upperBound=Tmat+width;

}

bool OBB::rayIntersect(const Eigen::Vector3d& from, const Eigen::Vector3d& dir, double& t1, double& t2) const
{

    HomVector hFrom = _TRSinv * from.homogeneous();
    HomVector hDir;
    hDir.segment(0,D) = dir;
    hDir[D] = 0.0;
    hDir = _TRSinv*hDir;

    AABB aabb(-Eigen::Vector3d::Ones(),Eigen::Vector3d::Ones());

    return aabb.rayIntersect(hFrom.segment(0,3),hDir.segment(0,3),t1,t2);

    return true;

}

bool collideOBBAABB(const OBB& obb, const AABB& aabb)
{
    OBB obb1(aabb);
    return collideOBBOBB(obb,obb1);
}

/** Based on the method described in:
 *  "Dynamic Collision Detection using Oriented Bounding Boxes"
 *	Eberly, David.,
 *	Geometric Tools, LLC
 *	http://www.geometrictools.com
 */
bool collideOBBOBB(const OBB& a, const OBB& b)
{

    // Get the (TRS)^-1 matrices of the two OBBs
    const HomMatrix& trsinva=a.getInverseTransformation();
    const HomMatrix& trsinvb=b.getInverseTransformation();

    // Get the extent of the two OBBs
    const Eigen::Vector3d& eiga=a.getExtents();
    const Eigen::Vector3d& eigb=b.getExtents();

    // Reconstruct the S matrices for the two OBBs
    Eigen::DiagonalMatrix<double,4> sa;
    Eigen::DiagonalMatrix<double,4> sb;
    sa.diagonal() << eiga[0], eiga[1],eiga[2],1;
    sb.diagonal() << eigb[0], eigb[1],eigb[2],1;

    // Reconstruct the (TR)^-1 matrices for the two OBBs
    const HomMatrix trinva(sa*trsinva);
    const HomMatrix trinvb(sb*trsinvb);

    // Reconstruct R for the two OBBs
    Eigen::Matrix3d ra(trinva.block(0,0,D,D).transpose());
    Eigen::Matrix3d rb(trinvb.block(0,0,D,D).transpose());

    // Extract T matrix from TRinv
    Eigen::Matrix3d ta=-ra*trinva.block(0,D,D,1);
    Eigen::Matrix3d tb=-rb*trinvb.block(0,D,D,1);

    Eigen::Matrix3d C=ra.transpose()*rb;
    Eigen::Matrix3d Cabs=C.array().abs();

    // The difference vector between the centers of OBB2 and OBB1
    Eigen::Matrix<double,1,3> diff=(tb-ta).transpose();

    // If for one of the following 15 conditions, R<=(R0+R1) then the two OBBs collide.
    T R0, R, R1;

    // condition 1,2,3
    for (unsigned int i=0;i<D;++i)
    {
        R0=eiga[i];
        R1=(Cabs.block(i,0,1,D)*eigb)(0,0);
        R=std::abs((diff*ra.block(0,i,D,1))(0,0));
        if (R>(R0+R1))
            return false;
    }

    // condition 4,5,6
    for (unsigned int i=0;i<3;++i)
    {
        R0=(Cabs.block(i,0,1,3)*eiga)(0,0);
        R1=eigb[i];
        R=std::abs((diff*rb.block(0,i,3,1))(0,0));
        if (R>(R0+R1))
            return false;
    }

    double A0D((diff*ra.block(0,0,3,1))(0,0));
    double A1D((diff*ra.block(0,1,3,1))(0,0));
    double A2D((diff*ra.block(0,2,3,1))(0,0));

    // condition 7
    R0=eiga[1]*Cabs(2,0)+eiga[2]*Cabs(1,0);
    R1=eigb[1]*Cabs(0,2)+eigb[2]*Cabs(0,1);
    R=std::abs(C(1,0)*A2D-C(2,0)*A1D);
    if (R>(R0+R1))
        return false;

    // condition 8
    R0=eiga[1]*Cabs(2,1)+eiga[2]*Cabs(1,1);
    R1=eigb[0]*Cabs(0,2)+eigb[2]*Cabs(0,0);
    R=std::abs(C(1,1)*A2D-C(2,1)*A1D);
    if (R>(R0+R1))
        return false;

    // condition 9
    R0=eiga[1]*Cabs(2,2)+eiga[2]*Cabs(1,2);
    R1=eigb[0]*Cabs(0,1)+eigb[1]*Cabs(0,0);
    R=std::abs(C(1,2)*A2D-C(2,2)*A1D);
    if (R>(R0+R1))
        return false;

    // condition 10
    R0=eiga[0]*Cabs(2,0)+eiga[2]*Cabs(0,0);
    R1=eigb[1]*Cabs(1,2)+eigb[2]*Cabs(1,1);
    R=std::abs(C(2,0)*A0D-C(0,0)*A2D);
    if (R>(R0+R1))
        return false;

    // condition 11
    R0=eiga[0]*Cabs(2,1)+eiga[2]*Cabs(0,1);
    R1=eigb[0]*Cabs(1,2)+eigb[2]*Cabs(1,0);
    R=std::abs(C(2,1)*A0D-C(0,1)*A2D);
    if (R>(R0+R1))
        return false;

    // condition 12
    R0=eiga[0]*Cabs(2,2)+eiga[2]*Cabs(0,2);
    R1=eigb[0]*Cabs(1,1)+eigb[1]*Cabs(1,0);
    R=std::abs(C(2,2)*A0D-C(0,2)*A2D);
    if (R>(R0+R1))
        return false;

    // condition 13
    R0=eiga[0]*Cabs(1,0)+eiga[1]*Cabs(0,0);
    R1=eigb[1]*Cabs(2,2)+eigb[2]*Cabs(2,1);
    R=std::abs(C(0,0)*A1D-C(1,0)*A0D);
    if (R>(R0+R1))
        return false;

    // condition 14
    R0=eiga[0]*Cabs(1,1)+eiga[1]*Cabs(0,1);
    R1=eigb[0]*Cabs(2,2)+eigb[2]*Cabs(2,0);
    R=std::abs(C(0,1)*A1D-C(1,1)*A0D);
    if (R>(R0+R1))
        return false;

    // condition 15
    R0=eiga[0]*Cabs(1,2)+eiga[1]*Cabs(0,2);
    R1=eigb[0]*Cabs(2,1)+eigb[1]*Cabs(2,0);
    R=std::abs(C(0,2)*A1D-C(1,2)*A0D);
    if (R>(R0+R1))
        return false;

    return true;
}

bool collideOBBEllipsoid(const OBB& obb, const Ellipsoid& ell)
{
    return collideEllipsoidOBB(ell,obb);
}

bool collideOBBSphere(const OBB& obb, const Sphere& s)
{
    return collideSphereOBB(s,obb);
}

} // end namespace nsx

