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
#include <memory>
#include <set>
#include <string>
#include <utility>

#include "Error.h"

namespace SX
{

namespace Chemistry
{

// Forward declarations
class Isotope;
class IsotopeManager;

class Isotope
{

private:

	friend class IsotopeManager;

private:

	//! Constructs an empty Isotope with a given name
	static Isotope* create(const std::string& name);

public:

	//! Destructor
	~Isotope();

	//! Return true if this Isotope is equal to another Isotope
	bool operator==(const Isotope& other);

	//! Returns the name of this Isotope
	const std::string& getName() const;

	//! Returns the symbol of this Isotope
	const std::string& getSymbol() const;

	//! Returns the chemical state of this Isotope
	const std::string& getState() const;

	//! Returns the nuclear spin of this Isotope
	double getNuclearSpin() const;

	//! Returns the natural abundance of this Isotope
	double getNaturalAbundance() const;
	//! Returns the half-life of this Isotope
	double getHalfLife() const;
	//! Returns whether this Isotope is stable or not
	bool getStable() const;

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
	//! Returns the formal charge of this Isotope
	double getFormalCharge() const;

	//! Returns the coherent scattering length of this Isotope
	std::complex<double>  getBCoherent() const;
	//! Returns the incoherent scattering length of this Isotope
	std::complex<double>  getBIncoherent() const;
	//! Returns the +1/2 spin-dependent scattering length of this Isotope
	std::complex<double>  getBPlus() const;
	//! Returns the -1/2 spin-dependent scattering length of this Isotope
	std::complex<double>  getBMinus() const;

	//! Returns the coherent cross section of this Isotope
	double getXsCoherent() const;
	//! Returns the incoherent cross section of this Isotope
	double getXsIncoherent() const;
	//! Returns the absorption cross section of this Isotope
	double getXsAbsorption() const;
	//! Returns the total cross section of this Isotope
	double getXsScattering() const;

	//! Returns true if this Isotope is an ion
	bool isIon() const;
	//! Return true if this Isotope is an anion (Q<0)
	bool isAnion() const;
	//! Return true if this Isotope is an cation (Q>0)
	bool isCation() const;

	//! Print some informations about this Isotope on a stream
	void print(std::ostream& os) const;

private:

	//! Default constructor
	Isotope();

	//! Copy constructor (deleted)
	Isotope(const Isotope& other)=delete;

	//! Construct an empty Isotope with a given name
	Isotope(const std::string& name);

	//! Assignment operator (deleted)
	Isotope& operator=(const Isotope& other)=delete;

private:

	//! The name of this Isotope
	std::string _name;
	//! The chemical symbol of this Isotope
	std::string _symbol;
	//! The chemical element corresponding to this Isotope
	std::string _element;
	//! The number of protons (aka Z) of this Isotope
	int _nProtons;
	//! The number of nucleons (aka N) of this Isotope
	int _nNucleons;
	//! The number of electrons of this Isotope
	int _nElectrons;
	//! The molar mass of this Isotope (in g/mol)
	double _molarMass;
	//! The nuclear spin of this Isotope
	double _nuclearSpin;
	//! The chemical state of this Isotope
	std::string _state;
	//! The natural abundance of this Isotope. Equal to 0 if unstable
	double _naturalAbundance;
	//! The half life of this Isotope. Equal to +Inf is stable
	double _halfLife;
	//! The stability of this Isotope
	bool _stable;
	//! The coherent scattering length of this Isotope
	std::complex<double> _bCoherent;
	//! The incoherent scattering length of this Isotope
	std::complex<double> _bIncoherent;
	//! The +1/2 spin-dependent scattering length of this Isotope
	std::complex<double> _bPlus;
	//! The -1/2 spin-dependent scattering length of this Isotope
	std::complex<double> _bMinus;
	//! The coherent cross section of this Isotope
	double _xsCoherent;
	//! The incoherent cross section of this Isotope
	double _xsIncoherent;
	//! The total cross section of this Isotope
	double _xsScattering;
	//! The absorption cross section of this Isotope
	double _xsAbsorption;

};

//! Overloads the operator<< with an Isotope object
std::ostream& operator<<(std::ostream&,const Isotope&);

} // end namespace Chemistry

} // end namespace SX

#endif /* NSXTOOL_ISOTOPE_H_ */
