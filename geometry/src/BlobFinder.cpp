#include "BlobFinder.h"
#include "IFrameIterator.h"

#include <iostream>

using std::cout;
using std::endl;

namespace SX
{

namespace Geometry
{

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

    /*
     * blob finding stages:
     *
     * initialize
     * iterate through frames
     *  add to collection if new label
     *  register equivalences
     * merge equivalences
     * register collisions
     * merge collisions
     *
     */
    // the typename matrix_iterator_t should be a forward iterator of type Eigen::Matrix

    blob3DCollection BlobFinder::find(int begin, int end, double background, double threshold, int minComp, int maxComp, double confidence)
    { 
        // Number of frames
        int nframes = 0;

        // Map of Blobs (key : label, value : blob)
        std::unordered_map<int,Blob3D> blobs;
        blobs.reserve(1000000);

        // determine the number of rows and columns
        auto frame_it = _data->getIterator(begin);
        auto nrows = frame_it->getFrame().rows();
        auto ncols = frame_it->getFrame().cols();

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

        if (_filterCallback) {
            cout << "blob finder using a filter" << endl;
        }
        else {
            cout << "blob finder is not using a filter" << endl;
        }

        // todo...
        // threshold *= background

        // Iterate on all pixels in the image
        for (; frame_it->index() != end; frame_it->advance())
        {
            ++nframes;
            auto frame_data = frame_it->getFrame();
            auto filtered_frame = _filterCallback ? _filterCallback(frame_data) : frame_data;
            // testing ONLY
            //frame_data = filtered_frame;

            // Go the the beginning of data
            index2D=0;
            for (unsigned int row=0;row<nrows;++row)
            {
                for (unsigned int col=0;col<ncols;++col)
                {
                    auto value = frame_data(row, col);

                    // Discard pixel if value < threshold
                    if (filtered_frame(row, col)<threshold)
                    {
                        labels[index2D]=labels2[index2D]=0;
                        index2D++;
                        continue;
                    }

                    newlabel=false;
                    // Get labels of adjacent pixels
                    left= (col == 0 ? 0 : labels[index2D-1]);
                    top=  (row == 0 ? 0 : labels[index2D-ncols]) ;
                    previous= (frame_it->index() == begin ? 0 : labels2[index2D]);
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
    }

    void BlobFinder::setFilter(BlobFinder::FilterCallback callback)
    {
        _filterCallback = callback;
    }

/**/

BlobFinder::BlobFinder(SX::Data::IData* data)
{
    _data = data;
}

} // namespace Geometry

} // namespace SX
