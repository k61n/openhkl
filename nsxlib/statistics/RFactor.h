/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2016- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher
 Institut Laue-Langevin
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
#include <vector>

#include "CrystalTypes.h"
#include "IMergedPeakStatistic.h"

namespace nsx {

class MergedData;

//! \brief Class used to compute the various R factors. 
class RFactor: public IMergedPeakStatistic {
public:
    RFactor(): 
        _Rmerge(0.0), _Rmeas(0.0), _Rpim(0.0),
        _expectedRmerge(0.0), _expectedRmeas(0.0), _expectedRpim(0.0) {}
    ~RFactor() {}

    void calculate(const MergedData& data);

    double Rmerge() {return _Rmerge;}
    double Rmeas() {return _Rmeas;}
    double Rpim() {return _Rpim;}

    double expectedRmerge() {return _expectedRmerge;}
    double expectedRmeas() {return _expectedRmeas;}
    double expectedRpim() {return _expectedRpim;}


private:
    double _Rmerge, _Rmeas, _Rpim;
    double _expectedRmerge, _expectedRmeas, _expectedRpim;
};

} // end namespace nsx
