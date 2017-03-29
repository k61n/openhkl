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

#ifndef NSXTOOL_ISOTOPE_H_
#define NSXTOOL_ISOTOPE_H_

#include <map>
#include <ostream>
#include <memory>
#include <string>

#include <boost/any.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>

namespace SX {

namespace Chemistry {

using boost::any_cast;
using boost::property_tree::ptree;
using boost::filesystem::path;

// Forward declarations
class Isotope;

class Isotope
{

public:

    enum class PropertyType {String, Int, Double, Complex, Bool};

    static std::map<std::string,PropertyType> PropertyTypes;

public:

    Isotope()=delete;

    Isotope(const Isotope& other)=default;

    Isotope& operator=(const Isotope& other)=default;

	Isotope(const ptree& isotopeNode);

	//! Destructor
	~Isotope()=default;

	//! Returns the name of this Isotope
	const std::string& getName() const;

	template <typename PropertyType>
	PropertyType getProperty(const std::string& propertyName) const;

	bool hasProperty(const std::string& propertyName) const;

	//! Print some informations about this Isotope on a stream
	void print(std::ostream& os) const;

	//! Inserts the information about this Element to an XML parent node
	ptree writeToXML() const;

private:

	//! The name of this Isotope
	std::string _name;

	std::map<std::string,boost::any> _properties;
	std::map<std::string,std::string> _units;
    std::map<std::string,std::string> _types;

};

template <typename PropertyType>
PropertyType Isotope::getProperty(const std::string& propertyName) const
{
    auto pit = _properties.find(propertyName);
    if (pit == _properties.end())
        throw std::runtime_error("Isotope "+_name+": unknown property name ("+propertyName+")");

    return any_cast<PropertyType>(pit->second);
}

//! Overloads the operator<< with an Isotope object
std::ostream& operator<<(std::ostream&,const Isotope&);

} // end namespace Chemistry

} // end namespace SX

#endif /* NSXTOOL_ISOTOPE_H_ */
