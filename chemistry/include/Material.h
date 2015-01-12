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

#ifndef NSXTOOL_MATERIAL_H_
#define NSXTOOL_MATERIAL_H_

#include <map>
#include <string>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace SX
{

namespace Chemistry
{

using boost::property_tree::ptree;

class Element;

class Material;

typedef std::vector<Element*> elementVector;
typedef std::pair<std::string,Material*> materialPair;
typedef std::map<std::string,Material*> materialMap;

class Material
{

public:

	enum class FillingMode : unsigned int {MassFraction=0,MoleFraction=1,numberOfAtoms=2};

	enum class State : unsigned int {Solid=0,Liquid=1,Gaz=2};

public:

	static Material* buildFromDatabase(const std::string& name);

private:

	static Material* readMaterial(const ptree& node);

private:

	static std::map<std::string,State> _toState;
	static std::map<std::string,FillingMode> _toFillingMode;
	static double tolerance;
	static std::string database;
	static materialMap registeredMaterials;

public:

	Material()=delete;

	Material(const Material& other)=delete;

	Material(const std::string& name, State state=State::Solid, FillingMode fillingMode=FillingMode::MassFraction);

	~Material();

	void addElement(Element* element, double fraction);

	void addElement(const std::string& name, double fraction);

	void addMaterial(Material* material, double fraction);

	void addMaterial(const std::string& name, double fraction);

	const std::string& getName() const;

	//! Returns the number of elements this Material is made of
	unsigned int getNElements() const;

	double getDensity() const;

	void setDensity(double density);

	void setDensity(double pressure, double temperature);

	const elementVector& getElements() const;

	std::map<Element*,double> getMoleFractions() const;

	std::map<Element*,double> getMassFractions() const;

	std::map<Element*,double> getNAtomsPerVolume() const;

private:

	std::string _name;
	double _density;
	State _state;
	FillingMode _fillingMode;
	elementVector _elements;
	std::vector<double> _fractions;

};

} // end namespace Chemistry

} // end namespace SX

#endif /* NSXTOOL_MATERIAL_H_ */
