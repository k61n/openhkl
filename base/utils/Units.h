//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/utils/Units.h
//! @brief     Defines class UnitsManager
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_BASE_UTILS_UNITS_H
#define NSX_BASE_UTILS_UNITS_H

#include <cmath>
#include <string>
#include <unordered_map>

#include "base/utils/ISingleton.h"

namespace nsx {

//! The units used internally in NSXTOOL.

//! The units are defined in the international units system aka SI
//! The Units namespace allows users of the library to specify their units when
//! defining variables. For example it is recommended to use statements such as
//! : double x=34*deg and when using methods: a.Rotate(34*deg). This guarantees
//! a self-consistent system of units in all calculations. This is inspired from
//! the system of units in Geant4. Printing a property in a given unit requires
//! to divide by the units. For example, if one wants an angle in degrees,
//! angle/deg needs to be used.

//! Singleton class to map the symbol of a Unit with its value in the internal
//! reference system. This is used for example when parsing values from input XML files.

class UnitsManager : public ISingleton<UnitsManager> {
 public:
    //! Gets the value of a given unit with respect to the internal reference unit.
    //! Throws std::invalid_argument if not present
    static double get(const std::string& units);
};

// Prefixes
static const double yocto = 1.0e-24;
static const double zepto = 1.0e-21;
static const double atto = 1.0e-18;
static const double femto = 1.0e-15;
static const double pico = 1.0e-12;
static const double nano = 1.0e-9;
static const double micro = 1.0e-6;
static const double milli = 1.0e-3;
static const double centi = 1.0e-2;
static const double deci = 1.0e-1;
static const double deca = 1.0e1;
static const double hecto = 1.0e2;
static const double kilo = 1.0e3;
static const double mega = 1.0e6;
static const double giga = 1.0e9;
static const double tera = 1.0e12;
static const double peta = 1.0e15;
static const double exa = 1.0e18;
static const double zetta = 1.0e21;
static const double yotta = 1.0e24;

// 2*pi
static const double twopi = 2.0 * M_PI;

// Avogadro number
static const double avogadro = 6.0221367e23;

// Elementary charge
static const double Q = 1.60217649e-19;

// arbitrary unit
static const double au = 1.0;

// Percentage
static const double perCent = 0.01;

// Length units
static const double m = 1.0;
static const double dm = m * deci;
static const double cm = m * centi;
static const double mm = m * milli;
static const double um = m * micro;
static const double nm = m * nano;
static const double pm = m * pico;
static const double fm = m * femto;
static const double am = m * 1e-10;
static const double ang = m * 1e-10;

// Time units
static const double s = 1.0;
static const double ms = s * milli;
static const double us = s * micro;
static const double ns = s * nano;
static const double ps = s * pico;
static const double fs = s * femto;
static const double min = 60 * s;
static const double hour = 3600 * s;
static const double day = 24 * hour;
static const double year = 364.25 * day;

// Matter units
static const double mole = 1.0;

// Mass units
static const double kg = 1.0;
static const double g = kg * milli;
static const double Da = g / avogadro;
static const double uma = Da;
static const double g_per_mole = g;

// Temperature units
static const double Kelvin = 1.0;

// Energy units
static const double J = m * kg / s / s;
static const double J_per_mole = 1.66055927e-24 * J;
static const double cal = 4.184 * J;
static const double cal_per_mole = 6.94778e-24 * J;
static const double eV = Q * J;

// Angle units
static const double rad = 1.0;
static const double deg = rad * M_PI / 180.0;
static const double mrad = rad * 0.001;

// Solid angle units
static const double sr = 1.0;

// Frequency units
static const double Hz = 1.0 / s;

// Force units
static const double N = m * kg / s / s;

// Pressure units
static const double Pa = N / m / m;
static const double Bar = 1.0e5 * Pa;

// Power units
static const double W = m * m * kg / s / s / s;

// Surface units
static const double m2 = m * m;
static const double cm2 = cm * cm;
static const double mm2 = mm * mm;
static const double um2 = um * um;
static const double nm2 = nm * nm;
static const double pm2 = pm * pm;
static const double am2 = am * am;
static const double ang2 = ang * ang;
static const double barn = cm2 * yocto;

// Volume units
static const double m3 = m * m * m;
static const double dm3 = dm * dm * dm;
static const double l = dm3;
static const double cm3 = cm * cm * cm;
static const double mm3 = mm * mm * mm;
static const double um3 = um * um * um;
static const double nm3 = nm * nm * nm;
static const double pm3 = pm * pm * pm;
static const double fm3 = fm * fm * fm;
static const double am3 = am * am * am;
static const double ang3 = ang * ang * ang;

// Density
static const double kg_per_m3 = 1.0;
static const double g_per_cm3 = kilo * kg_per_m3;

// Temperature units
static const double K = 1.0;

// Constants
static const double c = 299792458.0 * m / s;
static const double h = 6.626176e-34 * J * s;
static const double hbar = h / twopi;
static const double kB = 1.3806513e-23 * J / K;
static const double R = 8.3144621 * J / mole / K;

} // namespace nsx

#endif // NSX_BASE_UTILS_UNITS_H
