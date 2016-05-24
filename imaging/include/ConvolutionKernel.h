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

#ifndef NSXTOOL_CONVOLUTIONKERNEL_H_
#define NSXTOOL_CONVOLUTIONKERNEL_H_

#include <map>
#include <stdexcept>
#include <string>
#include <ostream>

#include <Eigen/Dense>

namespace SX
{

namespace Imaging
{

class ConvolutionKernel
{

public:

	//! Default constructor (deleted)
	ConvolutionKernel()=delete;

	//! Copy constructor
	ConvolutionKernel(const ConvolutionKernel& other);

	//! Construct a convolution kernel given its size and its corresponding parameters
	ConvolutionKernel(int kernelSize, const std::map<std::string,double>& parameters);

	//! Assignment operator
	ConvolutionKernel& operator=(const ConvolutionKernel& other);

	//! Return the size of the kernel
	int getKernelSize() const;
	//! Set the size of the kernel
	void setKernelSize(int kernelSize);

	//! Return a constant reference to the kernel matrix
	const Eigen::MatrixXd& getKernel() const;

	//! Return a constant reference to the parameters of the convolution kernel
	const std::map<std::string,double>& getParameters() const;
	//! Seto the parameters of the convolution kernel
	void setParameters(const std::map<std::string,double>& parameters);

	//! Call operator
	Eigen::MatrixXd operator()(const Eigen::MatrixXd& image) const;
	//! Apply the convolution kernel to an image
	Eigen::MatrixXd apply(const Eigen::MatrixXd& image) const;

	//! Destructor
	virtual ~ConvolutionKernel()=0;

	//! Flush information about the convolution kernel to an output stream
	void print(std::ostream& os) const;

protected:

	//! Update the kernel given its size and its parameters
	virtual void updateKernel()=0;

protected:

	int _kernelSize;

	std::map<std::string,double> _parameters;

	Eigen::MatrixXd _kernel;

};

std::ostream& operator<<(std::ostream& os, const ConvolutionKernel& kernel);

} /* namespace Imaging */

} /* namespace SX */

#endif /* NSXTOOL_CONVOLUTIONKERNEL_H_ */
