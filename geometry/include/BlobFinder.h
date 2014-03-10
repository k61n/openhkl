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
#ifndef NSXTOOL_BLOB_FINDER_H_
#define NSXTOOL_BLOB_FINDER_H_

#include <algorithm>
#include <map>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "Blob2D.h"
#include "Blob3D.h"

namespace SX
{

namespace Geometry
{
	typedef unsigned int uint;
	typedef std::map<int,int> imap;
	typedef std::pair<int,int> ipair;
	typedef std::vector<int> vints;
	typedef std::vector<ipair> vipairs;
	typedef std::unordered_map<int,Blob2D> blob2DCollection;
	typedef std::unordered_map<int,Blob3D> blob3DCollection;

	void registerEquivalence(int a, int b, vipairs& equivalences);

	bool sortEquivalences(const ipair& pa, const ipair& pb);

	imap removeDuplicates(vipairs& equivalences);

	void reassignEquivalences(imap& equivalences);

	void mergeBlobs(blob3DCollection& blobs, vipairs& equivalences);

	template <typename _datatype>
	blob2DCollection findBlobs2D(_datatype* dataptr, uint nrows, uint ncols, _datatype threshold, int minComp, int maxComp, double confidence, bool rowMajor=1);

	template <typename _datatype>
	blob3DCollection findBlobs3D(const std::vector<_datatype*>& ptrs, uint nrows, uint ncols, _datatype threshold, int minComp, int maxComp, double confidence, bool rowMajor=1);

} // namespace Geometry

} // namespace SX

#endif /*NSXTOOL_BLOB_FINDER_H_*/

