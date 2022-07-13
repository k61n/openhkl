//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
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

#ifndef NSX_TABLES_CRYSTAL_DOUBLETOFRACTION_H
#define NSX_TABLES_CRYSTAL_DOUBLETOFRACTION_H

namespace ohkl {

// Converts a number into nominator and denominator.
void doubleToFraction(double number, int nmax, long& nom, long& dnom);

} // namespace ohkl

#endif // NSX_TABLES_CRYSTAL_DOUBLETOFRACTION_H
