/*
 * KernelFactory.h
 *
 *  Created on: Nov 14, 2016
 *      Author: pellegrini
 */

#ifndef NSXLIB_IMAGING_KERNELFACTORY_H_
#define NSXLIB_IMAGING_KERNELFACTORY_H_

#include "ConvolutionKernel.h"
#include "Factory.h"
#include "Singleton.h"

namespace SX
{

namespace Imaging
{

using namespace SX::Kernel;

class KernelFactory : public Factory<ConvolutionKernel,std::string,int,int>, public Singleton<KernelFactory,Constructor,Destructor>
{
public:
	KernelFactory();
	virtual ~KernelFactory();
};

} // namespace Imaging

} // namespace SX

#endif /* NSXLIB_IMAGING_KERNELFACTORY_H_ */
