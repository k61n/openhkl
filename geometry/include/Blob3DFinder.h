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
#ifndef __BLOB_3D
#define __BLOB_3D
#include <map>
#include <vector>
#include "Blob3D.h"
#include <queue>
#include <algorithm>
#include <stdexcept>

namespace SX
{
namespace Geometry
{
	typedef std::map<int,Blob3D> blob3DCollection;

	void registerEquivalence(int a, int b,std::vector<std::pair<int,int> >& pairs)
	{
		if (a<b)
			std::swap(a,b);
		pairs.push_back(std::pair<int,int>(a,b));
	}
	//! Find blobs in a 2D image made of nrows*ncols data of type : _datatype. Blob are identified using a threashold,
	//! and a limits in the number of connected components in each blob (minComp, maxComp).
	template <typename _datatype> blob3DCollection findBlobs3D(const std::vector<_datatype*>& ptrs,unsigned int nrows,unsigned int ncols, _datatype threashold, int minComp, int maxComp, bool rowMajor=1)
	{

		// Number of frames
		int nframes=ptrs.size();
		if (nframes<=1)
			throw std::runtime_error("Third dimension should be at least 2 to run this algorithm. if 1, use 2D version");

		// Map of Blobs (key : label, value : blob)
		blob3DCollection blobs;

		// Change row and col. if necessary
		if (!rowMajor)
			std::swap(nrows,ncols);

		// Create a queue that will store the blob labels for previous frame.
		std::queue<int> labels;
		for (unsigned int i=0;i<ncols*nrows;++i)
			labels.push(0);

		// Create a queue for labels of previous line.
		std::queue<int> labels2;
		for (unsigned int i=0;i<ncols;++i)
			labels2.push(0);

		// Create empty equivalence table
		typedef std::pair<int,int> pairs;
		std::vector<pairs> equivalences;
		equivalences.reserve(10000);

		int left, top, previous; // Labels of the left and top pixels with respect to current one and the one above in previous frame

		int currentlabel=0;
		int label;
		bool newlabel;
		// Iterate on all pixels in the image
		for (int frame=0;frame<nframes;++frame)
		{
			// Go the the beginning of data
			_datatype* dataptr=ptrs[frame];
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
						labels2.push(0);
						labels2.pop();
						continue;
					}
					newlabel=false;
					// Get labels of adjacent pixels
					left= (col == 0 ? 0 : labels.back());
					previous=labels.front();
					top=labels2.front();
					// If none of the neighbors have labels, create new one
					if (!(top || left || previous))
					{
						label=++currentlabel;
						newlabel=true;
					}
					else if (top && left && previous) //three adjacent pixels have values
					{
						label=left;
						if ((top==left) && (top!=previous))
							registerEquivalence(top,previous,equivalences);
						else if ((top==previous) && (top!=left))
							registerEquivalence(top,left,equivalences);
						else if ((left==previous) && (left!=top))
							registerEquivalence(left,top,equivalences);
						else if ((left!=previous) && (left!=top) && (top!=previous))
						{
							registerEquivalence(top,previous,equivalences);
							registerEquivalence(top,left,equivalences);
							registerEquivalence(left,previous,equivalences);
						}
					}
					else if (top && left && (!previous))
					{
						label=top;
						if (top!=left)
							registerEquivalence(top,left,equivalences);
					}
					else if (top && previous && (!left))
					{
						label=top;
						if (top!=previous)
							registerEquivalence(top,previous,equivalences);
					}
					else if (left && previous && (!top))
					{
						label=left;
						if (left!=previous)
							registerEquivalence(left,previous,equivalences);
					}
					else if (left) // only left has label
					{
						label=left;
					}
					else if (top) // only left has label
					{
						label=top;
					}
					else if (previous) // only left has label
					{
						label=previous;
					}
					labels.push(label);
					labels.pop();
					labels2.push(label);
					labels2.pop();
					if (newlabel) // Create a new blob if necessary
					{
						blobs.insert(std::pair<int,Blob3D>(label,Blob3D(col,row,frame,value)));
					}
					else
					{
						Blob3D& roi=blobs[label];
						roi.addPoint(col,row,frame,value);
					}


				}
			}
		// Now deals with equivalences, first sort them
		}
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
		std::map<int,int> mequiv;
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
			Blob3D& p=it->second;
			if (p.getComponents()<minComp || p.getComponents()>maxComp)
				it=blobs.erase(it);
			else
				it++;
		}

		return blobs;
}




} // End namespace Geometry
} // End namespace SX

#endif
