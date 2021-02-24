//  ***********************************************************************************************
//
//  XSECTIONTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/utils/Units.h
//! @brief     Defines class UnitsManager
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef XSECTION_LIB_UNITS_H
#define XSECTION_LIB_UNITS_H

#include <math.h>
#include <unordered_map>

#include "ISingleton.h"

namespace xsection {

//! Define the units used internally in XSECTIONTOOL.
//! The units are defined in the international units system aka SI
//! The Units namespace allows users of the library to specify their units when
//! defining variables. For example it is recommended to use statements such as
//! : double x=34*deg and when using methods: a.Rotate(34*deg). This guarantees
//! a self-consistent system of units in all calculations. This is inspired from
//! the system of units in Geant4. Printing a property in a given unit requires
//! to divide by the units. For example, if one wants an angle in degrees,
//! angle/deg needs to be used.

//! ISingleton class to map the symbol of a Unit with its value in the internal
//! reference system. This is used for example when parsing values from input
//! XML files.
class UnitsManager : public ISingleton<UnitsManager, Constructor, Destructor> {
public:
    //! Gets the value of a given unit with respect to the internal reference unit.
    //! Throws std::invalid_argument if not present
    static double get(const std::string& units);

private:
    //! Store pairs of symbols and values.
    static std::unordered_map<std::string, double> _unitsMap;
};

// Prefixes
constexpr double yocto = 1.0e-24;
constexpr double zepto = 1.0e-21;
constexpr double atto = 1.0e-18;
constexpr double femto = 1.0e-15;
constexpr double pico = 1.0e-12;
constexpr double nano = 1.0e-9;
constexpr double micro = 1.0e-6;
constexpr double milli = 1.0e-3;
constexpr double centi = 1.0e-2;
constexpr double deci = 1.0e-1;
constexpr double deca = 1.0e1;
constexpr double hecto = 1.0e2;
constexpr double kilo = 1.0e3;
constexpr double mega = 1.0e6;
constexpr double giga = 1.0e9;
constexpr double tera = 1.0e12;
constexpr double peta = 1.0e15;
constexpr double exa = 1.0e18;
constexpr double zetta = 1.0e21;
constexpr double yotta = 1.0e24;

// 2*pi
constexpr double twopi = 2.0 * M_PI;

// Avogadro number
constexpr double avogadro = 6.0221367e23;

// Elementary charge
constexpr double Q = 1.60217649e-19;

// arbitrary unit
constexpr double au = 1.0;

// Percentage
constexpr double perCent = 0.01;

// Length units
constexpr double m = 1.0;
constexpr double dm = m * deci;
constexpr double cm = m * centi;
constexpr double mm = m * milli;
constexpr double um = m * micro;
constexpr double nm = m * nano;
constexpr double pm = m * pico;
constexpr double fm = m * femto;
constexpr double am = m * 1e-10;
constexpr double ang = m * 1e-10;

// Time units
constexpr double s = 1.0;
constexpr double ms = s * milli;
constexpr double us = s * micro;
constexpr double ns = s * nano;
constexpr double ps = s * pico;
constexpr double fs = s * femto;
constexpr double min = 60 * s;
constexpr double hour = 3600 * s;
constexpr double day = 24 * hour;
constexpr double year = 364.25 * day;

// Matter units
constexpr double mole = 1.0;

// Mass units
constexpr double kg = 1.0;
constexpr double g = kg * milli;
constexpr double Da = g / avogadro;
constexpr double uma = Da;
constexpr double g_per_mole = g;

// Temperature units
constexpr double Kelvin = 1.0;

// Energy units
constexpr double J = m * kg / s / s;
constexpr double J_per_mole = 1.66055927e-24 * J;
constexpr double cal = 4.184 * J;
constexpr double cal_per_mole = 6.94778e-24 * J;
constexpr double eV = Q * J;

// Angle units
constexpr double rad = 1.0;
constexpr double deg = rad * M_PI / 180.0;
constexpr double mrad = rad * 0.001;

// Solid angle units
constexpr double sr = 1.0;

// Frequency units
constexpr double Hz = 1.0 / s;

// Force units
constexpr double N = m * kg / s / s;

// Pressure units
constexpr double Pa = N / m / m;
constexpr double Bar = 1.0e5 * Pa;

// Power units
constexpr double W = m * m * kg / s / s / s;

// Surface units
constexpr double m2 = m * m;
constexpr double cm2 = cm * cm;
constexpr double mm2 = mm * mm;
constexpr double um2 = um * um;
constexpr double nm2 = nm * nm;
constexpr double pm2 = pm * pm;
constexpr double am2 = am * am;
constexpr double ang2 = ang * ang;
constexpr double barn = cm2 * yocto;

// Volume units
constexpr double m3 = m * m * m;
constexpr double dm3 = dm * dm * dm;
constexpr double l = dm3;
constexpr double cm3 = cm * cm * cm;
constexpr double mm3 = mm * mm * mm;
constexpr double um3 = um * um * um;
constexpr double nm3 = nm * nm * nm;
constexpr double pm3 = pm * pm * pm;
constexpr double fm3 = fm * fm * fm;
constexpr double am3 = am * am * am;
constexpr double ang3 = ang * ang * ang;

// Density
constexpr double kg_per_m3 = 1.0;
constexpr double g_per_cm3 = kilo * kg_per_m3;

// Temperature units
constexpr double K = 1.0;

// Constants
constexpr double c = 299792458.0 * m / s;
constexpr double h = 6.626176e-34 * J * s;
constexpr double hbar = h / twopi;
constexpr double kB = 1.3806513e-23 * J / K;
constexpr double R = 8.3144621 * J / mole / K;

} // namespace xsection

#endif // XSECTION_LIB_UNITS_H
