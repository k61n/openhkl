/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon Eric Pellegrini
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
#ifndef NSXTOOL_TIFFDATA_H_
#define NSXTOOL_TIFFDATA_H_

#include <memory>
#include "IData.h"
#include <tiffio.h>


namespace SX
{
namespace Data
{

class TiffData : public IData
{
public:
	TiffData(const std::string& filename, std::shared_ptr<Diffractometer> diffractometer, bool inMemory=false);
	virtual ~TiffData();
	void map();
	void unMap();
	void loadAllFrames();
	void releaseMemory();
	Eigen::MatrixXi getFrame(std::size_t frame);
	Eigen::MatrixXi readFrame(std::size_t frame) const;
private:
	//! Type of encoding for each pixel.
	uint16 _bits;
	//!
	TIFF* _file;
};

} // Namespace Data
} // Namespace SX

#endif /* NSXTOOL_TIFFDATA_H_ */
