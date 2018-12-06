#include <utility>
#include <vector>

#include "AABB.h"
#include "ConvolverFactory.h"
#include "DataSet.h"
#include "Diffractometer.h"
#include "IDataReader.h"
#include "Experiment.h"
#include "MathematicsTypes.h"
#include "Octree.h"
#include "Peak3D.h"
#include "PeakFinder.h"
#include "Sample.h"

using EquivalencePair = std::pair<int,int>;
using EquivalenceList = std::vector<EquivalencePair>;

namespace {

    void registerEquivalence(int a, int b, EquivalenceList& equivalences)
    {
        if (a < b) {
            equivalences.emplace_back(b,a);
        } else {
            equivalences.emplace_back(a,b);
        }
    }

    bool sortEquivalences(const EquivalencePair& pa, const EquivalencePair& pb)
    {
        if (pa.first<pb.first) {
            return true;
        }
        if (pa.first>pb.first) {
            return false;
        }
        return (pa.second<pb.second);
    }

    std::map<int,int> removeDuplicates(EquivalenceList& equivalences)
    {
        auto beg = equivalences.begin();
        auto last = std::unique(equivalences.begin(),equivalences.end());

        std::map<int,int> mequiv;

        for (auto it = beg; it != last; ++it) {
            mequiv.insert(*it);
        }
        return mequiv;
    }

    void reassignEquivalences(std::map<int,int>& equivalences)
    {
        for (auto it = equivalences.begin(); it != equivalences.end(); ++it) {
            auto found = equivalences.find(it->second);
            if (found != equivalences.end()) {
                it->second = found->second;
            }
        }
    }

}

