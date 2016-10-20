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
    j.fisher[at]fz-juelich.de

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
#include <queue>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <iterator>

#include "Blob2D.h"
#include "Blob3D.h"
#include "Ellipsoid.h"
#include "IShape.h"
#include "NDTree.h"
#include "Types.h"

#include "IData.h"

#include "ProgressHandler.h"

namespace SX {

namespace Geometry
{

	typedef std::map<int,int> imap;
	typedef std::pair<int,int> ipair;
	typedef std::vector<int> vints;
	typedef std::vector<ipair> vipairs;
	typedef std::unordered_map<int,Blob2D> blob2DCollection;
	typedef std::unordered_map<int,Blob3D> blob3DCollection;

	typedef Ellipsoid<double,2> Ellipsoid2D;
	typedef Ellipsoid<double,3> Ellipsoid3D;
	typedef IShape<double,2> IShape2D;
	typedef IShape<double,3> IShape3D;
	typedef NDTree<double,2> Quadtree;
	typedef NDTree<double,3> Octree;
	typedef std::unordered_map<IShape2D*,int> shape2Dmap;
	typedef std::unordered_map<IShape3D*,int> shape3Dmap;


/* Class used for blob-finding, which is the first step of peak-finding.
 * The use of IFrameIterator allows for custom iterators, e.g. which work multi-threaded.
 * Since blob-finding may take some time on large data sets, ProgressHandler is used to give feedback to the GUI.
 *
 */
class BlobFinder {
public:
    using RealMatrix = SX::Types::RealMatrix;
    using FilterCallback = std::function<RealMatrix(const RealMatrix&)>;

    BlobFinder(std::shared_ptr<SX::Data::IData> data);

    blob3DCollection find(unsigned int begin, unsigned int end);

    void findBlobs(unsigned int begin, unsigned int end);
    void findBlobsSerial(unsigned int begin, unsigned int end);

    void registerEquivalence(int a, int b, vipairs& equivalences);

    static bool sortEquivalences(const ipair& pa, const ipair& pb);

    imap removeDuplicates(vipairs& equivalences);

    void reassignEquivalences(imap& equivalences);

    void findBlobs();

    //void mergeBlobs();

    //! sets progress handler callback function
    void setProgressHandler(std::shared_ptr<SX::Utils::ProgressHandler> handler);

    void setThreshold(double threshold);

    void setConfidence(double confidence);

    void setMedian(double median);

    void setMinComp(int minComp);

    void setMaxComp(int maxComp);

    void setRelative(bool isRelative);


    void findCollisions();

    //! Sets the filter, which allows for more sophisticated blob-finding
    void setFilter(FilterCallback callback);

    void mergeBlobs();


    void eliminateBlobs();



private:
    double _threshold;
    double _confidence;
    double _median;

    int _minComp;
    int _maxComp;

    bool _isRelative;

    std::shared_ptr<SX::Data::IData> _data;
    FilterCallback _filterCallback;
    std::shared_ptr<SX::Utils::ProgressHandler> _progressHandler;

    unsigned int _nrows, _ncols, _nframes;

    // filter
    // progress handler
    // ...

    std::unordered_map<int,Blob3D> _blobs;
    vipairs _equivalences;
};



    /*
	template <typename _datatype>
	blob2DCollection findBlobs2D(_datatype* dataptr, SX::Types::uint nrows, SX::Types::uint ncols, _datatype threshold, int minComp, int maxComp, double confidence, bool rowMajor=1)
	{
		// Map of Blobs (key : label, value : blob)
		blob2DCollection blobs;

		// Create a queue that will store the ncols previous blob labels
		std::queue<int> labels;
		for (unsigned int col=0;col<ncols;++col)
			labels.push(0);

		// Create empty equivalence table
		vipairs equivalences;
		equivalences.reserve(10000);

		int left, top; // Labels of the left and top pixels with respect to current one

		int currentlabel=0;
		int label;
		bool newlabel;
		// Iterate on all pixels in the image
		_datatype* datastart=dataptr;
		for (unsigned int row=0;row<nrows;++row)
		{
			if (!rowMajor)
				dataptr=datastart+row;
			for (unsigned int col=0;col<ncols;++col)
			{
				_datatype value=*(dataptr);
				if (!rowMajor)
				  dataptr+=nrows;
				else
				  dataptr++;
				// Discard pixel if value < threshold
				if (value<threshold)
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

		mergeBlobs<Blob2D>(blobs,equivalences);

		for (auto it=blobs.begin();it!=blobs.end();)
		{
			Blob2D& p=it->second;
			if (p.getComponents()<minComp || p.getComponents()>maxComp)
				it=blobs.erase(it);
			else
				it++;
		}

		// Determine the AABB of the blobs
		shape2Dmap boxes;
		boxes.reserve(blobs.size());

		Eigen::Vector2d center,extents;
		Eigen::Matrix2d axis;

		for (auto it=blobs.begin();it!=blobs.end();)
		{
			Blob2D& p=it->second;
			p.toEllipse(confidence,center,extents,axis);
			if (extents.minCoeff()<1.0e-9)
				it=blobs.erase(it);
			else
			{
				boxes.insert(shape2Dmap::value_type(new Ellipsoid2D(center,extents,axis),it->first));
				it++;
			}
		}


		Quadtree oct({0.0,0.0},{double(ncols),double(nrows)});
		oct.setMaxDepth(6);
		oct.setMaxStorage(6);

		for (auto it=boxes.begin();it!=boxes.end();++it)
			oct.addData(it->first);

		std::set<Quadtree::collision_pair> collisions;
		oct.getPossibleCollisions(collisions);


		// Clear the equivalence vectors for reuse purpose
		equivalences.clear();

		for (auto it=collisions.begin();it!=collisions.end();++it)
		{
			if (it->first->collide(*(it->second)))
			{
				auto bit1=boxes.find(it->first);
				auto bit2=boxes.find(it->second);
				registerEquivalence(bit1->second,bit2->second,equivalences);
			}
		}

		mergeBlobs<Blob2D>(blobs,equivalences);

		return blobs;
    } */

