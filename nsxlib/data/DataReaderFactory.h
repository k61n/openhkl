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

#include "Factory.h"
#include "IData.h"
#include "Singleton.h"
#include <memory>
#include "Diffractometer.h"

namespace SX
{

namespace Data
{

using namespace SX::Kernel;

typedef typename std::shared_ptr<SX::Instrument::Diffractometer> ptrInstrument;

/** \brief DataReaderFactory. All IData formats must register their "create" method with the factory in order to
 * choose the correct DataReader at runtime. Reader selection is based on the extension of the datafile.
 *
 */
class DataReaderFactory :
        public Factory<IData,std::string,std::string,std::shared_ptr<SX::Instrument::Diffractometer> >,
        public Singleton<DataReaderFactory,Constructor,Destructor>
{
private:
    friend class Constructor<DataReaderFactory>;
    friend class Destructor<DataReaderFactory>;
    DataReaderFactory();
    ~DataReaderFactory() = default;
};

} // end namespace Data

} // end namespace SX

#endif /* NSXTOOL_READERFACTORY_H_ */
