/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
    BP 156
    6, rue Jules Horowitz
    38042 Grenoble Cedex 9
    France
    chapon[at]ill.fr
    pellegrini[at]ill.fr

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef NSXTOOL_OBB_H_
#define NSXTOOL_OBB_H_

#include <cmath>
#include <iostream>
#include <initializer_list>

#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <Eigen/Geometry>
#include <unsupported/Eigen/MatrixFunctions>

#include "IShape.h"
#include "AABB.h"
#include "Ellipsoid.h"
#include "Sphere.h"
#include "../utils/Types.h"

namespace nsx {

template<typename T, nsx::Types::uint D>
class OBB : public IShape<T,D> {

public:

    //! Some useful typedefs;
    typedef Eigen::Matrix<T,D,D> matrix;
    typedef Eigen::Matrix<T,D,1> vector;
    typedef Eigen::Matrix<T,D+1,1> HomVector;
    typedef Eigen::Matrix<T,D+1,D+1> HomMatrix;

    // Get rid of IShape resolution for protected attributes of IShape
    using IShape<T,D>::_lowerBound;
    using IShape<T,D>::_upperBound;

    //! Default constructor
    OBB();
    //! Copy constructor
    OBB(const OBB<T,D>&);
    //! Construct a N-dimensional box from its center, semi-axes, and eigenvectors ()
    OBB(const vector& center, const vector& eigenvalues, const matrix& eigenvectors);
    //! Construct a OBB from a AABB
    OBB(const AABB<T,D>& aabb);
    //! Assignment
    OBB& operator=(const OBB<T,D>& other);
    //! Return a copy of this OBB
    IShape<T,D>* clone() const;
    //! The destructor.
    ~OBB();
    //! Return true if the OBB intersects any kind of shape.
    bool collide(const IShape<T,D>& other) const;
    //! Returns true if the OBB collides with a AABB.
    bool collide(const AABB<T,D>&) const;
    //! Returns true if the OBB collides with an Ellipsoid.
    bool collide(const Ellipsoid<T,D>&) const;
    //! Returns true if the OBB collides with an OBB.
    bool collide(const OBB<T,D>& other) const;
    //! Returns true if the OBB collides with a Sphere.
    bool collide(const Sphere<T,D>&) const;
    //! Return the extents of the OBB
    const vector& getExtents() const;
    //! Return the inverse of the Mapping matrix (\f$ S^{-1}.R^{-1}.T^{-1} \f$)
    const HomMatrix& getInverseTransformation() const;
    //! Check whether a point given as Homogeneous coordinate in the (D+1) dimension is inside the OBB.
    bool isInside(const HomVector& vector) const;
    //! Rotate the OBB.
    void rotate(const matrix& eigenvectors);
    //! Scale isotropically the OBB.
    void scale(T value);
    //! Scale anisotropically the OBB.
    void scale(const vector& scale);
    //! Translate the OBB.
    void translate(const vector& t);

