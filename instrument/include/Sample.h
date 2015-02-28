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
#ifndef NSXTOOL_SAMPLE_H_
#define NSXTOOL_SAMPLE_H_

#include <memory>
#include <string>

#include "Component.h"
#include "ConvexHull.h"
#include "Material.h"
#include "UnitCell.h"
#include <vector>
namespace SX
{
namespace Instrument
{

class Sample : public Component
{
public:

	// Constructors and destructor

	// Default constructor
	Sample();
	//! Copy constructor
	Sample(const Sample& other);
	//! Constructs a default sample with a given name
	Sample(const std::string& name);
	//! Destructor
	virtual ~Sample();
	//! Virtual copy constructor
	Component* clone() const;
	// Operators
	//! Assignment operator
	Sample& operator=(const Sample& other);

	//! Return the sample shape, described as a convex hull
	SX::Geometry::ConvexHull<double>& getShape();

	//! Returns a shared pointer to the material this Sample is made of
	SX::Chemistry::sptrMaterial getMaterial() const;

	//! Create a new crystal with Empty UnitCell, and return it
	std::shared_ptr<SX::Crystal::UnitCell> addUnitCell();
	//! Get the UnitCell of Crystal number i in the list
	std::shared_ptr<SX::Crystal::UnitCell> getUnitCell(int i);
	//! Return number of crystals
	std::size_t getNCrystals() const;
private:
	SX::Geometry::ConvexHull<double> _sampleShape;
	std::shared_ptr<SX::Chemistry::Material> _material;
	//! UnitCells of all crystals associated with this sample
	std::vector<std::shared_ptr<SX::Crystal::UnitCell>> _cells;
};

} // Namespace Instrument
} /* namespace SX */

#endif /* NSXTOOL_SAMPLE_H_ */
