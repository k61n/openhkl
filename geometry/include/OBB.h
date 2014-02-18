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

	//; Get rid of AABB resolution for protected attributes of AABB
	using AABB<T,D>::_lowerBound;
	using AABB<T,D>::_upperBound;

public:
	// Construct a N-dimensional box from its center, semi-axes, and eigenvectors ()
	OBB(const vector& center, const vector& eigenvalues, const matrix& eigenvectors);
	//; The destructor.
	~OBB();
	//; Check whether a point given as Homogeneous coordinate in the (D+1) dimension is inside the OBB.
	bool isInside(const HomVector& vector) const;
	//; Scale isotropically the OBB.
	void scale(T value);
	//; Scale anisotropically the OBB.
	//void scale(const vector& scale);
	//; Translate the OBB.
	//void translate(const vector& t);

private:

	Eigen::Matrix<T,D+1,D+1> _TRSinv;
	//; EigenValues.
	vector _eigenVal;

	//; Translation.
	vector _center;

	// Method to update the closest fit AABB to the OBB
	void updateAABB();

};

template<typename T,uint D>
OBB<T,D>::OBB(const vector& center, const vector& eigenvalues, const matrix& eigenvectors)
: IShape<T,D>(), _eigenVal(eigenvalues), _center(center)
{
	// Define the inverse scale matrix from the eigenvalues
	Eigen::DiagonalMatrix<T,D+1> Sinv;
	for (unsigned int i=0;i<D;++i)
		Sinv.diagonal()[i]=1.0/eigenvalues[i];
	Sinv.diagonal()[D]=1.0;

	_TRSinv(D,D)=1.0;
	// Now prepare the R^-1.T^-1 (rotation,translation)
	_TRSinv.block(0,D,D,1)=-center;
	for (unsigned int i=0;i<D;++i)
	{
		_TRSinv(D,i)=0.0;
		_TRSinv.block(i,0,1,D)=eigenvectors.col(i).transpose().normalized();
	}
	_TRSinv=Sinv*_TRSinv;
	updateAABB();
}

template<typename T, uint D>
OBB<T,D>::~OBB()
{
}

template<typename T, uint D>
bool OBB<T,D>::isInside(const HomVector& vector) const
{
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
	scaleAABB(value);
}

template<typename T, uint D>
void OBB<T,D>::updateAABB()
{

	// Reconstruct S
	Eigen::DiagonalMatrix<T,D+1> S;
	for (unsigned int i=0;i<D;++i)
		S.diagonal()[i]=_eigenVal[i];
	S.diagonal()[D]=1.0;

	// Reconstruct T
	Eigen::Matrix<T,D+1,D+1> Tmat;
	Tmat.setZero();
	for (uint i=0;i<D+1;++i)
		Tmat(i,i)=1.0;
	Tmat.block(0,D,D,1)=_center;

	// Reconstruct the R^{-1} matrix to obtain the eigenvectors and center
	Eigen::Matrix<T,D+1,D+1> Rinv=S*_TRSinv*Tmat;

	// Calculate the width of the bounding box
	vector width=vector::Constant(0.0);
	for (uint i=0;i<D;++i)
	{
		for (uint j=0;j<D;++j)
		{
			width[i]+=std::abs(_eigenVal[j]*Rinv(j,i));
		}
	}

	// Update the upper and lower bound of the AABB
	_lowerBound=Tmat.block(0,D,D,1)-width;
	_upperBound=Tmat.block(0,D,D,1)+width;

}

} // namespace Geometry

} // namespace SX

#endif // NSXTOOL_NDBOX_H_

