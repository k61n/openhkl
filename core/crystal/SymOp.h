/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon, Eric Pellegrini
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

#pragma once

#include <ostream>

#include <Eigen/Dense>

#include "CrystalTypes.h"

namespace nsx {

//! \class SymOp
//! \brief Class to represent a symmetry operation of a spacegroup.
class SymOp {

public:
	//! Default constructor should not be used but is needed for swig.
	SymOp()=default;
	//! Construct a symmetry operation by parsing a generator.
	SymOp(std::string generator);
	//! Construct symmetry operation from an Eigen affine transformatiion
	SymOp(const affineTransformation& symmetryOperation);
	//! Copy constructor
	SymOp(const SymOp& other);
	//! Destructor
	~SymOp();
	//! Assignment
	SymOp& operator=(const SymOp& other);
	//! Comparison
	bool operator==(const SymOp& other) const;
	//! Compose symmetry operations
	SymOp operator*(const SymOp& other) const;
	//! Return 4x4 homogeneous matrix representation of the operation
	const affineTransformation& getMatrix() const;
	//! Pretty print
	void print(std::ostream& os) const;
	//! Returns the axis order
	int getAxisOrder() const;
	//! Returns the Jones symbol
	std::string getJonesSymbol() const;
	//! True if has translational part non zero
	bool hasTranslation() const;
	//! Return true if operation is pure translation
	bool isPureTranslation() const;
	//! Return the translation part of the symmetry operator
	Eigen::Vector3d getTranslationPart() const;
	//! Return the rotational part of the symmetry operator
	Eigen::Matrix3d getRotationPart() const;
    //! Return whether the translation part of 'other' is an integral multiple
    //! of the translation part of 'this'. Note: returns false if either is pure rotation.
    //! The return value, if non-zero, is the number n such that
    //! n * this->getTranslationPart() == other.getTranslationPart()
    int translationIsIntegralMultiple(const SymOp& other) const;

private:
	affineTransformation _matrix;
};

std::ostream& operator<<(std::ostream& os, const SymOp& sym);

} // end namespace nsx
