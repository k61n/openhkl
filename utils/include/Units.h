/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
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

#ifndef NSXTOOL_UNITS_H_
#define NSXTOOL_UNITS_H_
#include <cmath>
#include <Singleton.h>
#include <unordered_map>
#include <string>

namespace SX
{
//! Define the units used internally in NSXTOOL.
//! The Units namespace allows users of the library to specify their units when defining variables.
//! For example it is recommended to use statements such as : double x=34*deg and when using methods:
//! a.Rotate(34*deg). This guarantees a self-consistent system of units in all calculations. This is inspired
//! from the system of units in Geant4. Printing a property in a given unit requires to divide by the units.
//! For example, if one wants an angle in degrees, angle/Units::deg needs to be used.
//!
//!
//!
namespace Units
{

using namespace SX::Kernel;

//! Singleton class to map the symbol of a Unit with its value in the internal reference system.
//! This is used for example when parsing values from input XML files.
class UnitsManager : public Singleton<UnitsManager,Constructor,Destructor>
{
	public:
	//! Get the value of a given unit with respect to the internal reference unit.
	//! Throws std::invalid_argument if not present
	static double get(const std::string&);
	private:
	//! Store pairs of symbols and values.
	static std::unordered_map<std::string,double> _unitsMap;
};
// Units of [L]
static const double m=1.0;
static const double cm=m*0.01;
static const double mm=m*1e-3;
static const double um=m*1e-6;
static const double nm=m*1e-9;
static const double pm=m*1e-12;
static const double fm=m*1e-15;
static const double am=m*1e-10;

// Units of [L]^2
static const double m2=m*m;
static const double cm2=cm*cm;
static const double mm2=mm*mm;
static const double um2=um*um;
static const double nm2=nm*nm;
static const double pm2=pm*pm;
static const double am2=am*am;
static const double barn=m2*1e-24;

// Units of [L]^3
static const double m3=m*m*m;
static const double cm3=cm*cm*cm;
static const double mm3=mm*mm*mm;
static const double um3=um*um*um;
static const double nm3=nm*nm*nm;
static const double pm3=pm*pm*pm;
static const double fm3=fm*fm*fm;
static const double am3=am*am*am;

// Units of angle
static const double rad=1.0;
static const double deg=rad*M_PI/180.0;
static const double mrad=rad*0.001;

// Solid angle
static const double str=1.0;

// Units of [T]
static const double s=1.0;
static const double ms=s*1e-3;
static const double us=s*1e-6;
static const double min=60*s;
static const double hour=3600*s;
static const double day=24*hour;
static const double year=364.25*day;

//
static const double perCent=0.01;

} //Namespace Units

} //Namespace SX


#endif /* NSXTOOL_UNITS_H_ */
