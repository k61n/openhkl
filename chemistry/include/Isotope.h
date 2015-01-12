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
#include <complex>
#include <ostream>
#include <set>
#include <string>
#include <stdexcept>
#include <utility>

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "Error.h"

namespace SX
{

namespace Chemistry
{

using boost::property_tree::ptree;

class Isotope;

typedef std::set<Isotope*> isotopeSet;
typedef std::map<std::string,Isotope*> isotopeMap;
typedef std::pair<std::string,Isotope*> isotopePair;

class Isotope
{

public:

	//! Return a pointer to an Isotope built from the isotopes XML database
	static Isotope* buildFromDatabase(const std::string& name);

	//! Returns the number of registered isotopes
	static unsigned int getNRegisteredIsotopes();

	//! Returns the set of Isotopes whose property |prop| matches the value |value|
	template<typename T>
	static isotopeSet getIsotopes(const std::string& prop, T value);

	//! Returns the value of property |prop| for isotope |name|
	template<typename T>
	static T getProperty(const std::string& name, const std::string& prop);

private:

	//! Read an isotope XML node and returns the corresponding Isotope object
	static Isotope* readIsotope(const ptree& node);

private:

	static std::string database;
	static isotopeMap registry;

public:

	//! Copy constructor (deleted)
	Isotope(const Isotope& other)=delete;

	//! Destructor
	~Isotope();

	//! Return true if this Isotope is equal to another Isotope
	bool operator==(const Isotope& other);

	//! Returns the name of this Isotope
	const std::string& getName() const;

	//! Returns the abundance of this Isotope
	double getAbundance() const;

	//! Returns the formal charge of this Isotope
	double getFormalCharge() const;

	//! Returns the molar mass of this Isotope
	double getMolarMass() const;

	//! Returns the number of electrons of this Isotope
	double getNElectrons() const;
	//! Returns the number of neutrons (aka N) of this Isotope
	double getNNeutrons() const;
	//! Returns the number of nucleons of this Isotope
	double getNNucleons() const;
	//! Returns the number of protons (aka Z) of this Isotope
	double getNProtons() const;

	//! Returns true if this Isotope is an ion
	bool isIon() const;
	//! Return true if this Isotope is an anion (Q<0)
	bool isAnion() const;
	//! Return true if this Isotope is an cation (Q>0)
	bool isCation() const;

	//! Print some informations about this Isotope on a stream
	void print(std::ostream& os) const;

private:

	//! Constructs an empty Isotope
	Isotope();

private:

	std::string _name;
	std::string _symbol;
	std::string _element;
	int _nProtons;
	int _nNucleons;
	int _nElectrons;
	double _molarMass;
	double _nuclearSpin;
	std::string _state;
	double _abundance;
	double _halfLife;
	bool _stable;
	std::complex<double> _bCoherent;
	std::complex<double> _bIncoherent;
	std::complex<double> _bPlus;
	std::complex<double> _bMinus;
	double _xsCoherent;
	double _xsIncoherent;
	double _xsScattering;
	double _xsAbsorption;

};

//! Overloads the operator<< with an Isotope object
std::ostream& operator<<(std::ostream&,const Isotope&);

template<typename T>
T Isotope::getProperty(const std::string& name, const std::string& prop)
{
	ptree root;
	read_xml(database,root);

	BOOST_FOREACH(ptree::value_type const& v, root.get_child("isotopes"))
	{
		if (v.first.compare("isotope")!=0)
			continue;

		if (v.second.get<std::string>("<xmlattr>.name").compare(name)==0)
		{
			try
			{
				return v.second.get<T>(prop);
			}
			catch (const std::runtime_error& error)
			{
				throw SX::Kernel::Error<Isotope>(error.what());
			}
		}
	}
	throw SX::Kernel::Error<Isotope>("No match for entry "+name);
}

template<typename T>
isotopeSet Isotope::getIsotopes(const std::string& prop, T value)
{
	isotopeSet isset;

	// Open and read the isotopes XML database
	ptree root;
	read_xml(database,root);

	// Loop over the nodes of the isotopes XML database
	BOOST_FOREACH(ptree::value_type const& v, root.get_child("isotopes"))
	{
		// If the node is not an isotope node, ignore it
		if (v.first.compare("isotope")!=0)
			continue;

		// Search for the target property among the available ones for this isotope
		boost::optional<T> opt=v.second.get_optional<T>(prop);
		if (opt)
		{
			// Case where the property was found and matches the target value
			if (opt.get()==value)
			{
				Isotope* is=readIsotope(v.second);
				isset.insert(is);
			}
		}
	}

	// If the search gave no match, throws
	if (isset.empty())
		throw SX::Kernel::Error<Isotope>("No isotopes matches property "+prop+" with value "+value);

	return isset;
}

} // end namespace Chemistry

} // end namespace SX

#endif /* NSXTOOL_ISOTOPE_H_ */
