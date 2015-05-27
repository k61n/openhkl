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

	SpaceGroup(const std::string& symbol);

	SpaceGroup(const std::string& symbol, const std::string& generators);

	SpaceGroup(const SpaceGroup& other);

	SpaceGroup& operator=(const SpaceGroup& other);

	const std::string& getSymbol() const;

	const std::string& getGenerators() const;

	const groupElementsList& getGroupElements() const;

	bool isExtinct(double h, double k, double l) const;

	void print(std::ostream& os) const;

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
