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

#ifndef NSXTOOL_I16DATA_H_
#define NSXTOOL_I16DATA_H_

#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <Eigen/Dense>
#include <IData.h>
#include "Diffractometer.h"
#include "TiffData.h"

namespace SX
{

namespace Data
{
/*! \brief Legacy ILL Data in ASCII format.
 *
 */
class I16Data : public IData
{
public:

	static IData* create(const std::string& filename, std::shared_ptr<Diffractometer> diffractometer);

	//! Default constructor
	I16Data(const std::string& filename, std::shared_ptr<Diffractometer> diffractometer);
	//! Copy constructor
	I16Data(const I16Data& other)=delete;
	//! Destructor
	virtual ~I16Data();
	// Operators
	//! Assignment operator
	I16Data& operator=(const I16Data& other)=delete;

	// Other methods
	void open() override;
	void close() override;
    //! Read a single frame
    Eigen::MatrixXi readFrame(std::size_t idx) override;

private:
	//! Vector of all TIFF files.
	std::vector<std::string> _tifs;
	std::string _basedirectory;
};

} // end namespace Data

} // end namespace SX

#endif /* NSXTOOL_I16DATA_H_ */
