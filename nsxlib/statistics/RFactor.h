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

#ifndef NSXLIB_RFACTOR_H
#define NSXLIB_RFACTOR_H

#include <vector>
#include <set>

#include "../crystal/CrystalTypes.h"

namespace nsx {

    class MergedData;

class RFactor {
public:
    RFactor(): _Rmerge(0.0), _Rmeas(0.0), _Rpim(0.0) {}
    ~RFactor() {}

    void calculate(const MergedData& data);

    double Rmerge() {return _Rmerge;}
    double Rmeas() {return _Rmeas;}
    double Rpim() {return _Rpim;}


private:
    double _Rmerge, _Rmeas, _Rpim;
};

} // end namespace nsx

#endif // NSXLIB_RFACTOR_H