    /*
    // the typename matrix_iterator_t should be a forward iterator of type Eigen::Matrix
	template <typename matrix_iterator_t>
	blob3DCollection findBlobs3D(matrix_iterator_t it_begin, matrix_iterator_t it_end, double threshold, int minComp, int maxComp, double confidence)
	{
        

		// Number of frames
        int nframes = 0;

		// Map of Blobs (key : label, value : blob)
		std::unordered_map<int,Blob3D> blobs;
		blobs.reserve(1000000);

        // determine the number of rows and columns
        auto nrows = it_begin->rows();
        auto ncols = it_begin->cols();	

		// Store labels of current and previous frames.
		vints labels(nrows*ncols,0);
		vints labels2(nrows*ncols,0);

		// Create empty equivalence table
		vipairs equivalences;
		equivalences.reserve(100000);

		int left, top, previous; // Labels of the left and top pixels with respect to current one and the one above in previous frame

		int currentlabel=0;
		int label;
		bool newlabel;
		int index2D=0;

		// int representing the 8 possible nearest neighbor operations.
		int code;

        int frame = 0;

		// Iterate on all pixels in the image
        for (auto frame_it = it_begin; frame_it != it_end; ++frame_it)
		{
            ++nframes;
            auto& frame_data = *frame_it;
            
			// Go the the beginning of data
			index2D=0;
			for (unsigned int row=0;row<nrows;++row)
			{
				for (unsigned int col=0;col<ncols;++col)
				{
                    auto value = frame_data(row, col);

					// Discard pixel if value < threshold
					if (value<threshold)
					{
						labels[index2D]=labels2[index2D]=0;
						index2D++;
						continue;
					}
					newlabel=false;
					// Get labels of adjacent pixels
					left= (col == 0 ? 0 : labels[index2D-1]);
					top=  (row == 0 ? 0 : labels[index2D-ncols]) ;
					previous= (frame_it == it_begin ? 0 : labels2[index2D]);
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
						blobs.insert(blob3DCollection::value_type(label,Blob3D(col,row,frame,value)));
					}
					else
					{
						auto it=blobs.find(label);
						it->second.addPoint(col,row,frame,value);
					}
				}
			}

            // increase frame count
            // TODO: this could probably be done better!
            ++frame;

		}

        // too few frames for algorithm to be reliable
        if (nframes<=1)
			throw std::runtime_error("Third dimension should be at least 2 to run this algorithm. if 1, use 2D version");

		mergeBlobs<Blob3D>(blobs,equivalences);

		for (auto it=blobs.begin();it!=blobs.end();)
		{
			Blob3D& p=it->second;
			if (p.getComponents()<minComp || p.getComponents()>maxComp)
				it=blobs.erase(it);
			else
				it++;
		}


		// Determine the AABB of the blobs
		shape3Dmap boxes;
		boxes.reserve(blobs.size());

		Eigen::Vector3d center,extents;
		Eigen::Matrix3d axis;

		for (auto it=blobs.begin();it!=blobs.end();)
		{
			Blob3D& p=it->second;
			p.toEllipsoid(confidence,center,extents,axis);
			if (extents.minCoeff()<1.0e-9)
				it=blobs.erase(it);
			else
			{
				boxes.insert(shape3Dmap::value_type(new Ellipsoid3D(center,extents,axis),it->first));
				it++;
			}
		}


		Octree oct({0.0,0.0,0.0},{double(ncols),double(nrows),double(nframes)});
		oct.setMaxDepth(6);
		oct.setMaxStorage(6);

		for (auto it=boxes.begin();it!=boxes.end();++it)
			oct.addData(it->first);

		std::set<Octree::collision_pair> collisions;
		oct.getPossibleCollisions(collisions);


		// Clear the equivalence vectors for reuse purpose
		equivalences.clear();

		for (auto it=collisions.begin();it!=collisions.end();++it)
		{
			if (it->first->collide(*(it->second)))
			{
				auto bit1=boxes.find(it->first);
				auto bit2=boxes.find(it->second);
				registerEquivalence(bit1->second,bit2->second,equivalences);
			}
		}

		mergeBlobs<Blob3D>(blobs,equivalences);

		return blobs;
    } */



} // namespace Geometry

} // namespace SX

#endif /*NSXTOOL_BLOB_FINDER_H_*/

