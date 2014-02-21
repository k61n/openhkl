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
#include <initializer_list>

#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <Eigen/Geometry>
#include <unsupported/Eigen/MatrixFunctions>

#include "IPShape.h"

namespace SX
{

namespace Geometry
{


typedef unsigned int uint;

template<typename T, uint D>
class OBB : public IShape<T,D>
{

	//; Some useful typedefs;
	typedef Eigen::Matrix<T,D,D> matrix;
	typedef Eigen::Matrix<T,D,1> vector;
	typedef Eigen::Matrix<T,D+1,1> HomVector;
	typedef Eigen::Matrix<T,D+1,D+1> HomMatrix;

	//; Get rid of AABB resolution for protected attributes of AABB
	using AABB<T,D>::_lowerBound;
	using AABB<T,D>::_upperBound;

public:
	// Construct a N-dimensional box from its center, semi-axes, and eigenvectors ()
	OBB(const vector& center, const vector& eigenvalues, const matrix& eigenvectors);
	//; The destructor.
	~OBB();
	//; Check whether two OBBs collide.
	bool collide(const OBB& other) const;
	//; Return the extents of the OBB
	const vector& getSemiAxes() const;
	// Return the inverse of the Mapping matrix (\f$ S^{-1}.R^{-1}.T^{-1} \f$)
	const HomMatrix& getTRSInverseMatrix() const;
	//; Check whether a point given as Homogeneous coordinate in the (D+1) dimension is inside the OBB.
	bool isInside(const HomVector& vector) const;
	//; Scale isotropically the OBB.
	void scale(T value);
	//; Scale anisotropically the OBB.
	void scale(const vector& scale);
	//; Translate the OBB.
	void translate(const vector& t);


private:
	//; The inverse of the homogeneous transformation matrix.
	Eigen::Matrix<T,D+1,D+1> _TRSinv;
	//; The scale value.
	vector _eigenVal;
	//; Update the closest fit AABB to the OBB.
	void updateAABB();

public:
	// Macro to ensure that an OBB object can be dynamically allocated.
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW


};

// Collision detection in the 3D case.
template<typename T,uint D=3> bool collideOBBOBB(const OBB<T,3>&, const OBB<T,3>&);

template<typename T,uint D>
OBB<T,D>::OBB(const vector& center, const vector& eigenvalues, const matrix& eigenvectors)
: IShape<T,D>(), _eigenVal(eigenvalues)
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

template<typename T, uint D>
OBB<T,D>::~OBB()
{
}

template<typename T,uint D>
bool OBB<T,D>::collide(const OBB<T,D>& other) const
{
	return collideOBBOBB<T,D>(*this,other);
}

template<typename T,uint D>
const typename OBB<T,D>::vector& OBB<T,D>::getSemiAxes() const
{
	return _eigenVal;
}

template<typename T,uint D>
const typename OBB<T,D>::HomMatrix& OBB<T,D>::getTRSInverseMatrix() const
{
	return _TRSinv;
}

template<typename T, uint D>
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

template<typename T, uint D>
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

template<typename T,uint D>
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

template<typename T,uint D>
void OBB<T,D>::translate(const vector& t)
{
	Eigen::Matrix<T,D+1,D+1> tinv=Eigen::Matrix<T,D+1,D+1>::Constant(0.0);
	for (uint i=0;i<D+1;++i)
		tinv(i,i)=1.0;
	tinv.block(0,D,D,1)=-t;
	_TRSinv=_TRSinv*tinv;
	updateAABB();
}

template<typename T, uint D>
void OBB<T,D>::updateAABB()
{

	// Reconstruct S
	Eigen::DiagonalMatrix<T,D+1> S;
	for (unsigned int i=0;i<D;++i)
		S.diagonal()[i]=_eigenVal[i];
	S.diagonal()[D]=1.0;

	// Reconstruct R from TRinv
	HomMatrix TRinv=S*_TRSinv;
	matrix R(TRinv.block(0,0,D,D).transpose());

	// Extract T matrix from TRinv
	vector Tmat=-R*TRinv.block(0,D,D,1);

	// Calculate the width of the bounding box
	vector width=vector::Constant(0.0);
	for (uint i=0;i<D;++i)
	{
		for (uint j=0;j<D;++j)
			width[i]+=std::abs(_eigenVal[j]*R(j,i));
	}

	// Update the upper and lower bound of the AABB
	_lowerBound=Tmat-width;
	_upperBound=Tmat+width;

}

template<typename T,uint D=3> bool collideOBBOBB(const OBB<T,3>& a, const OBB<T,3>& b)
{

	// Get the (TRS)^-1 matrices of the two OBBs
	const Eigen::Matrix<T,4,4>& trsinva=a.getTRSInverseMatrix();
	const Eigen::Matrix<T,4,4>& trsinvb=b.getTRSInverseMatrix();

	// Get the extent of the two OBBs
	const Eigen::Matrix<T,3,1>& eiga=a.getSemiAxes();
	const Eigen::Matrix<T,3,1>& eigb=b.getSemiAxes();

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

	Eigen::Matrix<T,3,3> C=ra*rb;
	Eigen::Matrix<T,3,3> Cabs=C.array().abs();

	// The difference vector between the centers of OBB2 and OBB1
	Eigen::Matrix<T,3,1> diff=tb-ta;

	// If for one of the following 15 conditions, R<=(R0+R1) then the two OBBs collide.
	T R0, R, R1;

	// condition 1,2,3
	for (unsigned int i=0;i<D;++i)
	{
		R0=eiga[i];
		R1=(Cabs.block(i,0,1,D)*eigb)(0,0);
		R=std::abs((diff.transpose()*ra.block(0,i,D,1))(0,0));
		if (R<=R0+R1)
			return true;
	}

	// condition 4,5,6
	for (unsigned int i=0;i<D;++i)
	{
		R0=(Cabs.block(i,0,1,D)*eiga)(0,0);
		R1=eigb[i];
		R=std::abs((diff.transpose()*rb.block(0,i,D,1))(0,0));
		if (R<=R0+R1)
			return true;
	}

	T A0D((diff.transpose()*ra.block(0,0,D,1))(0,0));
	T A1D((diff.transpose()*ra.block(0,0,D,1))(0,0));
	T A2D((diff.transpose()*ra.block(0,0,D,1))(0,0));

	// condition 7
	R0=eiga[1]*Cabs(2,0)+eiga[2]*Cabs(1,0);
	R1=eigb[1]*Cabs(0,2)+eigb[2]*Cabs(0,1);
	R=std::abs(C(1,0)*A2D-C(2,0)*A1D);
	if (R<=R0+R1)
		return true;

	// condition 8
	R0=eiga[1]*Cabs(2,1)+eiga[2]*Cabs(1,1);
	R1=eigb[0]*Cabs(0,2)+eigb[2]*Cabs(0,0);
	R=std::abs(C(1,1)*A2D-C(2,1)*A1D);
	if (R<=R0+R1)
		return true;

	// condition 9
	R0=eiga[1]*Cabs(2,2)+eiga[2]*Cabs(1,2);
	R1=eigb[0]*Cabs(0,1)+eigb[1]*Cabs(0,0);
	R=std::abs(C(1,2)*A2D-C(2,2)*A1D);
	if (R<=R0+R1)
		return true;

	// condition 10
	R0=eiga[0]*Cabs(2,0)+eiga[2]*Cabs(0,0);
	R1=eigb[1]*Cabs(1,2)+eigb[2]*Cabs(1,1);
	R=std::abs(C(2,0)*A0D-C(0,0)*A2D);
	if (R<=R0+R1)
		return true;

	// condition 11
	R0=eiga[0]*Cabs(2,1)+eiga[2]*Cabs(0,1);
	R1=eigb[0]*Cabs(1,2)+eigb[2]*Cabs(1,0);
	R=std::abs(C(2,1)*A0D-C(0,1)*A2D);
	if (R<=R0+R1)
		return true;

	// condition 12
	R0=eiga[0]*Cabs(2,2)+eiga[2]*Cabs(0,2);
	R1=eigb[0]*Cabs(1,1)+eigb[1]*Cabs(1,0);
	R=std::abs(C(2,2)*A0D-C(0,2)*A2D);
	if (R<=R0+R1)
		return true;

	// condition 13
	R0=eiga[0]*Cabs(1,0)+eiga[1]*Cabs(0,0);
	R1=eigb[1]*Cabs(2,2)+eigb[2]*Cabs(2,1);
	R=std::abs(C(0,0)*A1D-C(1,0)*A0D);
	if (R<=R0+R1)
		return true;

	// condition 14
	R0=eiga[0]*Cabs(1,1)+eiga[1]*Cabs(0,1);
	R1=eigb[0]*Cabs(2,2)+eigb[2]*Cabs(2,0);
	R=std::abs(C(0,1)*A1D-C(1,1)*A0D);
	if (R<=R0+R1)
		return true;

	// condition 15
	R0=eiga[0]*Cabs(1,2)+eiga[1]*Cabs(0,2);
	R1=eigb[0]*Cabs(2,1)+eigb[1]*Cabs(2,0);
	R=std::abs(C(0,2)*A1D-C(1,2)*A0D);
	if (R<=R0+R1)
		return true;

	return false;
}

} // namespace Geometry

} // namespace SX

#endif // NSXTOOL_OBB_H_

