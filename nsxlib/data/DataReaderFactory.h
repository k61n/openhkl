/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon Institut Laue-Langevin
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

#ifndef NSXTOOL_DATAREADERFACTORY_H_
#define NSXTOOL_DATAREADERFACTORY_H_

#include <string>

#include "../kernel/Factory.h"
#include "IData.h"
#include "../kernel/Singleton.h"
#include <memory>
#include "../instrument/Diffractometer.h"

namespace nsx {
namespace Data {

using nsx::Kernel::Factory;
using nsx::Kernel::Constructor;
using nsx::Kernel::Destructor;
using nsx::Kernel::Singleton;
using ptrInstrument = typename std::shared_ptr<nsx::Instrument::Diffractometer>;

/** \brief DataReaderFactory. All IData formats must register their "create" method with the factory in order to
 * choose the correct DataReader at runtime. Reader selection is based on the extension of the datafile.
 *
 */
class DataReaderFactory :
        public Factory<DataSet,std::string,std::string,std::shared_ptr<nsx::Instrument::Diffractometer> >,
        public Singleton<DataReaderFactory,Constructor,Destructor>
{
private:
    friend class nsx::Kernel::Constructor<DataReaderFactory>;
    friend class nsx::Kernel::Destructor<DataReaderFactory>;
    DataReaderFactory();
    ~DataReaderFactory() = default;
};

} // end namespace Data
} // end namespace nsx

#endif /* NSXTOOL_READERFACTORY_H_ */
