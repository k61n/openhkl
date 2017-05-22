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

#ifndef NSXLIB_BASIS_H
#define NSXLIB_BASIS_H

#include <iostream>

#include <Eigen/Dense>

#include "../geometry/GeometryTypes.h"

namespace nsx {

/** @brief Class to define a Basis in 3D Euclidean space.
 *
 * Provide functionalities to define a Basis in 3-dimensional Euclidian space
 * from a set of 3 non-coplanar vectors. The Basis can be define with respect to a
 * reference basis, in which case a pointer to the reference is kept. If not, it is assumed
 * that the parent is the Standard orthonormal basis of a right-handed system.
 *
 */

class Basis {

public:

	Basis();
	//! Constructor from a set of three non coplanar vectors.
	//! These vectors will be used column-wised for building the transformation matrix from the reference basis to the current one.
	Basis(const Eigen::Vector3d& a, const Eigen::Vector3d& b, const Eigen::Vector3d& c, sptrBasis reference=nullptr);
	//! Copy constructor.
	Basis(const Basis& other);
	//! Assignment operator.
	Basis& operator=(const Basis& other);
	//! Destructor.
	virtual ~Basis();
	//! Build a basis from a set of three direct vectors.
	static Basis fromDirectVectors(const Eigen::Vector3d& a, const Eigen::Vector3d& b, const Eigen::Vector3d& c, sptrBasis reference=nullptr);
	//! Build a basis from a set of three reciprocal vectors.
	static Basis fromReciprocalVectors(const Eigen::Vector3d& a, const Eigen::Vector3d& b, const Eigen::Vector3d& c, sptrBasis reference=nullptr);
	//! Returns the norm of e1 basis vector.
	void setDirectVectors(const Eigen::Vector3d& a, const Eigen::Vector3d& b, const Eigen::Vector3d& c);
	void setReciprocalVectors(const Eigen::Vector3d& a, const Eigen::Vector3d& b, const Eigen::Vector3d& c);
	double getA() const;
	//! Returns the norm of e2 basis vector.
	double getB() const;
	//! Returns the norm of e3 basis vector.
	double getC() const;
	//! Returns the angle between e1 and e2 basis vectors (radian)
	double getAlpha() const;
	//! Returns the angle between e1 and e3 basis vectors (radian)
	double getBeta() const;
	//! Returns the angle between e2 and e3 basis vectors (radian)
	double getGamma() const;
	//! Returns the norm of e1 basis vector.
	double getReciprocalA() const;
	//! Returns the norm of e2 basis vector.
	double getReciprocalB() const;
	//! Returns the norm of e3 basis vector.
	double getReciprocalC() const;
	//! Returns the angle between e1 and e2 basis vectors (radian)
	double getReciprocalAlpha() const;
	//! Returns the angle between e1 and e3 basis vectors (radian)
	double getReciprocalBeta() const;
	//! Returns the angle between e2 and e3 basis vectors (radian)
	double getReciprocalGamma() const;
	//! Get the direct basis parameters
	void getParameters(double& a,double& b ,double& c,double& alpha, double& beta, double& gamma) const;
	//! Get the errors on direct basis parameters
	void getParametersSigmas(double& sa,double& sb ,double& sc,double& salpha, double& sbeta, double& sgamma) const;
	//! Get the errors on reciprocal basis parameters
	void getReciprocalParametersSigmas(double& sas,double& sbs ,double& scs,double& salphas, double& sbetas, double& sgammas) const;
	//! Returns the current basis' metric tensor.
	Eigen::Matrix3d getMetricTensor() const;
	//! Returns the current reciprocal basis' metric tensor.
	Eigen::Matrix3d getReciprocalMetricTensor() const;
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
	Eigen::Vector3d fromStandard(const Eigen::Vector3d& v) const;
	//! Transform direct (contravariant) coordinates from the current basis to the standard one.
	//! The coordinates triplet v = (x,y,z)^T must be given as a column vector.
	Eigen::Vector3d toStandard(const Eigen::Vector3d& v) const;
	//! Transform direct (contravariant) coordinates from the reference basis to the current one.
	//! The coordinates triplet v = (x,y,z)^T must be given as a column vector.
	Eigen::Vector3d fromReference(const Eigen::Vector3d& v) const;
	//! Transform direct (contravariant) coordinates from the current basis to the reference one.
	//! The coordinates triplet v = (x,y,z)^T must be given as a column vector.
	Eigen::Vector3d toReference(const Eigen::Vector3d& v) const;

