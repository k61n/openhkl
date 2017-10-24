#pragma once

#include "ConvolutionKernel.h"
#include "Factory.h"
#include "Singleton.h"

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
