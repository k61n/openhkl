/*
 * KernelFactory.h
 *
 *  Created on: Nov 14, 2016
 *      Author: pellegrini
 */

#ifndef NSXLIB_KERNELFACTORY_H
#define NSXLIB_KERNELFACTORY_H

#include "ConvolutionKernel.h"
#include "../kernel/Factory.h"
#include "../kernel/Singleton.h"

namespace nsx {

class KernelFactory :
        public Factory<ConvolutionKernel,std::string,int,int>,
        public Singleton<KernelFactory,Constructor,Destructor>
{
public:
    KernelFactory();
    virtual ~KernelFactory();
};

} // end namespace nsx

#endif // NSXLIB_KERNELFACTORY_H
