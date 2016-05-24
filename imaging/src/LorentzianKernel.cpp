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

#include "LorentzianKernel.h"

namespace SX
{

namespace Imaging
{

LorentzianKernel::LorentzianKernel(const LorentzianKernel& other) : ConvolutionKernel(other)
{
}

LorentzianKernel::LorentzianKernel(int kernelSize, const std::map<std::string,double>& parameters) : ConvolutionKernel(kernelSize,parameters)
{
	updateKernel();
}

LorentzianKernel::~LorentzianKernel()
{
}

LorentzianKernel& LorentzianKernel::operator=(const LorentzianKernel& other)
{
	if (this != &other)
		ConvolutionKernel::operator=(other);

	return *this;
}

void LorentzianKernel::updateKernel()
{

	auto it=_parameters.find("fwhm");

	if (it == _parameters.end())
		throw std::runtime_error("Missing parameters for this kernel: fwhm");

	_kernel = Eigen::MatrixXd(_kernelSize,_kernelSize);

	int offset(_kernelSize/2);

	double fwhm = _parameters["fwhm"];
	fwhm /= 2.0;

	for (int c=0;c<_kernelSize;++c)
	{
		double fc = static_cast<double>(c-offset);
		for (int r=c;r<_kernelSize;++r)
		{
			double fr = static_cast<double>(r-offset);
			_kernel(r,c) = _kernel(c,r) = fwhm/(fr*fr + fc*fc + fwhm*fwhm);
		}
	}

}

} /* namespace Imaging */

} /* namespace SX */
