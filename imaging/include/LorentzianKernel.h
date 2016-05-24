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

#ifndef NSXTOOL_INCLUDE_LORENTZIANKERNEL_H_
#define NSXTOOL_INCLUDE_LORENTZIANKERNEL_H_

#include "ConvolutionKernel.h"

namespace SX
{

namespace Imaging
{

class LorentzianKernel : public ConvolutionKernel
{
public:

	//! Default constructor (deleted)
	LorentzianKernel()=delete;

	//! Copy constructor
	LorentzianKernel(const LorentzianKernel& other);

	//! Construct a lorentzian kernel given its size and its corresponding parameters
	LorentzianKernel(int kernelSize, const std::map<std::string,double>& parameters);

	//! Destructor
	~LorentzianKernel();

	//! Update the kernel given its size and its parameters
	LorentzianKernel& operator=(const LorentzianKernel& other);

protected:

	void updateKernel();

};

} /* namespace Imaging */

} /* namespace SX */

#endif /* IMAGING_INCLUDE_LORENTZIANKERNEL_H_ */
