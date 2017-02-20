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

#ifndef NSXTOOL_MATERIALMANAGER_H_
#define NSXTOOL_MATERIALMANAGER_H_

#include <map>
#include <memory>
#include <set>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "Material.h"
#include "../kernel/Singleton.h"

namespace SX
{

namespace Chemistry
{

typedef std::map<std::string,sptrMaterial> MaterialsRegistry;

namespace filesystem=boost::filesystem;
namespace property_tree=boost::property_tree;
namespace xml_parser=boost::property_tree::xml_parser;

class MaterialManager : public SX::Kernel::Singleton<MaterialManager,SX::Kernel::Constructor,SX::Kernel::Destructor>
{

public:

    //! Default constructor
    MaterialManager();

    //! Destructor
    ~MaterialManager();

    //! Builds and register an empty material. If the material with the same name is already registered, throws.
    sptrMaterial buildEmptyMaterial(const std::string& name, BuildingMode mode);
    //! Constructs a material from a chemical formula and a given physical state
    sptrMaterial buildMaterialFromChemicalFormula(std::string formula);
    //! Find a Material
    sptrMaterial getMaterial(const std::string& name);
    //! Clean up the registry
    void cleanRegistry();
    //! Returns the number of registered materials
    unsigned int getNMaterials() const;
    //! Returns true if a Material with a given name is registered
    bool hasMaterial(const std::string& name) const;
    //! Save the registry to an XML file
    void saveRegistry(std::string filename="") const;

    //! Sets the path for the materials XML database
    void setDatabasePath(const std::string& path);

private:

    //! Builds a Material from an XML node
    sptrMaterial buildMaterial(const property_tree::ptree& node);

private:

    //! The path to the material database
    std::string _database;

    //! The registry that will store the created Material objects
    MaterialsRegistry _registry;

};

} // end namespace Chemistry

} // end namespace SX

#endif /* NSXTOOL_MATERIALMANAGER_H_ */
