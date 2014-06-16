/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon Institut Laue-Langevin
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

#ifndef NSXTOOL_DETECTORMAPPING_H_
#define NSXTOOL_DETECTORMAPPING_H_

#include <stdlib.h>

namespace SX
{

namespace Instrument
{

typedef unsigned int uint;

class DetectorMapping
{
public:

	DetectorMapping();

	//! Constructs a mapping that will match 1D indexes with 2D pixels.
	DetectorMapping(const uint nRows, const uint nCols, const uint startingIndex=0, bool rowMajor=true);

	//! Maps a 1D index with a 2D pixel.
	void operator()(uint idx, uint& px, uint& py) const;

	~DetectorMapping();

private:
	//! The number of rows of the detector.
	uint _nRows;
	//! The number of cols of the detector.
	uint _nCols;
	//! The 1D index at which starts the detector (maybe > 0 in case of multi-detector setup).
	uint _startingIndex;
	//! Indicates whether or not the data are row- or column-major ordered.
	bool _rowMajor;
	uint _blockSize;
};

} // end namespace Instrument

} // end namespace SX

#endif /* NSXTOOL_DETECTORMAPPING_H_ */