	//! Transform reciprocal (covariant) coordinates from the standard basis to the current one.
	//! The coordinates triplet rv = (h,k,l) can be given as a row vector or a column vector.
	Eigen::RowVector3d fromReciprocalStandard(const Eigen::RowVector3d& rv) const;
	//! Transform reciprocal (covariant) coordinates from the current basis to the standard one.
	//! The coordinates triplet rv = (h,k,l) can be given as a row vector or a column vector.
	Eigen::RowVector3d toReciprocalStandard(const Eigen::RowVector3d& rv) const;
	//! Transform reciprocal (covariant) coordinates from the reference basis to the current one.
	//! The coordinates triplet rv = (h,k,l) can be given as a row vector or a column vector.
	Eigen::RowVector3d fromReciprocalReference(const Eigen::RowVector3d& rv) const;
	//! Transform reciprocal (covariant) coordinates from the current basis to the reference one.
	//! The coordinates triplet rv = (h,k,l) can be given as a row vector or a column vector.
	Eigen::RowVector3d toReciprocalReference(const Eigen::RowVector3d& rv) const;

	//! Returns the transformation matrix that relates the current basis to other.
	Eigen::Matrix3d getM(const Basis& other) const;
	//! Returns the transformation matrix that relates the current basis to the standard basis.
	Eigen::Matrix3d getStandardM() const;
	//! Returns the transformation matrix that relates the current basis to the reference basis.
	const Eigen::Matrix3d& getReferenceM() const;

	//! Returns the transformation matrix that relates the current basis to other.
	Eigen::Matrix3d getReciprocalM(const Basis& other) const;
	//! Returns the transformation matrix that relates the current basis to the standard basis.
	Eigen::Matrix3d getReciprocalStandardM() const;
	//! Returns the transformation matrix that relates the current basis to the reference basis.
	const Eigen::Matrix3d& getReciprocalReferenceM() const;

	//! Rebase the current basis to the standard orthonormal basis by conjugating all intermediate basis.
	//! The reference basis is lost and set to nullptr.
	void rebaseToStandard();
	//! Rebase the current basis to a new one which becomes the new reference.
	void rebaseTo(sptrBasis other,bool sigmasFromReference=false);
	//! Transform the current Basis given a new transformation matrix M.
	//! M represents the components of new basis vectors with respect the old ones, given in columns.
	//! Reference is preserved, only A and B are recalculated.
	void transform(const Eigen::Matrix3d& M);

	//! Set the errors on the A matrix, assume that
	void setDirectSigmas(const Eigen::Vector3d& sa,const Eigen::Vector3d& sb,const Eigen::Vector3d& sc);
	//! Set the errors on the B matrix
	void setReciprocalSigmas(const Eigen::Vector3d& sas,const Eigen::Vector3d& sbs,const Eigen::Vector3d& scs);
	//! Set the errors on the A matrix
	void setDirectSigmas(const Eigen::Matrix3d& sigmas);
	//! Set the errors on the B matrix
	void setReciprocalSigmas(const Eigen::Matrix3d& sigmas);
	//! Set the errors on the A matrix
	void setDirectCovariance(const covMat& sigmas);
	//! Set the errors on the B matrix
	void setReciprocalCovariance(const covMat& sigmas);
	//! Get direct covariance matrix
	const covMat& getDirectCovariance();
	//! Get reciprocal covariance matrix
	const covMat& getReciprocalCovariance();

	//! Return whether or not some erros have been assigned on basis (direct & reciprocal) parameters.
	bool hasSigmas() const;

	//! Stream output operator.
	friend std::ostream& operator<<(std::ostream& os,const Basis& b);

protected:

	//! Computes the direct (direction=false) or reciprocal (direction=true) covariance matrix
	void calculateSigmasDirectToReciprocal(bool direction=true);

	//! Propagates the errors on direct and reciprocal covariance matrices under a basis transformation
	void propagateSigmas(const Eigen::Matrix3d& M);

	//! Returns true if three vectors are coplanar within a given tolerance.
	static bool coplanar(const Eigen::Vector3d& v1, const Eigen::Vector3d& v2, const Eigen::Vector3d& v3, double tolerance=1.0e-6);
	//! The transformation matrix form the direct reference basis to the current one.
	Eigen::Matrix3d _A;
	//! The transformation matrix form the reciprocal reference basis to the current one.
	Eigen::Matrix3d _B;
	//! A shared pointer to the reference basis. If null assume that the reference is the standard basis.
	sptrBasis _reference;
	//! Stores the errors on A matrix parameters
	covMat _Acov;
	//! Stores the errors on B matrix parameters
	covMat _Bcov;
	//! Define whtehr or not errors have been defined for the basis.
	bool _hasSigmas;

};

} // end namespace nsx

#endif // NSXLIB_BASIS_H
