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

#ifndef NSXTOOL_SYMMETRYGROUPGENERATOR_H_
#define NSXTOOL_SYMMETRYGROUPGENERATOR_H_

#include <ostream>
#include <vector>
#include <string>
#include <array>
#include <Eigen/Dense>
#include "SymOp.h"

namespace SX
{

namespace Crystal
{

typedef std::vector<SymOp> groupElementsList;

class SpaceGroup
{

public:
	//! Construct a space group from its IT symbol. Lookup in the IUCR table
	SpaceGroup(const std::string& symbol);
	//! Construct a space group from a string containing the generators given in the Jones notation and separated
	//! by ";" characters
	SpaceGroup(const std::string& symbol, const std::string& generators);
	//! Copy
	SpaceGroup(const SpaceGroup& other);
	//! Assignment
	SpaceGroup& operator=(const SpaceGroup& other);
	//! Get the IT symbol for this space group
	const std::string& getSymbol() const;
	//! Get the string containing the generators (in the Jones notation), separated bt ";' character
	const std::string& getGenerators() const;
	//! Get a vector containing the Symmetry operations for this space group
	const groupElementsList& getGroupElements() const;
	//! Determine whether a h,k,l reflection is forbidden
	bool isExtinct(double h, double k, double l) const;
	//! Return true if centrosymmetric
	bool isCentrosymmetric() const;
	//! Print to a stream
	void print(std::ostream& os) const;
	//! Return the type of cell (triclinic, monoclinic ...)
	char getBravaisType() const;
    //! Return the percentage of extinct reflections
    double fractionExtinct(std::vector<std::array<double, 3>> hkl);
    //! Return the Bravais type symbol
    std::string getBravaisTypeSymbol() const;
    //! Return the ID of the space group
    int getID() const;
private:

	void generateGroupElements();

private:

	std::string _symbol;

	std::string _generators;

	groupElementsList _groupElements;

};

std::ostream& operator<<(std::ostream& os, const SpaceGroup& sg);

} // end namespace Crystal

} // end namespace SX

#endif /* NSXTOOL_SYMMETRYGROUPGENERATOR_H_ */
