/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
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

#ifndef NSXTOOL_PEAKCOLLECTION_H_
#define NSXTOOL_PEAKCOLLECTION_H_

#include "IROI.h"
#include "Peak.h"

namespace SX
{

namespace Geometry
{

typedef unsigned int uint;

template<typename TYPE, typename DATATYPE, uint DIM>
class PeakCollection : public IROI<TYPE,DATATYPE,DIM>
{
public:

	PeakCollection();

	//! return whether or not a given point is inside one of the peak of the collection
	virtual bool is_inside(const std::initializer_list<TYPE>& point) const;

	//! add a peak to the collection
	void add_peak(Peak<TYPE,DATATYPE,DIM>* peak);

private:
	//! store pointers to the peaks
	std::vector<Peak<TYPE,DATATYPE,DIM>*> _peaks;

};

template<typename TYPE, typename DATATYPE, uint DIM>
PeakCollection<TYPE,DATATYPE,DIM>::PeakCollection() : IROI<TYPE,DATATYPE,DIM>()
{
	_peaks.reserve(10);
}

template<typename TYPE, typename DATATYPE, uint DIM>
void PeakCollection<TYPE,DATATYPE,DIM>::add_peak(Peak<TYPE,DATATYPE,DIM>* peak)
{
	_peaks.push_back(peak);
}

template<typename TYPE, typename DATATYPE, uint DIM>
bool PeakCollection<TYPE,DATATYPE,DIM>::is_inside(const std::initializer_list<TYPE>& point) const
{
	for (auto it=_peaks.begin(); it!=_peaks.end(); ++it)
	{
		if ((*it)->is_inside(point))
			return true;
	}
	return false;

}

} // namespace Geometry

} // namespace SX

#endif // NSXTOOL_PEAKCOLLECTION_H_