    //! Compute the intersection between the OBB and a given ray.
    //! Return true if an intersection was found, false otherwise.
    //! If the return value is true the intersection "times" will be stored
    //! in t1 and t2 in such a way that from + t1*dir and from + t2*dir are
    //! the two intersection points between the ray and this shape.
    bool rayIntersect(const vector& from, const vector& dir, double& t1, double& t2) const;

private:
    //! The inverse of the homogeneous transformation matrix.
    Eigen::Matrix<T,D+1,D+1> _TRSinv;
    //! The scale value.
    vector _eigenVal;
    //! Update the closest fit AABB to the OBB.
    void updateAABB();

public:
    // Macro to ensure that an OBB object can be dynamically allocated.
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW


};

// Collision detection in the 3D case.
template<typename T,nsx::Types::uint D> bool collideOBBAABB(const OBB<T,D>&, const AABB<T,D>&);
template<typename T,nsx::Types::uint D> bool collideOBBEllipsoid(const OBB<T,D>&, const Ellipsoid<T,D>&);
template<typename T,nsx::Types::uint D> bool collideOBBOBB(const OBB<T,D>&, const OBB<T,D>&);
template<typename T,nsx::Types::uint D> bool collideOBBSphere(const OBB<T,D>&, const Sphere<T,D>&);

template<typename T,nsx::Types::uint D>
OBB<T,D>::OBB() : IShape<T,D>()
{
}

template<typename T,nsx::Types::uint D>
OBB<T,D>::OBB(const OBB<T,D>& other) : IShape<T,D>(other)
{
    _eigenVal=other._eigenVal;
    _TRSinv=other._TRSinv;
    updateAABB();
}

template<typename T,nsx::Types::uint D>
OBB<T,D>::OBB(const AABB<T,D>& aabb)
{
    _TRSinv=Eigen::Matrix<T,D+1,D+1>::Identity();
    _TRSinv.block(0,D,D,1)=-aabb.getAABBCenter();
    updateAABB();
}

template<typename T,nsx::Types::uint D>
OBB<T,D>& OBB<T,D>::operator=(const OBB<T,D>& other)
{
    if (this!=&other)
    {
        IShape<T,D>::operator=(other);
        _eigenVal=other._eigenVal;
        _TRSinv=other._TRSinv;
        updateAABB();
    }
    return *this;
}

template<typename T,nsx::Types::uint D>
OBB<T,D>::OBB(const vector& center, const vector& eigenvalues, const matrix& eigenvectors)
: IShape<T,D>(),
  _eigenVal(eigenvalues)
{
    // Define the inverse scale matrix from the eigenvalues
    Eigen::DiagonalMatrix<T,D+1> Sinv;
    for (unsigned int i=0;i<D;++i)
        Sinv.diagonal()[i]=1.0/eigenvalues[i];
    Sinv.diagonal()[D]=1.0;

    // Now prepare the R^-1.T^-1 (rotation,translation)
    _TRSinv=Eigen::Matrix<T,D+1,D+1>::Constant(0.0);
    _TRSinv(D,D)=1.0;
    for (unsigned int i=0;i<D;++i)
        _TRSinv.block(i,0,1,D)=eigenvectors.col(i).transpose().normalized();
    _TRSinv.block(0,D,D,1)=-_TRSinv.block(0,0,D,D)*center;

    // Finally compute (TRS)^-1 by left-multiplying (TR)^-1 by S^-1
    _TRSinv=Sinv*_TRSinv;
    updateAABB();
}

template<typename T,nsx::Types::uint D>
IShape<T,D>* OBB<T,D>::clone() const
{
    return new OBB<T,D>(*this);
}

template<typename T, nsx::Types::uint D>
OBB<T,D>::~OBB()
{
}

template<typename T,nsx::Types::uint D>
bool OBB<T,D>::collide(const IShape<T,D>& other) const
{
    if (this->intercept(other))
        return other.collide(*this);
    return false;
}

template<typename T,nsx::Types::uint D>
bool OBB<T,D>::collide(const AABB<T,D>& aabb) const
{
    return collideOBBAABB<T,D>(*this,aabb);
}

template<typename T,nsx::Types::uint D>
bool OBB<T,D>::collide(const Ellipsoid<T,D>& other) const
{
    return collideOBBEllipsoid<T,D>(*this,other);
}

template<typename T,nsx::Types::uint D>
bool OBB<T,D>::collide(const OBB<T,D>& other) const
{
    return collideOBBOBB<T,D>(*this,other);
}

template<typename T,nsx::Types::uint D>
bool OBB<T,D>::collide(const Sphere<T,D>& other) const
{
    return collideOBBSphere<T,D>(*this,other);
}

template<typename T,nsx::Types::uint D>
const typename OBB<T,D>::vector& OBB<T,D>::getExtents() const
{
    return _eigenVal;
}

template<typename T,nsx::Types::uint D>
const typename OBB<T,D>::HomMatrix& OBB<T,D>::getInverseTransformation() const
{
    return _TRSinv;
}

template<typename T, nsx::Types::uint D>
bool OBB<T,D>::isInside(const HomVector& point) const
{
    HomVector p=_TRSinv*point;

    for(unsigned int i=0; i<D; ++i)
    {
        if (p[i] < -1 || p[i] > 1)
            return false;
    }

    return true;
}

template<typename T, nsx::Types::uint D>
void OBB<T,D>::rotate(const matrix& eigenvectors)
{
    // Reconstruct S
    Eigen::DiagonalMatrix<T,D+1> S;
    for (unsigned int i=0;i<D;++i)
        S.diagonal()[i]=_eigenVal[i];
    S.diagonal()[D]=1.0;

    _TRSinv=S*_TRSinv;

    // Construct the inverse of the new rotation matrix
    HomMatrix Rnewinv=HomMatrix::Zero();
    Rnewinv(D,D) = 1.0;
    for (unsigned int i=0;i<D;++i)
        Rnewinv.block(i,0,1,D)=eigenvectors.col(i).transpose().normalized();
    _TRSinv=Rnewinv*_TRSinv;

    // Reconstruct Sinv
    for (unsigned int i=0;i<D;++i)
        S.diagonal()[i]=1.0/_eigenVal[i];
    S.diagonal()[D]=1.0;

    // Reconstruct the complete TRS inverse
    _TRSinv = S*_TRSinv;

    // Update the bounds of the AABB
    updateAABB();
}


template<typename T, nsx::Types::uint D>
void OBB<T,D>::scale(T value)
{
    _eigenVal*=value;
    Eigen::DiagonalMatrix<T,D+1> Sinv;
    for (unsigned int i=0;i<D;++i)
        Sinv.diagonal()[i]=1.0/value;
    Sinv.diagonal()[D]=1.0;
    _TRSinv=Sinv*_TRSinv;
    updateAABB();
}

template<typename T,nsx::Types::uint D>
void OBB<T,D>::scale(const vector& v)
{
    _eigenVal=_eigenVal.cwiseProduct(v);
    Eigen::DiagonalMatrix<T,D+1> Sinv;
    for (unsigned int i=0;i<D;++i)
        Sinv.diagonal()[i]=1.0/v[i];
    Sinv.diagonal()[D]=1.0;
    _TRSinv=Sinv*_TRSinv;
    updateAABB();
}

template<typename T,nsx::Types::uint D>
void OBB<T,D>::translate(const vector& t)
{
    HomMatrix tinv=HomMatrix::Zero();
    for (nsx::Types::uint i=0;i<D+1;++i)
        tinv(i,i)=1.0;
    tinv.block(0,D,D,1)=-t;
    _TRSinv=_TRSinv*tinv;
    updateAABB();
}

template<typename T, nsx::Types::uint D>
void OBB<T,D>::updateAABB()
{

    // Reconstruct S
    Eigen::DiagonalMatrix<T,D+1> S;
    for (unsigned int i=0;i<D;++i)
        S.diagonal()[i]=_eigenVal[i];
    S.diagonal()[D]=1.0;

    // Reconstruct R from TRinv
    HomMatrix TRinv=S*_TRSinv;
    matrix R=TRinv.block(0,0,D,D).transpose();

    // Extract T matrix from TRinv
    vector Tmat=-R*TRinv.block(0,D,D,1);

    // Calculate the width of the bounding box
    vector width=vector::Constant(0.0);
    for (nsx::Types::uint i=0;i<D;++i)
    {
        for (nsx::Types::uint j=0;j<D;++j)
            width[i]+=std::abs(_eigenVal[j]*R(j,i));
    }

    // Update the upper and lower bound of the AABB
    _lowerBound=Tmat-width;
    _upperBound=Tmat+width;

}

template<typename T, nsx::Types::uint D>
bool OBB<T,D>::rayIntersect(const vector& from, const vector& dir, double& t1, double& t2) const
{

    HomVector hFrom = _TRSinv * from.homogeneous();
    HomVector hDir;
    hDir.segment(0,D) = dir;
    hDir[D] = 0.0;
    hDir = _TRSinv*hDir;

    AABB<T,D> aabb(-vector::Ones(),vector::Ones());

    return aabb.rayIntersect(hFrom.segment(0,D),hDir.segment(0,D),t1,t2);

    return true;

}

template<typename T,nsx::Types::uint D>
bool collideOBBAABB(const OBB<T,D>& obb, const AABB<T,D>& aabb)
{
    OBB<T,D> obb1(aabb);
    return collideOBBOBB(obb,obb1);
}

template<typename T,nsx::Types::uint D=2>
bool collideOBBOBB(const OBB<T,2>& a, const OBB<T,2>& b)
{

    // Get the (TRS)^-1 matrices of the two OBBs
    const Eigen::Matrix<T,3,3>& trsinva=a.getInverseTransformation();
    const Eigen::Matrix<T,3,3>& trsinvb=b.getInverseTransformation();

    // Get the extent of the two OBBs
    const Eigen::Matrix<T,2,1>& eiga=a.getExtents();
    const Eigen::Matrix<T,2,1>& eigb=b.getExtents();

    // Reconstruct the S matrices for the two OBBs
    Eigen::DiagonalMatrix<T,3> sa;
    Eigen::DiagonalMatrix<T,3> sb;
    sa.diagonal() << eiga[0], eiga[1],1;
    sb.diagonal() << eigb[0], eigb[1],1;

    // Reconstruct the (TR)^-1 matrices for the two OBBs
    const Eigen::Matrix<T,3,3> trinva(sa*trsinva);
    const Eigen::Matrix<T,3,3> trinvb(sb*trsinvb);

    // Reconstruct R for the two OBBs
    Eigen::Matrix<T,2,2> ra(trinva.block(0,0,D,D).transpose());
    Eigen::Matrix<T,2,2> rb(trinvb.block(0,0,D,D).transpose());

    // Extract T matrix from TRinv
    Eigen::Matrix<T,2,1> ta=-ra*trinva.block(0,D,D,1);
    Eigen::Matrix<T,2,1> tb=-rb*trinvb.block(0,D,D,1);

    Eigen::Matrix<T,2,2> C=ra.transpose()*rb;
    Eigen::Matrix<T,2,2> Cabs=C.array().abs();

    // The difference vector between the centers of OBB2 and OBB1
    Eigen::Matrix<T,1,2> diff=(tb-ta).transpose();

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
    for (unsigned int i=0;i<D;++i)
    {
        R0=(Cabs.block(i,0,1,D)*eiga)(0,0);
        R1=eigb[i];
        R=std::abs((diff*rb.block(0,i,D,1))(0,0));
        if (R>(R0+R1))
            return false;
    }

    return true;
}

/** Based on the method described in:
 *  "Dynamic Collision Detection using Oriented Bounding Boxes"
 *	Eberly, David.,
 *	Geometric Tools, LLC
 *	http://www.geometrictools.com
 */
template<typename T,nsx::Types::uint D=3>
bool collideOBBOBB(const OBB<T,3>& a, const OBB<T,3>& b)
{

    // Get the (TRS)^-1 matrices of the two OBBs
    const Eigen::Matrix<T,4,4>& trsinva=a.getInverseTransformation();
    const Eigen::Matrix<T,4,4>& trsinvb=b.getInverseTransformation();

    // Get the extent of the two OBBs
    const Eigen::Matrix<T,3,1>& eiga=a.getExtents();
    const Eigen::Matrix<T,3,1>& eigb=b.getExtents();

    // Reconstruct the S matrices for the two OBBs
    Eigen::DiagonalMatrix<T,4> sa;
    Eigen::DiagonalMatrix<T,4> sb;
    sa.diagonal() << eiga[0], eiga[1],eiga[2],1;
    sb.diagonal() << eigb[0], eigb[1],eigb[2],1;

    // Reconstruct the (TR)^-1 matrices for the two OBBs
    const Eigen::Matrix<T,4,4> trinva(sa*trsinva);
    const Eigen::Matrix<T,4,4> trinvb(sb*trsinvb);

    // Reconstruct R for the two OBBs
    Eigen::Matrix<T,3,3> ra(trinva.block(0,0,D,D).transpose());
    Eigen::Matrix<T,3,3> rb(trinvb.block(0,0,D,D).transpose());

    // Extract T matrix from TRinv
    Eigen::Matrix<T,3,1> ta=-ra*trinva.block(0,D,D,1);
    Eigen::Matrix<T,3,1> tb=-rb*trinvb.block(0,D,D,1);

    Eigen::Matrix<T,3,3> C=ra.transpose()*rb;
    Eigen::Matrix<T,3,3> Cabs=C.array().abs();

    // The difference vector between the centers of OBB2 and OBB1
    Eigen::Matrix<T,1,3> diff=(tb-ta).transpose();

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
    for (unsigned int i=0;i<D;++i)
    {
        R0=(Cabs.block(i,0,1,D)*eiga)(0,0);
        R1=eigb[i];
        R=std::abs((diff*rb.block(0,i,D,1))(0,0));
        if (R>(R0+R1))
            return false;
    }

    T A0D((diff*ra.block(0,0,D,1))(0,0));
    T A1D((diff*ra.block(0,1,D,1))(0,0));
    T A2D((diff*ra.block(0,2,D,1))(0,0));

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

template<typename T,nsx::Types::uint D>
bool collideOBBEllipsoid(const OBB<T,D>& obb, const Ellipsoid<T,D>& ell)
{
    return collideEllipsoidOBB(ell,obb);
}

template<typename T,nsx::Types::uint D>
bool collideOBBSphere(const OBB<T,D>& obb, const Sphere<T,D>& s)
{
    return collideSphereOBB(s,obb);
}

} // end namespace nsx

#endif // NSXTOOL_OBB_H_

