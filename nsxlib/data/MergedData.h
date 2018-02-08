/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2017- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
          Jonathan Fisher, Forschungszentrum Juelich GmbH
    BP 156
    6, rue Jules Horowitz
    38042 Grenoble Cedex 9
    France
    chapon[at]ill.fr
    pellegrini[at]ill.fr
    j.fisher[at]fz-juelich.de

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

#pragma once

#include <set>

#include <Eigen/Dense>

#include "CrystalTypes.h"
#include "MergedPeak.h"

namespace nsx {

class DataSet;

//! \class MergedData
//! \brief Class to handle merged datasets.
class MergedData {
public:
    //! Construct merged dataset with given spacegroup. Paramter \p friedel determines whether
    //! to include the Friedel relation \f$q \mapsto -q\f$, if this is not already part of the
    //! space group symmetry.  
    MergedData(const SpaceGroup& grp, bool friedel);
    //! Add a peak to the data set. It will be automatically merged correctly according to
    //! the space group symmetry.
    bool addPeak(const sptrPeak3D& peak);
    //! Retern the set of merged peaks.
    const MergedPeakSet& getPeaks() const;

    //! Return redundancy = #observations / #symmetry-inequvialent peaks.
    double redundancy() const;

    //! Return total number of observations (including redundant ones)
    size_t totalSize() const;
    
private:
    SpaceGroup _group;
    bool _friedel;
    MergedPeakSet _peaks;
};

} // end namespace nsx
