/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon Eric Pellegrini
 Institut Laue-Langevin
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
#ifndef NSXTOOL_BASIS_H_
#define NSXTOOL_BASIS_H_

#include <iostream>
#include <memory>

#include <Eigen/Dense>

namespace SX
{

namespace Geometry
{

using Eigen::RowVector3d;
using Eigen::Vector3d;
using Eigen::Matrix3d;

/** @brief Class to define a Basis in 3D Euclidean space.
 *
 * Provide functionalities to define a Basis in 3-dimensional Euclidian space
 * from a set of 3 non-coplanar vectors. The Basis can be define with respect to a
 * reference basis, in which case a pointer to the reference is kept. If not, it is assumed
 * that the parent is the Standard orthonormal basis of a right-handed system.
 *
 */
class Basis {

typedef std::shared_ptr<Basis> ptrBasis;

public:
	Basis();
	//! Constructor from a set of three non coplanar vectors.
	//! These vectors will be used column-wised for building the transformation matrix from the reference basis to the current one.
	Basis(const Vector3d& a, const Vector3d& b, const Vector3d& c, ptrBasis reference=nullptr);
	//! Copy constructor.
	Basis(const Basis& other);
	//! Assignment operator.
	Basis& operator=(const Basis& other);
	//! Destructor.
	virtual ~Basis();
	//! Build a basis from a set of three direct vectors.
	static Basis fromDirectVectors(const Vector3d& a, const Vector3d& b, const Vector3d& c, ptrBasis reference=nullptr);
	//! Build a basis from a set of three reciprocal vectors.
	static Basis fromReciprocalVectors(const Vector3d& a, const Vector3d& b, const Vector3d& c, ptrBasis reference=nullptr);
	//! Returns the a basis vector.
	//Vector3d geta() const;
	//! Returns the a basis vector.
	double gete1Norm() const;
	double gete2Norm() const;
	double gete3Norm() const;
	double gete1e2Angle() const;
	double gete2e3Angle() const;
	double gete1e3Angle() const;
	//! Returns the current basis' metric tensor.
	Matrix3d getMetricTensor() const;
	//! Returns the current reciprocal basis' metric tensor.
	Matrix3d getReciprocalMetricTensor() const;
	//! Returns the direct volume fraction with respect to the reference volume.
	double getFractionalVolume() const;
	//! Returns the reciprocal volume fraction with respect to the reference volume.
	double getFractionalReciprocalVolume() const;
	//! Returns the direct volume.
	double getVolume() const;
	//! Returns the reciprocal volume.
	double getReciprocalVolume() const;
	//! Transform direct (contravariant) coordinates from the standard basis to the current one.
	//! The coordinates triplet v = (x,y,z)^T must be given as a column vector.
	Vector3d fromStandard(const Vector3d& v) const;
	//! Transform direct (contravariant) coordinates from the current basis to the standard one.
	//! The coordinates triplet v = (x,y,z)^T must be given as a column vector.
	Vector3d toStandard(const Vector3d& v) const;
	//! Transform direct (contravariant) coordinates from the reference basis to the current one.
	//! The coordinates triplet v = (x,y,z)^T must be given as a column vector.
	Vector3d fromReference(const Vector3d& v) const;
	//! Transform direct (contravariant) coordinates from the current basis to the reference one.
	//! The coordinates triplet v = (x,y,z)^T must be given as a column vector.
	Vector3d toReference(const Vector3d& v) const;

	//! Transform reciprocal (covariant) coordinates from the standard basis to the current one.
	//! The coordinates triplet rv = (h,k,l) can be given as a row vector or a column vector.
	RowVector3d fromReciprocalStandard(const RowVector3d& rv) const;
	//! Transform reciprocal (covariant) coordinates from the current basis to the standard one.
	//! The coordinates triplet rv = (h,k,l) can be given as a row vector or a column vector.
	RowVector3d toReciprocalStandard(const RowVector3d& rv) const;
	//! Transform reciprocal (covariant) coordinates from the reference basis to the current one.
	//! The coordinates triplet rv = (h,k,l) can be given as a row vector or a column vector.
	RowVector3d fromReciprocalReference(const RowVector3d& rv) const;
	//! Transform reciprocal (covariant) coordinates from the current basis to the reference one.
	//! The coordinates triplet rv = (h,k,l) can be given as a row vector or a column vector.
	RowVector3d toReciprocalReference(const RowVector3d& rv) const;

	//! Returns the transformation matrix that relates the current basis to other.
	Matrix3d getM(const Basis& other) const;
	//! Returns the transformation matrix that relates the current basis to the standard basis.
	Matrix3d getStandardM() const;
	//! Returns the transformation matrix that relates the current basis to the reference basis.
	const Matrix3d& getReferenceM() const;

	//! Returns the transformation matrix that relates the current basis to other.
	Matrix3d getReciprocalM(const Basis& other) const;
	//! Returns the transformation matrix that relates the current basis to the standard basis.
	Matrix3d getReciprocalStandardM() const;
	//! Returns the transformation matrix that relates the current basis to the reference basis.
	const Matrix3d& getReciprocalReferenceM() const;

	//! Rebase the current basis to the standard orthonormal basis by conjugating all intermediate basis.
	//! The reference basis is lost and set to nullptr.
	void rebaseToStandard();
	//! Rebase the current basis to a new one which becomes the new reference.
	void rebaseTo(std::shared_ptr<Basis> other);
	//! Transform the current Basis given a new transformation matrix M.
	//! M represents the components of new basis vectors with respect the old ones, given in columns.
	//! Reference is preserved, only A and B are recalculated.
	void transform(const Matrix3d& M);
	friend std::ostream& operator<<(std::ostream& os,const Basis& b);

protected:

	//! Returns true if three vectors are coplanar within a given tolerance.
	static bool coplanar(const Vector3d& v1, const Vector3d& v2, const Vector3d& v3, double tolerance=1.0e-6);
	//! The transformation matrix form the direct reference basis to the current one.
	Matrix3d _A;
	//! The transformation matrix form the reciprocal reference basis to the current one.
	Matrix3d _B;
	//! A shared pointer to the reference basis. If null assume that the reference is the standard basis.
	ptrBasis _reference;
};

} // end namespace Geometry

} // end namespace SX

#endif /* NSXTOOL_BASIS_H_ */
