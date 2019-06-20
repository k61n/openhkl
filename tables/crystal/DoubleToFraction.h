//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      tables/crystal/DoubleToFraction.h
//! @brief     Defines function doubleToFraction
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_CRYSTAL_DOUBLETOFRACTION_H
#define CORE_CRYSTAL_DOUBLETOFRACTION_H

namespace nsx {

// Convert number into nominator and denominator
void doubleToFraction(double number, int nmax, long& nom, long& dnom);

} // namespace nsx

#endif // CORE_CRYSTAL_DOUBLETOFRACTION_H
