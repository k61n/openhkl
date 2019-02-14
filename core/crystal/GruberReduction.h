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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
 *
 */

#pragma once

#include <string>

#include <Eigen/Dense>

#include "NiggliCharacter.h"
#include "UnitCell.h"

namespace nsx {

//! \class GruberReduction
//! \brief Classify and reduce a lattice according to the 44 types in the
//! international tables.
class GruberReduction {
public:
  //!! Construct algorithm with the metric tensor of the Cell, and a tolerance
  GruberReduction(const Eigen::Matrix3d &g, double epsilon);
  //! Find the conventional cell and output transformation matrix, centring type
  //! and Bravais lattice of the new cell. Return value is the condition which
  //! matched (1-44)
  int reduce(Eigen::Matrix3d &P, LatticeCentring &centring,
             BravaisType &bravais);
  //! Classify the lattice as one of the 44 types listed in the international
  //! tables. The return value contains all relevant information about the
  //! class.
  NiggliCharacter classify();

private:
  bool equal(double A, double B) const;
  Eigen::Matrix3d _g;
  double _epsilon;
};

} // end namespace nsx
