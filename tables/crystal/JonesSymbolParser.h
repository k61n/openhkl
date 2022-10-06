//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      tables/crystal/JonesSymbolParser.h
//! @brief     Defines function parseJonesSymbol
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_TABLES_CRYSTAL_JONESSYMBOLPARSER_H
#define OHKL_TABLES_CRYSTAL_JONESSYMBOLPARSER_H

#include <Eigen/Dense>

namespace ohkl {

//! Function to parse Jones faithful representation of affine transformations.

//! Input written as strings of the form 2x+1/3,y-z+1,x+1/4.
//! Return an affine transform matrix which represents the rotational and
//! translational parts of the symmetry operator.
//! This parser only validates the grammar but not the symmetry operation itself.
//! Valid terms are signed linear terms in x, y, or z
//! and constants,either integer or fractional numbers written as n/d. For
//! example, the symbol x,y,z will be parsed as the identity matrix and no
//! translational part. The symbol -y,-x,z+1/2 will be parsed into the matrix\n
//!  0,-1, 0, 0\n
//!  1, 0, 0, 0\n
//!  0, 0, 1, 0.5,\n
//!  0, 0, 0, 1

Eigen::Transform<double, 3, Eigen::Affine> parseJonesSymbol(const std::string& jonesSymbol);

} // namespace ohkl

#endif // OHKL_TABLES_CRYSTAL_JONESSYMBOLPARSER_H
