#include "BlobFinder.h"
#include "Ellipsoid.h"
#include "IShape.h"
#include "NDTree.h"

namespace SX
{

namespace Geometry
{

	typedef Ellipsoid<double,2> Ellipsoid2D;
	typedef Ellipsoid<double,3> Ellipsoid3D;
	typedef IShape<double,2> IShape2D;
	typedef IShape<double,3> IShape3D;
	typedef NDTree<double,2> Quadtree;
	typedef NDTree<double,3> Octree;
	typedef std::unordered_map<IShape2D*,int> shape2Dmap;
	typedef std::unordered_map<IShape3D*,int> shape3Dmap;

	void registerEquivalence(int a, int b, vipairs& equivalences)
	{
		if (a < b)
			equivalences.push_back(vipairs::value_type(b,a));
		else
			equivalences.push_back(vipairs::value_type(a,b));
	}

	bool sortEquivalences(const ipair& pa, const ipair& pb)
	{
		if (pa.first<pb.first)
			return true;
		if (pa.first>pb.first)
			return false;
		return (pa.second<pb.second);
	}

	imap removeDuplicates(vipairs& equivalences)
	{
		auto beg=equivalences.begin();
		auto last=std::unique(equivalences.begin(),equivalences.end());

		imap mequiv;
		for (auto it=beg;it!=last;++it)
			mequiv.insert(*it);

		return mequiv;
	}

	void reassignEquivalences(imap& equivalences)
	{
		for (auto it=equivalences.begin();it!=equivalences.end();++it)
		{
			auto found=equivalences.find(it->second);
			if (found!=equivalences.end())
				it->second=found->second;
		}
	}

	void mergeBlobs(blob3DCollection& blobs, vipairs& equivalences)
	{

		// Sort the equivalences pair by ascending order of the first element and if equal by ascending order of their second element.
		std::sort(equivalences.begin(), equivalences.end(),sortEquivalences);

		// Remove the duplicate pairs
		imap mequiv=removeDuplicates(equivalences);

		reassignEquivalences(mequiv);

		// Iterate on blobs and merge equivalences
		for (auto it=blobs.begin();it!=blobs.end();)
		{
			auto match=mequiv.find(it->first);
			if (match==mequiv.end())
			{
				// Nothing is found get to the next blob
				it++;
			}
			else
			{
				auto tomerge=blobs.find(match->second);
				// Should never be the case
				if (tomerge!=blobs.end())
				{
					tomerge->second.merge(it->second);
					it=blobs.erase(it);
				}
			}
		}
	}

	template <typename _datatype>
	blob2DCollection findBlobs2D(_datatype* dataptr, uint nrows, uint ncols, _datatype threshold, int minComp, int maxComp, double confidence, bool rowMajor=1)
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

		mergeBlobs(blobs,equivalences);

		for (auto it=blobs.begin();it!=blobs.end();)
		{
			Blob2D& p=it->second;
			if (p.getComponents()<minComp || p.getComponents()>maxComp)
				it=blobs.erase(it);
			else
				it++;
		}

		std::cout << "Found the initial peaks :" << blobs.size() << "\n";

		// Determine the AABB of the blobs
		shape2Dmap boxes;
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
				boxes.insert(shape2Dmap::value_type(new Ellipsoid2D(center,extents,axis),it->first));
				it++;
			}
		}

		std::cout << "Reduced initial peaks to:" << boxes.size() << "\n";

		Quadtree oct({0.0,0.0},{double(ncols),double(nrows)});
		oct.setMaxDepth(6);
		oct.setMaxStorage(6);

		for (auto it=boxes.begin();it!=boxes.end();++it)
			oct.addData(it->first);

		std::set<Quadtree::collision_pair> collisions;
		oct.getPossibleCollisions(collisions);

		std::cout << "Number of possible collisions" << collisions.size() << std::endl;

		// Clear the equivalence vectors for reuse purpose
		equivalences.clear();

		IShape2D *pt1, *pt2;
		for (auto it=collisions.begin();it!=collisions.end();++it)
		{
			pt1=dynamic_cast<IShape2D*>(it->first);
			pt2=dynamic_cast<IShape2D*>(it->second);
			if (pt1->collide(*(pt2)))
			{
				auto bit1=boxes.find(pt1);
				auto bit2=boxes.find(pt2);
				std::cout<<bit1->second<<" "<<bit2->second<<std::endl;
				registerEquivalence(bit1->second,bit2->second,equivalences);
			}
		}

		mergeBlobs(blobs,equivalences);

		return blobs;
	}

	template <typename _datatype>
	blob3DCollection findBlobs3D(const std::vector<_datatype*>& ptrs,unsigned int nrows,unsigned int ncols, _datatype threshold, int minComp, int maxComp, double confidence, bool rowMajor=1)
	{

		// Number of frames
		int nframes=ptrs.size();
		if (nframes<=1)
			throw std::runtime_error("Third dimension should be at least 2 to run this algorithm. if 1, use 2D version");

		// Map of Blobs (key : label, value : blob)
		std::unordered_map<int,Blob3D> blobs;
		blobs.reserve(1000000);

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

		}

		mergeBlobs(blobs,equivalences);

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

		std::cout << "Reduced initial peaks to:" << boxes.size() << "\n";

		Octree oct({0.0,0.0,0.0},{double(ncols),double(nrows),double(nframes)});
		oct.setMaxDepth(6);
		oct.setMaxStorage(6);

		for (auto it=boxes.begin();it!=boxes.end();++it)
			oct.addData(it->first);

		std::set<Octree::collision_pair> collisions;
		oct.getPossibleCollisions(collisions);

		std::cout << "Number of possible collisions" << collisions.size() << std::endl;

		// Clear the equivalence vectors for reuse purpose
		equivalences.clear();

		IShape3D *pt1, *pt2;
		for (auto it=collisions.begin();it!=collisions.end();++it)
		{
			pt1=dynamic_cast<IShape3D*>(it->first);
			pt2=dynamic_cast<IShape3D*>(it->second);
			if (pt1->collide(*(pt2)))
			{
				auto bit1=boxes.find(pt1);
				auto bit2=boxes.find(pt2);
				std::cout<<bit1->second<<" "<<bit2->second<<std::endl;
				registerEquivalence(bit1->second,bit2->second,equivalences);
			}
		}

		mergeBlobs(blobs,equivalences);

		return blobs;
	}

} // namespace Geometry

} // namespace SX
