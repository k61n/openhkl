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

#include <complex>
#include <ostream>
#include <string>

namespace SX
{

namespace Chemistry
{

class IsotopeManager;

class Isotope
{
public:
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

	friend class IsotopeManager;

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


} // end namespace Chemistry

} // end namespace SX

#endif /* NSXTOOL_ISOTOPE_H_ */