namespace nsx {

PeakFinder::PeakFinder(const DataList &datasets)
: ITask("peak finder"),
  _datasets(datasets),
  _peak_merging_scale(1.0),
  _current_label(0),
  _minSize(30),
  _maxSize(10000),
  _maxFrames(10),
  _threshold(3.0),
  _peaks()
{
    ConvolverFactory convolver_factory;
    _convolver.reset(convolver_factory.create("annular",{}));
}

/*
 * blob finding stages:
 *
 * initialize
 * iterate through frames
 *    add to collection if new label
 *    register equivalences
 * merge equivalent blobs
 * register collisions
 * merge colliding blobs
 *
 */
bool PeakFinder::doTask()
{
    _current_step = 0;
    _n_steps = _datasets.size();
    _status = Status::STARTED;
    _info = "Start peak search";

    sendState();

    _peaks.clear();

    for (auto&& dataset : _datasets) {

        auto dataset_basename = dataset->reader()->basename();

        size_t peaks_per_dataset(0);

        int nrows = dataset->nRows();
        int ncols = dataset->nCols();
        int nframes = dataset->nFrames();

        // The blobs found for this numor
        std::map<int,Blob3D> blobs;

        try {

            _current_label = 0;

            #pragma omp parallel
            {
                int loop_begin = -1;
                int loop_end = -1;

                std::map<int,Blob3D> local_blobs = {{}};
                EquivalenceList local_equivalences;

                // determine begining and ending index of current thread
                #pragma omp for
                for (size_t i = 0; i < dataset->nFrames(); ++i) {
                    if ( loop_begin == -1) {
                        loop_begin = i;
                    }
                    loop_end = i+1;
                }

                _info = dataset_basename + ": find primary blobs";
                sendState();

                // find blobs within the current frame range
                findPrimaryBlobs(*dataset, local_blobs, local_equivalences, loop_begin, loop_end);

                _info = dataset_basename + ": merge primary blobs";
                sendState();

                // merge adjacent blobs
                mergeEquivalentBlobs(local_blobs, local_equivalences);

                #pragma omp critical
                {
                    // merge the blobs into the global set
                    for (auto&& blob: local_blobs) {
                        blobs.insert(blob);
                    }
                }
            }

            _info = dataset_basename + ": merge colliding blobs";
            sendState();

            mergeCollidingBlobs(*dataset, blobs);

        }

        // Warning if error
        catch(std::exception& e) {
            _status = Status::ABORTED;
            _info = e.what();
            sendState();
            return false;
        }

        // AABB used for rejecting peaks which overlaps with detector boundaries
        AABB dAABB(Eigen::Vector3d(0,0,0),Eigen::Vector3d(ncols-1, nrows-1, nframes-1));

        for (auto& blob : blobs) {

            Eigen::Vector3d center, eigenvalues;
            Eigen::Matrix3d eigenvectors;

            blob.second.toEllipsoid(1.0, center, eigenvalues, eigenvectors);
            auto shape = Ellipsoid(center, eigenvalues, eigenvectors);

            auto p = sptrPeak3D(new Peak3D(dataset, shape));
            const auto extents = p->shape().aabb().extents();

            // peak too small or too large
            if (extents.maxCoeff() > 1e5 || extents.minCoeff() < 1e-5) {
                continue;
            }

            if (extents(2) > _maxFrames) {
                continue;
            }

            // peak's bounding box not completely contained in detector image
            if (!dAABB.contains(p->shape().aabb())) {
                p->setSelected(false);
            }

            p->setPredicted(false);
            peaks_per_dataset++;
            _peaks.push_back(p);

        }

        dataset->close();

        _current_step += 1;
        sendState();
    }

    _status = Status::COMPLETED;
    _info = "Completed";

    sendState();

    return true;
}

void PeakFinder::setPeakMergingScale(double peak_merging_scale)
{
    _peak_merging_scale = peak_merging_scale;
}

void PeakFinder::setMaxFrames(int maxFrames)
{
    _maxFrames = maxFrames;
}

int PeakFinder::maxFrames() const
{
    return _maxFrames;
}

void PeakFinder::setMinSize(int size)
{
    _minSize = size;
}

int PeakFinder::minSize() const
{
    return _minSize;
}

void PeakFinder::setMaxSize(int size)
{
    _maxSize = size;
}

int PeakFinder::maxSize() const
{
    return _maxSize;
}

const PeakList& PeakFinder::peaks() const
{
    return _peaks;
}

void PeakFinder::setConvolver(std::unique_ptr<Convolver> convolver)
{
    _convolver = std::move(convolver);
}

void PeakFinder::setConvolver(const Convolver& convolver) {
    _convolver.reset(convolver.clone());
}

void PeakFinder::setThreshold(double value)
{
    _threshold = value;
}

void PeakFinder::eliminateBlobs(std::map<int, Blob3D>& blobs) const
{
    for (auto it = blobs.begin(); it != blobs.end();) {
        Blob3D& p=it->second;
        if (p.nPixels() < _minSize || p.nPixels() > _maxSize) {
            it = blobs.erase(it);
        } else {
            it++;
        }
    }
}

void PeakFinder::mergeCollidingBlobs(const DataSet &dataset, std::map<int,Blob3D>& blobs) const
{
    // serial section below
    size_t num_blobs;

    do {
        EquivalenceList equivalences;
        num_blobs = blobs.size();

        // determine which additional blobs should be merged due to collisions / intersection
        findCollisions(dataset, blobs, equivalences);
        // merge the remaining blobs
        mergeEquivalentBlobs(blobs, equivalences);
    } while (num_blobs != blobs.size());

    // remove blobs which are too small or too large
    eliminateBlobs(blobs);
}

void PeakFinder::findPrimaryBlobs(const DataSet &dataset, std::map<int,Blob3D>& blobs,EquivalenceList& equivalences, size_t begin, size_t end)
{
    size_t nrows = dataset.nRows();
    size_t ncols = dataset.nCols();

    // this is the filter function to be applied to each frame
    auto convolve_frame = [&] (const RealMatrix& input) -> RealMatrix {
        RealMatrix output;
        #pragma omp critical
        output = _convolver->convolve(input);
        return output;
    };

    // Map of Blobs (key : label, value : blob)
    blobs.clear();

    // Store labels of current and previous frames.
    std::vector<int> labels(nrows*ncols,0);
    std::vector<int> labels2(nrows*ncols,0);

    // Create empty equivalence table
    equivalences.clear();
    equivalences.reserve(100000);

    // Labels of the left and top pixels with respect to current one and the one above in previous frame
    int left, top, previous;

    int label;
    bool newlabel;
    int index2D = 0;

    // int representing the 8 possible nearest neighbor operations.
    int code;

    int nframes(0);

    // Iterate on all pixels in the image
    // #pragma omp for schedule(dynamic, DYNAMIC_CHUNK)
    for (size_t idx = begin; idx < end; ++idx) {

        #pragma omp atomic
        ++nframes;

        RealMatrix frame_data;

        #pragma omp critical
        frame_data = dataset.frame(idx).cast<double>();

        auto filtered_frame = convolve_frame(frame_data);

        // Go the the beginning of data
        index2D=0;
        for (unsigned int row = 0; row < nrows; ++row) {
            for (unsigned int col = 0; col < ncols; ++col) {
                // Discard pixel if value < threshold
                if (filtered_frame(row, col) < _threshold) {
                    labels[index2D]=labels2[index2D]=0;
                    index2D++;
                    continue;
                }

                newlabel = false;

                // Get labels of adjacent pixels
                left= (col == 0 ? 0 : labels[index2D-1]);
                top=  (row == 0 ? 0 : labels[index2D-ncols]) ;
                previous= (idx == begin ? 0 : labels2[index2D]);
                // Encode type of config.
                code=0;
                code |= ( (left!=0) << 0);
                code |= ( (top!=0) << 1);
                code |= ( (previous!=0)  << 2);

                switch (code) {
                case 0:
                    #pragma omp critical
                    label = ++_current_label;
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
                        registerEquivalence(top, left, equivalences);
                    break;
                case 4: // Only previous
                    label=previous;
                    break;
                case 5: // Left and previous
                    label=left;
                    if (left!=previous)
                        registerEquivalence(left, previous, equivalences);
                    break;
                case 6: // Top and previous
                    label=top;
                    if (top!=previous)
                        registerEquivalence(top, previous, equivalences);
                    break;
                case 7: // All three
                    label=left;
                    if ((top==left) && (top!=previous)) {
                        registerEquivalence(top, previous, equivalences);
                    }
                    else if ((top==previous) && (top!=left)) {
                        registerEquivalence(top, left, equivalences);
                    }
                    else if ((left==previous) && (left!=top)) {
                        registerEquivalence(left, top, equivalences);
                    }
                    else if ((left!=previous) && (left!=top) && (top!=previous)) {
                        registerEquivalence(top, previous, equivalences);
                        registerEquivalence(top, left, equivalences);
                        registerEquivalence(left, previous, equivalences);
                    }
                    break;
                default:
                    break;
                }
                // If none of the neighbors have labels, create new one

                labels[index2D] = labels2[index2D] = label;
                index2D++;
                auto value = frame_data(row, col);
                // Create a new blob if necessary
                if (newlabel) {
                    blobs.insert(std::make_pair(label,Blob3D(col,row,idx,value)));
                } else {
                    blobs[label].addPoint(col,row,idx,value);
                }
            }
        }
    }
}

void PeakFinder::findCollisions(const DataSet &dataset, std::map<int,Blob3D>& blobs, EquivalenceList& equivalences) const
{
    // Clear the equivalence vectors for reuse purpose
    equivalences.clear();

    // Determine the AABB of the blobs
    std::map<const Ellipsoid*,int> boxes;

    Eigen::Vector3d center,extents;
    Eigen::Matrix3d axis;

    for (auto it = blobs.begin(); it != blobs.end();) {
        try {
            // toEllipsoid throws exception if mass is too small
            it->second.toEllipsoid(_peak_merging_scale,center,extents,axis);
        } catch(...) {
            it = blobs.erase(it);
            continue;
        }

        if (extents.minCoeff()<1.0e-13) {
            it = blobs.erase(it);
            continue;
        }

        auto ellipse = new Ellipsoid(center,extents,axis);
        boxes.insert(std::make_pair(ellipse, it->first));
        it++;
    }

    int nrows = dataset.nRows();
    int ncols = dataset.nCols();
    int nframes = dataset.nFrames();

    Octree oct(Eigen::Vector3d(0.0,0.0,0.0),Eigen::Vector3d(double(ncols),double(nrows),double(nframes)));
    oct.setMaxDepth(6);
    oct.setMaxStorage(6);

    std::vector<const Ellipsoid*> xyz_sorted_ellipsoids;
    xyz_sorted_ellipsoids.reserve(boxes.size());
    for (auto&& it : boxes) {
        xyz_sorted_ellipsoids.push_back(it.first);
    }

    // Sort the ellipsoid by increasing x, y and z
    auto cmp = [](const Ellipsoid* ell1, const Ellipsoid* ell2) -> bool {
        auto&& c1 = ell1->center();
        auto&& c2 = ell2->center();

        if (c1[0] < c2[0]) return true;
        if (c1[0] > c2[0]) return false;

        if (c1[0] < c2[0]) return true;
        if (c1[0] > c2[0]) return false;

        if (c1[0] < c2[0]) return true;
        if (c1[0] > c2[0]) return false;

        return false;
    };
    std::sort(xyz_sorted_ellipsoids.begin(),xyz_sorted_ellipsoids.end(),cmp);

    for (auto it: xyz_sorted_ellipsoids) {
        oct.addData(it);
    }

    auto collisions = oct.getCollisions();

    for (auto&& it = collisions.begin(); it != collisions.end(); ++it) {

        auto&& bit1 = boxes.find(it->first);
        auto&& bit2 = boxes.find(it->second);
        registerEquivalence(bit1->second, bit2->second, equivalences);
    }

    // free memory stored in unordered map
    for (auto&& it: boxes) {
        delete it.first;
    }
}

void PeakFinder::mergeEquivalentBlobs(std::map<int,Blob3D>& blobs, EquivalenceList& equivalences) const
{
    // Sort the equivalences pair by ascending order of the first element
    // and if equal by ascending order of their second element.
    std::sort(equivalences.begin(), equivalences.end(), sortEquivalences);

    // Remove the duplicate pairs
    auto mequiv = removeDuplicates(equivalences);

    reassignEquivalences(mequiv);

    // Iterate on blobs and merge equivalences
    for (auto it = blobs.begin(); it != blobs.end();) {

        auto match = mequiv.find(it->first);
        if (match == mequiv.end()) {
            // Nothing is found get to the next blob
            it++;
        } else {
            auto tomerge = blobs.find(match->second);
            // Should never be the case
            if (tomerge != blobs.end()) {
                tomerge->second.merge(it->second);
                it = blobs.erase(it);
            }
        }

    }
}

} // end namespace nsx
