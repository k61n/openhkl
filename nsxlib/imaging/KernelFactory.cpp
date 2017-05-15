/*
 * KernelFactory.cpp
 *
 *  Created on: Nov 14, 2016
 *      Author: pellegrini
 */

#include "AnnularKernel.h"
#include "ConstantKernel.h"
#include "DeltaKernel.h"
#include "KernelFactory.h"

namespace nsx {
namespace Imaging {

KernelFactory::KernelFactory()
{
    registerCallback("annular",&AnnularKernel::create);
    registerCallback("constant",&ConstantKernel::create);
    registerCallback("delta",&DeltaKernel::create);
}

KernelFactory::~KernelFactory()
{
}

} // Imaging
} // SX
