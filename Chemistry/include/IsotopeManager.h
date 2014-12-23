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

#ifndef NSXTOOL_ISOTOPEMANAGER_H_
#define NSXTOOL_ISOTOPEMANAGER_H_

#include <limits.h>
#include <map>
#include <set>
#include <string>

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "Error.h"
#include "Isotope.h"
#include "Singleton.h"
#include "Units.h"

namespace SX
{

namespace Chemistry
{

using boost::property_tree::ptree;
using SX::Units::UnitsManager;

typedef std::set<Isotope*> isotopeSet;
typedef std::map<std::string,Isotope*> isotopeMap;
typedef std::pair<std::string,Isotope*> isotopePair;

class IsotopeManager : public SX::Kernel::Singleton<IsotopeManager,SX::Kernel::Constructor,SX::Kernel::Destructor>
{

public:

	//! Returns the number of registered isotopes
	unsigned int getNRegisteredIsotopes() const;

	//! Returns the Isotope whose entry |name| matches the input value |value|
	Isotope* getIsotope(const std::string& name) const;

	//! Returns a set of Isotope whose property |key| matches the input value |value|
	template<typename T>
	isotopeSet getIsotopes(const std::string& key, T value) const;

	//! Returns the value of the property |prop| of entry |name|
	template<typename T>
	T getProperty(const std::string& name, const std::string& prop) const;

public:
	static std::string database;

private:
	friend class SX::Kernel::Constructor<IsotopeManager>;
	friend class SX::Kernel::Destructor<IsotopeManager>;
	IsotopeManager();

private:
	static isotopeMap _isotopeRegistry;

};

template<typename T>
isotopeSet IsotopeManager::getIsotopes(const std::string& key, T value) const
{
	isotopeSet ismap;

	ptree root;
	read_xml(database,root);

	UnitsManager* um = UnitsManager::Instance();

	double units;
	BOOST_FOREACH(ptree::value_type const& v, root.get_child("sx_isotope_database"))
	{
		if (v.first.compare("isotope")!=0)
			continue;
		if (v.second.get<T>(key)==value)
		{
			std::string name=v.second.get<std::string>("<xmlattr>.name");
			auto p=_isotopeRegistry.find(name);
			if (p!=_isotopeRegistry.end())
				ismap.insert(p->second);
			else
			{
				Isotope* is=new Isotope();
				is->_name=name;
				is->_symbol=v.second.get<std::string>("symbol");
				is->_element=v.second.get<std::string>("element");
				is->_nProtons=v.second.get<int>("nProtons");
				is->_nNucleons=v.second.get<int>("nNucleons");
				is->_nElectrons=is->_nProtons;
				units=um->get(v.second.get<std::string>("<xmlattr>.units"));
				is->_molarMass=v.second.get<double>("molarMass")*units;
				is->_nuclearSpin=v.second.get<double>("nuclearSpin");
				is->_state=v.second.get<std::string>("state");
				is->_abundance=v.second.get<double>("abundance",0.0)*units;
				is->_halfLife=v.second.get<double>("halfLife",std::numeric_limits<double>::infinity())*units;
				is->_stable=v.second.get<bool>("stable");
				is->_bCoherent=v.second.get<std::complex<double>>("bCoherent")*units;
				is->_bIncoherent=v.second.get<std::complex<double>>("bIncoherent")*units;
				is->_bPlus=v.second.get<std::complex<double>>("bPlus");
				is->_bMinus=v.second.get<std::complex<double>>("bMinus");
				is->_xsCoherent=v.second.get<double>("xsCoherent");
				is->_xsIncoherent=v.second.get<double>("xsIncoherent");
				is->_xsScattering=v.second.get<double>("xsScattering");
				is->_xsAbsorption=v.second.get<double>("xsAbsorption");
				auto ret=_isotopeRegistry.insert(isotopePair(name,is));
				ismap.insert(ret.first->second);
			}
		}
	}

	return ismap;
}

template<typename T>
T IsotopeManager::getProperty(const std::string& name, const std::string& prop) const
{

	ptree root;
	read_xml(database,root);

	BOOST_FOREACH(ptree::value_type const& v, root.get_child("sx_isotope_database"))
	{
		if (v.first.compare("isotope")!=0)
			continue;

		if (v.second.get<std::string>("<xmlattr>.name").compare(name)==0)
			return v.second.get<T>(prop);
	}
	throw SX::Kernel::Error<IsotopeManager>("No match for entry "+name+" and/or property "+prop);
}

} // end namespace Chemistry

} // end namespace SX

#endif /* NSXTOOL_ISOTOPEMANAGER_H_ */
