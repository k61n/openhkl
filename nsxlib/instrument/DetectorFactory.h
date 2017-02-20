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

#ifndef NSXTOOL_DETECTORFACTORY_H_
#define NSXTOOL_DETECTORFACTORY_H_

#include <string>

#include "Detector.h"
#include "../kernel/Factory.h"
#include "../kernel/Singleton.h"

namespace SX {
namespace Instrument {

using SX::Instrument::Detector;
using SX::Kernel::Factory;
using SX::Kernel::Singleton;
using SX::Kernel::Constructor;
using SX::Kernel::Destructor;

class DetectorFactory : public Factory<Detector,std::string,const proptree::ptree&>, public Singleton<DetectorFactory,Constructor,Destructor>
{
private:
    friend class SX::Kernel::Constructor<DetectorFactory>;
    friend class SX::Kernel::Destructor<DetectorFactory>;
    DetectorFactory();
    ~DetectorFactory();
};

} // end namespace Instrument
} // end namespace SX

#endif /* NSXTOOL_DETECTORFACTORY_H_ */
