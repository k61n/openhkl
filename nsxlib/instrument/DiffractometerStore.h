/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon, Eric Pellegrini
 Institut Laue-Langevin
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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef NSXLIB_DIFFRACTOMETERSTORE_H
#define NSXLIB_DIFFRACTOMETERSTORE_H

#include <memory>
#include <map>
#include <set>
#include <string>

#include "../kernel/Singleton.h"

namespace nsx {

class Diffractometer;

typedef std::set<std::string> diffractometersList;
typedef std::shared_ptr<Diffractometer> sptrDiffractometer;
typedef std::map<std::string,sptrDiffractometer> diffractomersMap;

class DiffractometerStore : public Singleton<DiffractometerStore,Constructor,Destructor> {

public:

    //! Constructor
    DiffractometerStore();

    sptrDiffractometer buildDiffractometer(const std::string& name) const;

    diffractometersList getDiffractometersList() const;

};

} // end namespace nsx

#endif // NSXLIB_DIFFRACTOMETERSTORE_H
