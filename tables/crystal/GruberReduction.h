//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      tables/crystal/GruberReduction.h
//! @brief     Defines class GruberReduction
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_TABLES_CRYSTAL_GRUBERREDUCTION_H
#define NSX_TABLES_CRYSTAL_GRUBERREDUCTION_H

#include "tables/crystal/UnitCell.h"

namespace nsx {

//! Classifies and reduces a lattice according to the 44 types in the international tables.

class GruberReduction {
 public:
    //!! Construct algorithm with the metric tensor of the Cell, and a tolerance
    GruberReduction(const Eigen::Matrix3d& g, double epsilon);
    //! Find the conventional cell and output transformation matrix, centring type
    //! and Bravais lattice of the new cell. Return value is the matched case number (1-44).
    int reduce(Eigen::Matrix3d& P, LatticeCentring& centring, BravaisType& bravais);
    //! Classify the lattice as one of the 44 types listed in the international
    //! tables. The return value contains all relevant information about the class.
    NiggliCharacter classify();

 private:
    bool equal(double A, double B) const;
    Eigen::Matrix3d _g;
    double _epsilon;
};

} // namespace nsx

#endif // NSX_TABLES_CRYSTAL_GRUBERREDUCTION_H
