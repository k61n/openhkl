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

#ifndef NSXLIB_SYMOP_H
#define NSXLIB_SYMOP_H

#include <ostream>

#include <Eigen/Dense>

namespace nsx {

typedef Eigen::Transform<double,3,Eigen::Affine> affineTransformation;

class SymOp {

public:

	SymOp()=delete;

	SymOp(std::string generator);

	SymOp(const affineTransformation& symmetryOperation);

	SymOp(const SymOp& other);

	~SymOp();

	SymOp& operator=(const SymOp& other);

	bool operator==(const SymOp& other) const;

	SymOp operator*(const SymOp& other) const;

	const affineTransformation& getMatrix() const;

	void print(std::ostream& os) const;

	//! Returns the axis order
	int getAxisOrder() const;
	//! Returns the Jones symbol
	std::string getJonesSymbol() const;

	//! True if has translational part non zero
	bool hasTranslation() const;

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

#endif // NSXLIB_SYMOP_H
