/*
 * KernelFactory.h
 *
 *  Created on: Nov 14, 2016
 *      Author: pellegrini
 */

#ifndef NSXLIB_IMAGING_KERNELFACTORY_H_
#define NSXLIB_IMAGING_KERNELFACTORY_H_

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

#endif /* NSXLIB_IMAGING_KERNELFACTORY_H_ */
