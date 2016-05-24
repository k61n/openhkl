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

#include <stdexcept>
#include <utility>

#include "ConvolutionKernel.h"

namespace SX
{

namespace Imaging
{

ConvolutionKernel::ConvolutionKernel(const ConvolutionKernel& other)
: _kernelSize(other._kernelSize),
  _parameters(other._parameters),
  _kernel(other._kernel)
{
}

ConvolutionKernel::ConvolutionKernel(int kernelSize, const std::map<std::string,double>& parameters)
{
	if (kernelSize <= 0 || (kernelSize%2==0))
		throw std::runtime_error("Invalid kernel size");

	_kernelSize = kernelSize;
	_parameters = parameters;
}

ConvolutionKernel::~ConvolutionKernel()
{
}

ConvolutionKernel& ConvolutionKernel::operator=(const ConvolutionKernel& other)
{
	if (this != &other)
	{
		_kernelSize = other._kernelSize;
		_parameters = other._parameters;
		_kernel = other._kernel;
	}
	return *this;
}

const Eigen::MatrixXd& ConvolutionKernel::getKernel() const
{
	return _kernel;
}

int ConvolutionKernel::getKernelSize() const
{
	return _kernelSize;
}

void ConvolutionKernel::setKernelSize(int kernelSize)
{
	if (kernelSize <= 0 ||  (kernelSize%2==0))
		throw std::runtime_error("Invalid kernel size");

	_kernelSize = kernelSize;
	updateKernel();
}

const std::map<std::string,double>& ConvolutionKernel::getParameters() const
{
	return _parameters;
}

void ConvolutionKernel::setParameters(const std::map<std::string,double>& parameters)
{
	_parameters = parameters;
	updateKernel();
}

Eigen::MatrixXd ConvolutionKernel::operator()(const Eigen::MatrixXd& image) const
{
	int imageNRows(image.rows());
	int imageNCols(image.cols());

	if (imageNRows<_kernelSize || imageNCols<_kernelSize)
		throw std::runtime_error("The image to filter is smaller than the kernel");

	int offset(_kernelSize/2);

	int offset2 = 2*offset;

	Eigen::MatrixXd resizedImage = Eigen::MatrixXd::Zero(imageNRows+offset2,imageNCols+offset2);
	resizedImage.block(offset,offset,imageNRows,imageNCols) = image;

	Eigen::MatrixXd convolutedImage = Eigen::MatrixXd::Zero(imageNRows,imageNCols);

	for (int c=0;c<imageNCols;++c)
	{
		for (int r=0;r<imageNRows;++r)
			convolutedImage(r,c) = (resizedImage.block(r,c,_kernelSize,_kernelSize)*_kernel.block(0,0,_kernelSize,_kernelSize)).sum();
	}

	return convolutedImage;
}

Eigen::MatrixXd ConvolutionKernel::apply(const Eigen::MatrixXd& image) const
{
	return ConvolutionKernel::operator()(image);
}

void ConvolutionKernel::print(std::ostream& os) const
{
	os<<"Kernel Matrix ("<<_kernelSize<<","<<_kernelSize<<"):"<<std::endl;
	os<<_kernel<<std::endl;
}

std::ostream& operator<<(std::ostream& os, const ConvolutionKernel& kernel)
{
	kernel.print(os);
	return os;
}

} /* namespace Imaging */

} /* namespace SX */
