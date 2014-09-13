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
#include <iostream>
#include <Eigen/Dense>
#include "Basis.h"

namespace SX
{

namespace Crystal
{

using Eigen::Vector3d;

/** @brief Class to define a crystallographic unit-cell.
 *
 * Provide functionalities to transform vectors of the direct lattice or reciprocal
 * lattice from unit-cell coordinates to a right-handed coordinates system.
 * The UnitCell is defined by the parameters a,b,c and angles alpha, beta, gamma.
 * The UnitCell parameters a,b,c are given in \f$ \AA \f$  and angle alpha, beta, gamma
 * are given in degrees.
 * The a axis is chosen as pointing along the x-direction, and the b-axis is in the xy-plane.
 */

//! Centering type of the Bravais lattice
enum  class LatticeCentring : char  {P='P',A='A',B='B',C='C',I='I',F='F',R='R'};
//! Bravais type
enum  class BravaisType : char  {Triclinic='a',Monolinic='m',Orthorhombic='o',Tetragonal='t',Hexagonal='h',Cubic='c'};

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
	//! Build a UnitCell from a set of three direct vectors.
	static UnitCell fromDirectVectors(const Vector3d& a, const Vector3d& b, const Vector3d& c, LatticeCentring centring=LatticeCentring::P, BravaisType bravais=BravaisType::Triclinic,std::shared_ptr<SX::Geometry::Basis> reference=nullptr);
	//! Build a UnitCell from a set of three reciprocal vectors.
	static UnitCell fromReciprocalVectors(const Vector3d& a, const Vector3d& b, const Vector3d& c,LatticeCentring centring=LatticeCentring::P, BravaisType bravais=BravaisType::Triclinic,std::shared_ptr<SX::Geometry::Basis> reference=nullptr);
	/// Get the a parameter (\f$ \AA \f$)
	double getA() const;
	/// Get the b parameter (\f$ \AA \f$)
 	double getB() const;
 	/// Get the c parameter (\f$ \AA \f$)
 	double getC() const;
	/// Get the alpha angle (radians)
 	double getAlpha() const;
	/// Get the beta angle (radians)
 	double getBeta() const;
	/// Get the gamma angle (radians)
 	double getGamma() const;
	//!
 	void setLatticeCentring(LatticeCentring centring);
 	//!
 	void setBravaisType(BravaisType bravais);
 	//! Print to a stream
 	void printSelf(std::ostream& os) const;
private:
	LatticeCentring _centring;
	BravaisType _bravaisType;
};
//! Print to a stream
std::ostream& operator<<(std::ostream&,const UnitCell&);

}

}

#endif
