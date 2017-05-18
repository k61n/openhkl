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
#ifndef NSXLIB_INDEXER_H
#define NSXLIB_INDEXER_H

#include <memory>
#include "UnitCell.h"
#include "Peak3D.h"
#include <Eigen/Dense>
#include <vector>
#include <map>

namespace nsx {

typedef std::shared_ptr<UnitCell> ptrUnitCell;

class Indexer
{
public:
	Indexer(ptrUnitCell);

	Indexer(const Indexer& other);

	Indexer& operator=(const Indexer& other);

	~Indexer();
	//! Set the dspacing tolerance
	void setdSpacingTolerance(double percent);
	//! Set the tolerance for Angle comnparison between two reflections
	void setAngularTolerance(double percent);
	//! Return possible h,k,l for this peak based on dspacing and angle with stored peaks
	std::vector<Eigen::Vector3d> index(const Peak3D&);
	//! Store this peak as being indexed, this is used for angle comparison for next peaks.
	void storePeak(Peak3D*);
private:
	ptrUnitCell _cell;
	//! Tolerance in d-spacing, default 0.01
	double _dtol;
	//! Tolerance in angle, default 0.01
	double _angletol;
	//! Stored peaks already indexed
	std::vector<Peak3D*> _peaks;
	//!
    //std::multimap<double,Eigen::Vector3d> _possiblePeaks;
    std::vector<std::pair<double, Eigen::Vector3d>> _possiblePeaks;
};

} // end namespace nsx


#endif // NSXLIB_INDEXER_H
