//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/crystal/DoubleToFraction.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_UTILS_DOUBLETOFRACTION_H
#define CORE_UTILS_DOUBLETOFRACTION_H

//@ #include <cmath>

namespace nsx {

// Convert number into nominator and denominator
void doubleToFraction(double number, int nmax, long& nom, long& dnom);

} // namespace nsx

#endif // CORE_UTILS_DOUBLETOFRACTION_H
