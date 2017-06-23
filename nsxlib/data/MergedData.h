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

#ifndef NSXLIB_MERGEDDATA_H
#define NSXLIB_MERGEDDATA_H

#include <Eigen/Core>
#include <set>

#include "../crystal/CrystalTypes.h"
#include "../crystal/MergedPeak.h"

namespace nsx {

class DataSet;

class MergedData {
public:
    MergedData(const SpaceGroup& grp, bool friedel);

    bool addPeak(const PeakCalc& peak);

    const MergedPeakSet& getPeaks() const;
    
private:
      SpaceGroup _group;
      bool _friedel;
      MergedPeakSet _peaks;
};

} // end namespace nsx

#endif // NSXLIB_MERGEDDATA_H
