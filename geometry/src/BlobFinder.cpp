#include "BlobFinder.h"
#include "IFrameIterator.h"

#include <iostream>

using std::cout;
using std::endl;

namespace SX
{

namespace Geometry
{

void BlobFinder::registerEquivalence(int a, int b, vipairs& equivalences)
{
    if (a < b)
        equivalences.push_back(vipairs::value_type(b,a));
    else
        equivalences.push_back(vipairs::value_type(a,b));
}

bool BlobFinder::sortEquivalences(const ipair& pa, const ipair& pb)
{
    if (pa.first<pb.first)
        return true;
    if (pa.first>pb.first)
        return false;
    return (pa.second<pb.second);
}

imap BlobFinder::removeDuplicates(vipairs& equivalences)
{
    auto beg=equivalences.begin();
    auto last=std::unique(equivalences.begin(),equivalences.end());

    imap mequiv;
    for (auto it=beg;it!=last;++it)
        mequiv.insert(*it);

    return mequiv;
}

void BlobFinder::reassignEquivalences(imap& equivalences)
{
    for (auto it=equivalences.begin();it!=equivalences.end();++it)
    {
        auto found=equivalences.find(it->second);
        if (found!=equivalences.end())
            it->second=found->second;
    }
}


void BlobFinder::eliminateBlobs()
{
    // update progress handler
    if ( _progressHandler ) {
        _progressHandler->setStatus("Eliminating blobs which are too small or too large...");
        _progressHandler->setProgress(0);
    }

    // dummies use to help progress handler
    int dummy = 0;
    int magic = 0.2 * std::distance(_blobs.begin(), _blobs.end());

    for (auto it = _blobs.begin(); it != _blobs.end();)
    {
        ++dummy;

        Blob3D& p=it->second;
        if (p.getComponents() < _minComp || p.getComponents() > _maxComp)
            it = _blobs.erase(it);
        else
            it++;

        // update progress handler
        if ( dummy&magic == 0 && _progressHandler) {

            double total_dist = std::distance(_blobs.begin(), _blobs.end());
            double current_dist = std::distance(_blobs.begin(), it);
            double progress = 100.0 * current_dist / total_dist;
            _progressHandler->setProgress(50 + 0.5*progress);
        }
    }

    if ( _progressHandler )
        _progressHandler->setProgress(100);
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
blob3DCollection BlobFinder::find(int begin, int end)
{
    // find all blobs, possibly with multiple labels
    findBlobs(begin, end);

    // merge adjacent blobs
    mergeBlobs();

    // remove blobs which are too small or too large
    eliminateBlobs();

    // determine which additional blobs should be merged due to collisions / intersection
    findCollisions();

    // merge the remaining blobs
    mergeBlobs();

    if (_progressHandler) {
        _progressHandler->setStatus("Blob finding complete.");
        _progressHandler->setProgress(100);
    }

    return _blobs;
}

void BlobFinder::findBlobs(int begin, int end)
{
    // update via handler if necessary
    if ( _progressHandler ) {
        _progressHandler->setStatus("Finding blobs...");
        _progressHandler->setProgress(0);
    }

    // used to pass to progress handler
    double progress = 0.0;

    // threshold for blob addition
    double threshold = _isRelative ? _threshold*_median : _threshold;

    // Number of frames
    _nframes = 0;

    // Map of Blobs (key : label, value : blob)
    _blobs.clear();
    _blobs.reserve(1000000);

    // determine the number of rows and columns
    auto frame_it = _data->getIterator(begin);
    _nrows = frame_it->getFrame().rows();
    _ncols = frame_it->getFrame().cols();

    // Store labels of current and previous frames.
    vints labels(_nrows*_ncols,0);
    vints labels2(_nrows*_ncols,0);

    // Create empty equivalence table
    _equivalences.clear();
    _equivalences.reserve(100000);

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

    // Iterate on all pixels in the image
    for (; frame_it->index() != end; frame_it->advance())
    {
        ++_nframes;

        auto frame_data = frame_it->getFrame();
        auto filtered_frame = _filterCallback ? _filterCallback(frame_data) : frame_data;
        // testing ONLY
        //frame_data = filtered_frame;

        // Go the the beginning of data
        index2D=0;
        for (unsigned int row=0;row<_nrows;++row)
        {
            for (unsigned int col=0;col<_ncols;++col)
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
                top=  (row == 0 ? 0 : labels[index2D-_ncols]) ;
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
                        registerEquivalence(top, left, _equivalences);
                    break;
                case 4: // Only previous
                    label=previous;
                    break;
                case 5: // Left and previous
                    label=left;
                    if (left!=previous)
                        registerEquivalence(left, previous, _equivalences);
                    break;
                case 6: // Top and previous
                    label=top;
                    if (top!=previous)
                        registerEquivalence(top, previous, _equivalences);
                    break;
                case 7: // All three
                    label=left;
                    if ((top==left) && (top!=previous))
                        registerEquivalence(top, previous, _equivalences);
                    else if ((top==previous) && (top!=left))
                        registerEquivalence(top, left, _equivalences);
                    else if ((left==previous) && (left!=top))
                        registerEquivalence(left, top, _equivalences);
                    else if ((left!=previous) && (left!=top) && (top!=previous))
                    {
                        registerEquivalence(top, previous, _equivalences);
                        registerEquivalence(top, left, _equivalences);
                        registerEquivalence(left, previous, _equivalences);
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
                    _blobs.insert(blob3DCollection::value_type(label,Blob3D(col,row,frame,value)));
                }
                else
                {
                    auto it = _blobs.find(label);
                    it->second.addPoint(col,row,frame,value);
                }
            }
        }

        // increase frame count
        // TODO: this could probably be done better!
        ++frame;

        progress = static_cast<double>(frame) / static_cast<double>(end-begin) * 100.0;

        if ( _progressHandler )
            _progressHandler->setProgress(progress);
    }

    if (_progressHandler)
        _progressHandler->setProgress(100);

    // too few frames for algorithm to be reliable
    if (_nframes<=1)
        throw std::runtime_error("Third dimension should be at least 2 to run this algorithm. if 1, use 2D version");

}

void BlobFinder::setProgressHandler(std::shared_ptr<Utils::ProgressHandler> callback)
{
    _progressHandler = callback;
}

void BlobFinder::setThreshold(double threshold)
{
    _threshold = threshold;
}

void BlobFinder::setConfidence(double confidence)
{
    _confidence = confidence;
}

void BlobFinder::setMedian(double median)
{
    _median = median;
}

void BlobFinder::setMinComp(int minComp)
{
    _minComp = minComp;
}

void BlobFinder::setMaxComp(int maxComp)
{
    _maxComp = maxComp;
}

void BlobFinder::setRelative(bool isRelative)
{
    _isRelative = isRelative;
}

void BlobFinder::findCollisions()
{
    // update progress handler
    if (_progressHandler) {
        _progressHandler->setStatus("Finding blob collisions...");
        _progressHandler->setProgress(0);
    }

    // Determine the AABB of the blobs
    shape3Dmap boxes;
    boxes.reserve(_blobs.size());

    Eigen::Vector3d center,extents;
    Eigen::Matrix3d axis;

    // dummies used to help progress handler
    int dummy = 0;
    int magic = 0.2 * std::distance(_blobs.begin(), _blobs.end());

    for (auto it = _blobs.begin(); it != _blobs.end();)
    {
        ++dummy;

        Blob3D& p=it->second;
        p.toEllipsoid(_confidence,center,extents,axis);

        if (extents.minCoeff()<1.0e-9)
            it = _blobs.erase(it);
        else
        {
            boxes.insert(shape3Dmap::value_type(new Ellipsoid3D(center,extents,axis),it->first));
            it++;
        }

        // update progress handler
        if ( dummy&magic == 0 && _progressHandler) {
            double total_dist = std::distance(_blobs.begin(), _blobs.end());
            double current_dist = std::distance(_blobs.begin(), it);
            double progress = 100.0 * current_dist / total_dist;
            _progressHandler->setProgress(0.5*progress);
        }
    }

    Octree oct({0.0,0.0,0.0},{double(_ncols),double(_nrows),double(_nframes)});
    oct.setMaxDepth(6);
    oct.setMaxStorage(6);

    for (auto it=boxes.begin();it!=boxes.end();++it)
        oct.addData(it->first);

    std::set<Octree::collision_pair> collisions;
    oct.getPossibleCollisions(collisions);


    // Clear the equivalence vectors for reuse purpose
    _equivalences.clear();


    // dummies used to help progress handler
    dummy = 0;
    magic = 0.02 * std::distance(collisions.begin(), collisions.end());

    for (auto it = collisions.begin(); it != collisions.end(); ++it)
    {
        if (it->first->collide(*(it->second)))
        {
            auto bit1 = boxes.find(it->first);
            auto bit2 = boxes.find(it->second);
            registerEquivalence(bit1->second, bit2->second, _equivalences);
        }

        // update progress handler
        if ( dummy&magic == 0 && _progressHandler) {

            double total_dist = std::distance(collisions.begin(), collisions.end());
            double current_dist = std::distance(collisions.begin(), it);
            double progress = 100.0 * current_dist / total_dist;
            _progressHandler->setProgress(50 + 0.5*progress);
        }
    }

    // calculation complete
    if ( _progressHandler )
        _progressHandler->setProgress(100);
}

void BlobFinder::setFilter(BlobFinder::FilterCallback callback)
{
    _filterCallback = callback;
}

/**/

BlobFinder::BlobFinder(std::shared_ptr<SX::Data::IData> data)
{
    _data = data;
}


void BlobFinder::mergeBlobs()
{
    // initialize progress handler if necessary
    if (_progressHandler) {
        _progressHandler->setStatus("Merging blobs...");
        _progressHandler->setProgress(0);
    }

    // Sort the equivalences pair by ascending order of the first element
    // and if equal by ascending order of their second element.
    std::sort(_equivalences.begin(), _equivalences.end(), sortEquivalences);

    // Remove the duplicate pairs
    imap mequiv = removeDuplicates(_equivalences);

    reassignEquivalences(mequiv);

    // dummy for calling progress updater
    int dummy = 0;
    int magic = 0.02 * std::distance(_blobs.begin(), _blobs.end());

    // Iterate on blobs and merge equivalences
    for (auto it = _blobs.begin(); it != _blobs.end();)
    {
        ++dummy;

        auto match = mequiv.find(it->first);
        if (match == mequiv.end())
        {
            // Nothing is found get to the next blob
            it++;
        }
        else
        {
            auto tomerge = _blobs.find(match->second);
            // Should never be the case
            if (tomerge != _blobs.end())
            {
                tomerge->second.merge(it->second);
                it = _blobs.erase(it);
            }
        }

        // update progress handler
        if ( dummy%magic == 0 && _progressHandler) {
            double total_dist = std::distance(_blobs.begin(), _blobs.end());
            double current_dist = std::distance(_blobs.begin(), it);
            int progress = static_cast<int>( 100.0 * current_dist / total_dist);
            _progressHandler->setProgress(progress);
        }
    }

    // finalize update handler
    if ( _progressHandler )
        _progressHandler->setProgress(100);
}

} // namespace Geometry

} // namespace SX
