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

#pragma once

#include <string>
#include <vector>

#include <yaml-cpp/yaml.h>

#include "ChemistryTypes.h"
#include "Component.h"
#include "ConvexHull.h"
#include "CrystalTypes.h"

namespace nsx {

class Sample : public Component {
public:

    //! Static constructor of a Sample from a property tree node
    static Sample* create(const YAML::Node& node);

    // Default constructor
    Sample();
    //! Copy constructor
    Sample(const Sample& other);
    //! Constructs a default sample with a given name
    Sample(const std::string& name);
    //! Constructs a sample from a property tree node
    Sample(const YAML::Node& node);
    //! Virtual copy constructor
    Sample* clone() const;
    //! Destructor
    virtual ~Sample();
    // Operators
    //! Assignment operator
    Sample& operator=(const Sample& other);

    //! Set the sample shape described as a convex hull
    void setShape(const ConvexHull& shape);

    //! Return the sample shape, described as a convex hull
    ConvexHull& shape();

    //! Create a new crystal with Empty UnitCell, and return it
    std::shared_ptr<UnitCell> addUnitCell(std::shared_ptr<UnitCell> cell = nullptr);

    //! Get the UnitCell of Crystal number i in the list
    sptrUnitCell unitCell(int index);
    const UnitCellList& unitCells() const;
    //! Return number of crystals
    std::size_t nCrystals() const;
    //!
    void removeUnitCell(int index);
    void removeUnitCell(sptrUnitCell cell);

    //! Gets the Z number of a given unit cell
    unsigned int z(int index) const;
    //! Sets the Z number of a given unit cell
    void setZ(int Z, int index);

    //! Gets the Material of one of the unit cells of this Sample
    sptrMaterial material(int index) const;
    //! Sets the Material of one of the unit cells of this Sample
    void setMaterial(sptrMaterial material, int index);

private:
    ConvexHull _sampleShape;
    //! UnitCells of all crystals associated with this sample
    UnitCellList _cells;
};

} // end namespace nsx
