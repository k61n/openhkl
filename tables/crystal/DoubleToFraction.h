//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      tables/crystal/DoubleToFraction.h
//! @brief     Defines function doubleToFraction
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_TABLES_CRYSTAL_DOUBLETOFRACTION_H
#define OHKL_TABLES_CRYSTAL_DOUBLETOFRACTION_H

namespace ohkl {

// Converts a number into numerator and denominator.
void doubleToFraction(double number, int nmax, long& nom, long& dnom);

} // namespace ohkl

#endif // OHKL_TABLES_CRYSTAL_DOUBLETOFRACTION_H
