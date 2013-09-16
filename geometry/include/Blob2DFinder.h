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


#include <unordered_map>
#include "Blob2D.h"
#include <queue>
#include <vector>
#include <algorithm>

namespace SX
{
namespace Geometry
{
	typedef std::unordered_map<int,Blob2D> blob2DCollection;

	//! Find blobs in a 2D image made of nrows*ncols data of type : _datatype. Blob are identified using a threashold,
	//! and a limits in the number of connected components in each blob (minComp, maxComp).
template <typename _datatype> blob2DCollection findBlobs(_datatype* dataptr,unsigned int nrows,unsigned int ncols, _datatype threashold, int minComp, int maxComp, bool rowMajor=1)
{

	// Map of Blobs (key : label, value : blob)
	blob2DCollection blobs;
	blobs.reserve(1000);

	// Change row and col. if necessary
	if (!rowMajor)
		std::swap(nrows,ncols);

	// Create a queue that will store the ncols previous blob labels
	std::queue<int> labels;
	for (unsigned int col=0;col<ncols;++col)
		labels.push(0);

	// Create empty equivalence table
	typedef std::pair<int,int> pairs;
	std::vector<pairs> equivalences;

	int left, top; // Labels of the left and top pixels with respect to current one

	int currentlabel=0;
	int label;
	bool newlabel;
	// Iterate on all pixels in the image
	for (unsigned int row=0;row<nrows;++row)
	{
		for (unsigned int col=0;col<ncols;++col)
		{
			_datatype value=*(dataptr++);
			// Discard pixel if value < threashold
			if (value<threashold)
			{
				labels.push(0);
				labels.pop();
				continue;
			}
			newlabel=false;
			// Get labels of adjacent pixels
			left= (col == 0 ? 0 : labels.back());
			top=labels.front();
			// If none of the neighbors have labels, create new one
			if (!(top || left))
			{
				label=++currentlabel;
				newlabel=true;
			}
			else if (top && left && (top!=left)) //two adjacent pixels have different labels
			{
				label=left;
				// Register equivalence (top and left pixels are equivalent ROI) with highest label first
				if (top<left)
					std::swap(top,left);

				equivalences.push_back(std::pair<int,int>(top,left));
			}
			else if (top) // only top has label
			{
				label=top;
			}
			else if (left) // only left has label
			{
				label=left;
			}
			labels.push(label);
			labels.pop();
			if (newlabel) // Create a new blob if necessary
			{
				blobs.insert(std::pair<int,Blob2D>(label,Blob2D(col,row,value)));
			}
			else
			{
				Blob2D& roi=blobs[label];
				roi.addPoint(col,row,value);
			}


		}
	}
	// Now deals with equivalences, first sort them
	std::sort(equivalences.begin(),
		      equivalences.end(),
		      [&](const std::pair<int,int>& a,const std::pair<int,int>& b)
		      {
				if (a.first<b.first)
					return true;
		        if (a.first>b.first)
		            return false;
		        return (a.second<b.second);
		       });

	auto beg=equivalences.begin();

	// Remove duplicates
	auto last=std::unique(equivalences.begin(),equivalences.end());

	// Assign the smallest label to the second element of each equivalence class
	for (auto it=beg;it!=last;++it)
	{
		for (auto it2=beg+1;it2!=last;++it2)
		{
			if (it2->second==it->first)
			it2->second=it->second;
		}
	}

	// Copy the equivalences into a map
	std::unordered_map<int,int> mequiv;
	for (auto it=equivalences.begin();it!=equivalences.end();++it)
		mequiv.insert(*it);

	// Iterate on blobs and merge equivalences
	for (auto it=blobs.begin();it!=blobs.end();)
	{
		auto match=mequiv.find(it->first);
		if (match==mequiv.end())
		{
		   it++; // Nothing is found get to the next blob
		}
		else
		{
		   auto tomerge=blobs.find(match->second);
		   if (tomerge!=blobs.end()) // Should never be the case
			{
				tomerge->second.merge(it->second);
				it=blobs.erase(it);
			}
		}
	}

	for (auto it=blobs.begin();it!=blobs.end();)
	{
		Blob2D& p=it->second;
		if (p.getComponents()<minComp || p.getComponents()>maxComp)
			it=blobs.erase(it);
		else
			it++;
	}


	return blobs;
}

} // End namespace Geometry
} // End namespace SX

