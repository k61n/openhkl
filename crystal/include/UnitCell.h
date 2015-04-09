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

#ifndef NSXTOOL_UNITCELL_H_
#define NSXTOOL_UNITCELL_H_

#include <vector>
#include <string>

#include <Eigen/Dense>

#include "Basis.h"
#include "IMaterial.h"
#include "Peak3D.h"

namespace SX
{

namespace Crystal
{

using Eigen::Vector3d;



//! Centering type of the Bravais lattice
enum  class LatticeCentring : char  {P='P',A='A',B='B',C='C',I='I',F='F',R='R'};
//! Bravais type
enum  class BravaisType : char  {Triclinic='a',Monolinic='m',Orthorhombic='o',Tetragonal='t',Hexagonal='h',Cubic='c'};

/** @brief Class to define a crystallographic unit-cell.
 *
 * Provide functionalities to transform vectors of the direct lattice or reciprocal
 * lattice from unit-cell coordinates to a right-handed coordinates system.
 * The UnitCell is defined by the parameters a,b,c and angles alpha, beta, gamma.
 * The UnitCell parameters a,b,c are given in \f$ \AA \f$  and angle alpha, beta, gamma
 * are given in degrees.
 * The a axis is chosen as pointing along the x-direction, and the b-axis is in the xy-plane.
 */
class UnitCell : public SX::Geometry::Basis
{
public:
	//! Empty UnitCell, initialiazed to right-handed orthonormal system
	UnitCell();
	//! Construct unitCell from lattice parameters, the A matrix is built with avector along x, bvector in the xy-plane.
	UnitCell(double a, double b, double c, double alpha, double beta, double gamma,LatticeCentring centring=LatticeCentring::P, BravaisType bravais=BravaisType::Triclinic, std::shared_ptr<SX::Geometry::Basis> reference=nullptr);
	//! Build Unit-cell from 3 non-coplanar vectors. Throw if coplanars.
	UnitCell(const Vector3d& v1,const Vector3d& v2,const Vector3d& v3, LatticeCentring centring=LatticeCentring::P, BravaisType bravais=BravaisType::Triclinic,std::shared_ptr<SX::Geometry::Basis> reference=nullptr );
	//! Copy constructor
	UnitCell(const UnitCell&);
	//! Assignment
	UnitCell& operator=(const UnitCell&);
	~UnitCell();
	//! Set lattice parameters
	void setParams(double a, double b, double c, double alpha, double beta, double gamma);
	void setLatticeVectors(const Vector3d& a, const Vector3d& b, const Vector3d& c);
	void copyMatrices(const UnitCell& other);
	//! Build a UnitCell from a set of three direct vectors.
	static UnitCell fromDirectVectors(const Vector3d& a, const Vector3d& b, const Vector3d& c, LatticeCentring centring=LatticeCentring::P, BravaisType bravais=BravaisType::Triclinic,std::shared_ptr<SX::Geometry::Basis> reference=nullptr);
	//! Build a UnitCell from a set of three reciprocal vectors.
	static UnitCell fromReciprocalVectors(const Vector3d& a, const Vector3d& b, const Vector3d& c,LatticeCentring centring=LatticeCentring::P, BravaisType bravais=BravaisType::Triclinic,std::shared_ptr<SX::Geometry::Basis> reference=nullptr);
	//!
 	void setLatticeCentring(LatticeCentring centring);
 	//!
 	void setBravaisType(BravaisType bravais);
 	//!
 	std::string getBravaisTypeSymbol() const;
 	//! Get a list of reflections sorted by their d* value in $\AA$ within a sphere defined by dstarmax
 	std::vector<Eigen::Vector3d> generateReflectionsInSphere(double dstarmax) const;
 	//! Return the angle in radians between two reflections h1,k1,l1 and h2,k2,l2
 	double getAngle(double h1,double k1, double l1, double h2, double k2, double l2) const;
 	//! Return the angle in radians between two reflections hkl1 and hkl2
 	double getAngle(const Eigen::Vector3d& hkl1,const Eigen::Vector3d& hkl2) const;
 	//! Get UB
 	void getUB(const Peak3D& p1, const Peak3D& p2);
 	//! Get the Busing-Levy B matrix as defined in Acta Cryst. (1967). 22, 457
 	//! The returned matrix is the transposed version of the matrix since
 	//! reciprocal bases are contravariant in NSXTool.
 	Eigen::Matrix3d getBusingLevyB() const;
 	Eigen::Matrix3d getBusingLevyU() const;
 	//! Print into a stream
 	void printSelf(std::ostream& os) const;

 	//! Gets the Z value for the unit cell
 	unsigned int getZ() const;
 	//! Sets the Z value for the unit cell
 	void setZ(unsigned int Z);

 	//! Gets the Material for the unit cell
 	Chemistry::sptrMaterial getMaterial() const;
 	//! Sets the Material for the unit cell
 	void setMaterial(Chemistry::sptrMaterial material);

private:
	Chemistry::sptrMaterial _material;
 	LatticeCentring _centring;
	BravaisType _bravaisType;
	unsigned int _Z;
};

//! Print to a stream
std::ostream& operator<<(std::ostream&,const UnitCell&);

}

}

#endif
