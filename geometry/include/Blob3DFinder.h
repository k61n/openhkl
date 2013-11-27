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
#ifndef __BLOB_3D
#define __BLOB_3D
#include <unordered_map>
#include <vector>
#include "Blob3D.h"
#include <algorithm>
#include <stdexcept>
#include <map>
#include "AABB.h"
#include "NDTree.h"

namespace SX
{
namespace Geometry
{
	typedef std::unordered_map<int,Blob3D> blob3DCollection;
	typedef std::vector<std::pair<int,int> > pairints;
	typedef AABB<double,3> AABB3D;
	typedef NDTree<double,3> Octree;


	inline void registerEquivalence(int a, int b,std::vector<std::pair<int,int> >& pairs)
	{
		if (a<b)
			pairs.push_back(pairints::value_type(b,a));
		else
			pairs.push_back(pairints::value_type(a,b));

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
		blobs.reserve(1000000);

		// Store labels of current and previous frames.
		std::vector<int> labels(nrows*ncols,0);
		std::vector<int> labels2(nrows*ncols,0);

		// Create empty equivalence table
		typedef std::pair<int,int> pairs;
		std::vector<pairs> equivalences;
		equivalences.reserve(100000);

		int left, top, previous; // Labels of the left and top pixels with respect to current one and the one above in previous frame

		int currentlabel=0;
		int label;
		bool newlabel;
		int index2D=0;

		// int representing the 8 possible nearest neighbor operations.
		int code;

		//
		int row_inc=(rowMajor ? 1 : nrows);

		// Iterate on all pixels in the image
		for (int frame=0;frame<nframes;++frame)
		{
			// Go the the beginning of data
			_datatype* datastart=ptrs[frame];
			_datatype* dataptr=datastart;
			index2D=0;
			for (unsigned int row=0;row<nrows;++row)
			{
				if (!rowMajor)
					dataptr=datastart+row;
				for (unsigned int col=0;col<ncols;++col)
				{
					_datatype value=*(dataptr);
					dataptr+=row_inc;
					// Discard pixel if value < threashold
					if (value<threashold)
					{
						labels[index2D]=labels2[index2D]=0;
						index2D++;
						continue;
					}
					newlabel=false;
					// Get labels of adjacent pixels
					left= (col == 0 ? 0 : labels[index2D-1]);
					top=  (row == 0 ? 0 : labels[index2D-ncols]) ;
					previous= (frame == 0 ? 0 : labels2[index2D]);
					// Encode type of config.
					code=0;
					code |= ( (left!=0) << 0);
					code |= ( (top!=0) << 1);
					code |= ( (previous!=0)  << 2);

					switch (code) {
						case 0:
							label=++currentlabel;
							newlabel=true;
							break;
						case 1: // Only left pixel
							label=left;
							break;
						case 2: // Only top pixel
							label=top;
							break;
						case 3: // Top and left
							label=top;
							if (top!=left)
								registerEquivalence(top,left,equivalences);
							break;
						case 4: // Only previous
							label=previous;
							break;
						case 5: // Left and previous
							label=left;
							if (left!=previous)
								registerEquivalence(left,previous,equivalences);
							break;
						case 6: // Top and previous
							label=top;
							if (top!=previous)
								registerEquivalence(top,previous,equivalences);
							break;
						case 7: // All three
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
							break;
						default:
							break;
					}
					// If none of the neighbors have labels, create new one

					labels[index2D]=labels2[index2D]=label;
					index2D++;
					//
					if (newlabel) // Create a new blob if necessary
					{
						blobs.insert(blob3DCollection::value_type(label,Blob3D(col,row,frame,1)));
					}
					else
					{
						auto it=blobs.find(label);
						it->second.addPoint(col,row,frame,1);
					}


				}
			}
		// Now deals with equivalences, first sort them
		}
		//

		std::sort(equivalences.begin(),
			      equivalences.end(),
			      [&](const std::pair<int,int> a,const std::pair<int,int> b)
			      {
					if (a.first<b.first)
						return true;
			        if (a.first>b.first)
			            return false;
			        return (a.second<b.second);
			       });



		// Remove duplicates
		auto beg=equivalences.begin();
		auto last=std::unique(equivalences.begin(),equivalences.end());

		// Copy to a map. This can't be an unordered_map in this case, ordering is important
		std::map<int,int> mequiv;
		for (auto it=beg;it!=last;++it)
					mequiv.insert(*it);
		// Reassign labels
		for (auto it=mequiv.begin();it!=mequiv.end();++it)
		{
			auto found=mequiv.find(it->second);
			if (found!=mequiv.end())
				it->second=found->second;
		}

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

		std::cout << "Found the initial peaks :" << blobs.size() << "\n";

		// Determine the AABB of the blobs
		typedef std::unordered_map<int,AABB3D> mapbox;
		mapbox boxes;
		boxes.reserve(blobs.size());

		V3D center,semi_axes, v0, v1,v2;
		V3D hw;

		for (auto it=blobs.begin();it!=blobs.end();++it)
		{
			Blob3D& p=it->second;
			p.toEllipsoid(center,semi_axes,v0,v1,v2);
			double w=std::pow(semi_axes[0]*v0[0],2)+std::pow(semi_axes[1]*v1[0],2)+std::pow(semi_axes[2]*v2[0],2);
			hw[0]=3.0*sqrt(w);
			double h=std::pow(semi_axes[0]*v0[1],2)+std::pow(semi_axes[1]*v1[1],2)+std::pow(semi_axes[2]*v2[1],2);
			hw[1]=3.0*sqrt(h);
			double d=std::pow(semi_axes[0]*v0[2],2)+std::pow(semi_axes[1]*v1[2],2)+std::pow(semi_axes[2]*v2[2],2);
			hw[2]=3.0*sqrt(d);
			V3D low=center-hw;
			V3D high=center+hw;
			boxes.insert(mapbox::value_type(it->first,AABB3D({low[0],low[1],low[2]},{high[0],high[1],high[2]})));
		}


		Octree oct({0.0,0.0,0.0},{640.0,256.0,1112.0});
		oct.setMaxDepth(6);
		oct.setMaxStorage(6);

		for (auto it=boxes.begin();it!=boxes.end();++it)
		{
			oct.addData(&(it->second));
		}


		std::set<Octree::collision_pair> collisions;

		oct.getPossibleCollisions(collisions);

		std::cout << " Number of possible intersections" << collisions.size() << std::endl;


		return blobs;
}



} // End namespace Geometry
} // End namespace SX

#endif
