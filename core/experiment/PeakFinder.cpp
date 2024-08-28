//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/PeakFinder.cpp
//! @brief     Implements class PeakFinder
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/experiment/PeakFinder.h"

#include "base/geometry/AABB.h"
#include "base/mask/IMask.h"
#include "base/utils/Logger.h"
#include "base/utils/ProgressHandler.h"
#include "core/convolve/ConvolverFactory.h"
#include "core/data/DataSet.h"
#include "core/detector/Detector.h"
#include "core/experiment/Experiment.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"
#include "core/integration/Blob3D.h"
#include "core/loader/IDataReader.h"
#include "core/peak/Peak3D.h"
#include "core/shape/Octree.h"

#include <Eigen/Dense>
#include <cstdio>
#include <utility>
#include <vector>

#define DYNAMIC_CHUNK 10

using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

//  ***********************************************************************************************
//  local functions
//  ***********************************************************************************************

namespace {

void registerEquivalence(int a, int b, ohkl::EquivalenceList& equivalences)
{
    if (a < b)
        equivalences.emplace_back(ohkl::EquivalenceList::value_type(b, a));
    else
        equivalences.emplace_back(ohkl::EquivalenceList::value_type(a, b));
}

bool sortEquivalences(const ohkl::EquivalencePair& pa, const ohkl::EquivalencePair& pb)
{
    if (pa.first < pb.first)
        return true;
    if (pa.first > pb.first)
        return false;
    return (pa.second < pb.second);
}

std::map<int, int> removeDuplicates(ohkl::EquivalenceList& equivalences)
{
    auto beg = equivalences.begin();
    auto last = std::unique(equivalences.begin(), equivalences.end());

    std::map<int, int> mequiv;

    for (auto it = beg; it != last; ++it)
        mequiv.insert(*it);
    return mequiv;
}

void reassignEquivalences(std::map<int, int>& equivalences)
{
    for (auto it = equivalences.begin(); it != equivalences.end(); ++it) {
        auto found = equivalences.find(it->second);
        if (found != equivalences.end())
            it->second = found->second;
    }
}

} // namespace

namespace ohkl {

void PeakFinderParameters::log(const Level& level) const
{
    ohklLog(level, "Peak finder parameters:");
    ohklLog(level, "minimum_size           = ", minimum_size);
    ohklLog(level, "maximum_size           = ", maximum_size);
    ohklLog(level, "peak_end               = ", peak_end);
    ohklLog(level, "maximum_frames         = ", maximum_frames);
    ohklLog(level, "first_frame           = ", first_frame);
    ohklLog(level, "last_frame             = ", last_frame);
    ohklLog(level, "threshold              = ", threshold);
    ohklLog(level, "convolver              = ", convolver);
}

//  ***********************************************************************************************
//  PeakFinder trivia
//  ***********************************************************************************************

PeakFinder::PeakFinder() : _handler(nullptr), _current_label(0), _integrated(false)
{
    _params = std::make_unique<PeakFinderParameters>();
    _convolver.reset(ConvolverFactory{}.create("annular", {{"r1", 5.}, {"r2", 10.}, {"r3", 15.}}));
}

std::vector<Peak3D*> PeakFinder::currentPeaks()
{
    std::vector<Peak3D*> output;
    for (const sptrPeak3D& peak : _current_peaks)
        output.push_back(peak.get());
    return output;
}

PeakCollection* PeakFinder::getPeakCollection()
{
    PeakCollection* ptr = &_peak_collection;
    return ptr;
}

void PeakFinder::setPeakCollection(
    const std::string name, const ohkl::PeakCollectionType type,
    std::vector<std::shared_ptr<ohkl::Peak3D>> peak_list, sptrDataSet data)
{
    _peak_collection = PeakCollection(name, type, data);
    _peak_collection.populate(peak_list);
    _peak_collection.setIntegrated(_integrated);
}

void PeakFinder::setHandler(const sptrProgressHandler& handler)
{
    _handler = handler;
}

PeakFinderParameters* PeakFinder::parameters()
{
    return _params.get();
}

void PeakFinder::setConvolver(std::unique_ptr<Convolver> convolver)
{
    _convolver = std::move(convolver);
}

void PeakFinder::setConvolver(
        const std::string& convolver, const std::map<std::string, double>& parameters)
{
    _convolver.reset(ConvolverFactory{}.create(convolver, parameters));
}

void PeakFinder::setConvolver(const Convolver& convolver)
{
    _convolver.reset(convolver.clone());
}

//  ***********************************************************************************************
//  PeakFinder algorithm
//  ***********************************************************************************************

void PeakFinder::eliminateBlobs(std::map<int, Blob3D>& blobs) const
{
    // update progress handler
    if (_handler) {
        _handler->setStatus("Eliminating blobs which are too small or too large...");
        _handler->setProgress(0);
    }

    // dummies use to help progress handler
    int dummy = 0;
    int magic = 0.2 * std::distance(blobs.begin(), blobs.end());

    for (auto it = blobs.begin(); it != blobs.end();) {
        ++dummy;

        Blob3D& p = it->second;
        if (p.getComponents() < _params->minimum_size || p.getComponents() > _params->maximum_size)
            it = blobs.erase(it);
        else
            it++;
        // update progress handler
        if ((dummy & magic) == 0 && _handler) {
            double total_dist = std::distance(blobs.begin(), blobs.end());
            double current_dist = std::distance(blobs.begin(), it);
            double progress = 100.0 * current_dist / total_dist;
            _handler->setProgress(50 + 0.5 * progress);
        }
    }

    if (_handler)
        _handler->setProgress(100);
}

void PeakFinder::mergeCollidingBlobs(const DataSet& data, std::map<int, Blob3D>& blobs) const
{
    // serial section below
    size_t num_blobs;

    do {
        ohkl::EquivalenceList equivalences;
        num_blobs = blobs.size();

        // determine which additional blobs should be merged due to collisions /
        // intersection
        findCollisions(data, blobs, equivalences);
        // merge the remaining blobs
        mergeEquivalentBlobs(blobs, equivalences);
    } while (num_blobs != blobs.size());

    // remove blobs which are too small or too large
    eliminateBlobs(blobs);
}

void PeakFinder::findPrimaryBlobs(
    const DataSet& data, std::map<int, Blob3D>& blobs, ohkl::EquivalenceList& equivalences,
    size_t begin, size_t end)
{
    std::ostringstream oss;
    oss << "Finding blobs for data set " << data.name();

    // update via handler if necessary
    if (_handler) {
        _handler->setStatus(oss.str().c_str());
        _handler->setProgress(0);
    }

    // Map of Blobs (key : label, value : blob)
    blobs.clear();

    int nrows = data.nRows();
    int ncols = data.nCols();

    // Store labels of current and previous frames.
    std::vector<int> labels(nrows * ncols, 0);
    std::vector<int> labels2(nrows * ncols, 0);

    // Create empty equivalence table
    equivalences.clear();
    equivalences.reserve(100000);

    // Iterate on all pixels in the image
    int nframes = 0;
    for (size_t idx = begin; idx < end; ++idx) {
        ++nframes;

        RealMatrix frame_data = data.frame(idx).cast<double>();
        RealMatrix filtered_frame = _convolver->convolve(frame_data);

        // Go the the beginning of data
        int index2D = 0;
        for (unsigned int row = 0; row < nrows; ++row) {
            for (unsigned int col = 0; col < ncols; ++col) {
                // Discard pixel if value < threshold
                if (filtered_frame(row, col) < _params->threshold) {
                    labels[index2D] = labels2[index2D] = 0;
                    index2D++;
                    continue;
                }

                // Gets labels of adjacent pixels
                int left = (col == 0 ? 0 : labels[index2D - 1]);
                int top = (row == 0 ? 0 : labels[index2D - ncols]);
                int previous = (idx == begin ? 0 : labels2[index2D]);
                // Encode type of config.
                int code = 0;
                code |= ((left != 0) << 0);
                code |= ((top != 0) << 1);
                code |= ((previous != 0) << 2);

                int label = 0;
                bool newlabel = false;
                switch (code) {
                    case 0:
                        label = ++_current_label;
                        newlabel = true;
                        break;
                    case 1: // Only left pixel
                        label = left;
                        break;
                    case 2: // Only top pixel
                        label = top;
                        break;
                    case 3: // Top and left
                        label = top;
                        if (top != left)
                            registerEquivalence(top, left, equivalences);
                        break;
                    case 4: // Only previous
                        label = previous;
                        break;
                    case 5: // Left and previous
                        label = left;
                        if (left != previous)
                            registerEquivalence(left, previous, equivalences);
                        break;
                    case 6: // Top and previous
                        label = top;
                        if (top != previous)
                            registerEquivalence(top, previous, equivalences);
                        break;
                    case 7: // All three
                        label = left;
                        if ((top == left) && (top != previous))
                            registerEquivalence(top, previous, equivalences);
                        else if ((top == previous) && (top != left))
                            registerEquivalence(top, left, equivalences);
                        else if ((left == previous) && (left != top))
                            registerEquivalence(left, top, equivalences);
                        else if ((left != previous) && (left != top) && (top != previous)) {
                            registerEquivalence(top, previous, equivalences);
                            registerEquivalence(top, left, equivalences);
                            registerEquivalence(left, previous, equivalences);
                        }
                        break;
                    default: break;
                }
                // If none of the neighbors have labels, create new one

                labels[index2D] = labels2[index2D] = label;
                index2D++;
                auto value = frame_data(row, col);
                // Create a new blob if necessary
                if (newlabel)
                    blobs.insert(std::make_pair(label, Blob3D(col, row, idx, value)));
                else {
                    auto it = blobs.find(label);
                    it->second.addPoint(col, row, idx, value);
                }
            }
        }

        if (_handler)
            _handler->setProgress(100.0 * nframes / (end - begin + 1));
    }

    if (_handler)
        _handler->setProgress(100);
}

void PeakFinder::findCollisions(
    const DataSet& data, std::map<int, Blob3D>& blobs, ohkl::EquivalenceList& equivalences) const
{
    // Clear the equivalence vectors for reuse purpose
    equivalences.clear();

    // update progress handler
    if (_handler) {
        _handler->setStatus("Finding blob collisions...");
        _handler->setProgress(0);
    }

    // Determine the AABB of the blobs
    std::map<const Ellipsoid*, int> boxes;

    Eigen::Vector3d center, extents;
    Eigen::Matrix3d axis;

    // dummies used to help progress handler
    int dummy = 0;
    int magic = 0.2 * std::distance(blobs.begin(), blobs.end());

    if (magic < 1)
        magic = 1;

    for (auto it = blobs.begin(); it != blobs.end();) {
        ++dummy;

        if (!it->second.toEllipsoid(_params->peak_end, center, extents, axis)) {
            it = blobs.erase(it);
            continue;
        }

        // if the threshold is too small it will break the OpenMP peak search
        // when the number of threads is very large
        if (extents.minCoeff() < 1.0e-13) {
            it = blobs.erase(it);
            continue;
        }

        auto ellipse = new Ellipsoid(center, extents, axis);
        boxes.insert(std::make_pair(ellipse, it->first));
        it++;

        // update progress handler
        if ((dummy % magic) == 0 && _handler) {
            double total_dist = std::distance(blobs.begin(), blobs.end());
            double current_dist = std::distance(blobs.begin(), it);
            double progress = 100.0 * current_dist / total_dist;
            _handler->setProgress(0.5 * progress);
        }
    }

    const auto& dectector = data.diffractometer()->detector();
    const int nrows = dectector->nRows();
    const int ncols = dectector->nCols();
    const int nframes = data.nFrames();

    Octree oct(
        Eigen::Vector3d(0.0, 0.0, 0.0),
        Eigen::Vector3d(double(ncols), double(nrows), double(nframes)));
    oct.setMaxDepth(6);
    oct.setMaxStorage(6);

    std::vector<const Ellipsoid*> xyz_sorted_ellipsoids;
    xyz_sorted_ellipsoids.reserve(boxes.size());
    for (const auto& it : boxes)
        xyz_sorted_ellipsoids.push_back(it.first);

    // Sort the ellipsoid by increasing x, y and z
    auto cmp = [](const Ellipsoid* ell1, const Ellipsoid* ell2) -> bool {
        auto&& c1 = ell1->center();
        auto&& c2 = ell2->center();

        if (c1[0] < c2[0])
            return true;
        if (c1[0] > c2[0])
            return false;

        if (c1[0] < c2[0])
            return true;
        if (c1[0] > c2[0])
            return false;

        if (c1[0] < c2[0])
            return true;
        if (c1[0] > c2[0])
            return false;

        return false;
    };
    std::sort(xyz_sorted_ellipsoids.begin(), xyz_sorted_ellipsoids.end(), cmp);

    for (const auto& it : xyz_sorted_ellipsoids)
        oct.addData(it);

    const std::set<std::pair<const Ellipsoid*, const Ellipsoid*>>& collisions = oct.getCollisions();

    // dummies used to help progress handler
    dummy = 0;
    magic = 0.02 * std::distance(collisions.begin(), collisions.end());

    if (magic < 1)
        magic = 1;

    for (auto&& it = collisions.begin(); it != collisions.end(); ++it) {
        const auto& bit1 = boxes.find(it->first);
        const auto& bit2 = boxes.find(it->second);
        registerEquivalence(bit1->second, bit2->second, equivalences);

        // update progress handler
        if ((dummy % magic) == 0 && _handler) {
            const double total_dist = std::distance(collisions.begin(), collisions.end());
            const double current_dist = std::distance(collisions.begin(), it);
            const double progress = 100.0 * current_dist / total_dist;
            _handler->setProgress(50 + 0.5 * progress);
        }
        ++dummy;
    }

    // calculation complete
    if (_handler)
        _handler->setProgress(100);

    // free memory stored in unordered map
    for (const auto& it : boxes)
        delete it.first;
}

void PeakFinder::mergeEquivalentBlobs(
    std::map<int, Blob3D>& blobs, ohkl::EquivalenceList& equivalences) const
{
    // initialize progress handler if necessary
    if (_handler)
        _handler->setProgress(0);

    // Sort the equivalences pair by ascending order of the first element
    // and if equal by ascending order of their second element.
    std::sort(equivalences.begin(), equivalences.end(), sortEquivalences);

    // Remove the duplicate pairs
    auto mequiv = removeDuplicates(equivalences);

    reassignEquivalences(mequiv);

    // dummy for calling progress updater
    int dummy = 0;
    int magic = 0.02 * std::distance(blobs.begin(), blobs.end());
    if (magic == 0)
        magic = 1;

    // Iterate on blobs and merge equivalences
    for (auto&& it = blobs.begin(); it != blobs.end();) {
        ++dummy;

        const auto match = mequiv.find(it->first);
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

        // update progress handler
        if ((dummy % magic) == 0 && _handler) {
            double total_dist = std::distance(blobs.begin(), blobs.end());
            double current_dist = std::distance(blobs.begin(), it);
            int progress = static_cast<int>(100.0 * current_dist / total_dist);
            _handler->setProgress(progress);
        }
    }
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
void PeakFinder::find(const sptrDataSet data)
{
    ohklLog(Level::Debug, "PeakFinder::find: begin");
    _params->log(Level::Info);
    _current_data = data;
    _current_peaks.clear();

    PeakList peaks;

    const auto& dectector = data->diffractometer()->detector();
    const int nrows = dectector->nRows();
    const int ncols = dectector->nCols();
    const int nframes = data->nFrames();

    std::map<int, Blob3D> blobs;

    _current_label = 0;

    int loop_begin = _params->first_frame;
    int loop_end = _params->last_frame;
    if (loop_begin == -1)
        loop_begin = 0;
    if (loop_end == -1)
        loop_end = nframes;

    // keep frame indices in bounds
    if (loop_begin < 0)
        loop_begin = 0;
    if (loop_begin > nframes)
        loop_begin = nframes;
    if (loop_end < 0)
        loop_end = 0;
    if (loop_end > nframes)
        loop_end = nframes;

    std::map<int, Blob3D> local_blobs = {{}};
    ohkl::EquivalenceList local_equivalences;

    // find blobs within the current frame range
    ohklLog(Level::Debug, "PeakFinder::find: findPrimary from ", loop_begin, " to ", loop_end);
    findPrimaryBlobs(*data, local_blobs, local_equivalences, loop_begin, loop_end);

    // merge adjacent blobs
    ohklLog(Level::Debug, "PeakFinder::find: mergeBlobs");
    mergeEquivalentBlobs(local_blobs, local_equivalences);

    ohklLog(Level::Debug, "PeakFinder::find: blob loop");
    // merge the blobs into the global set
    for (const auto& blob : local_blobs)
        blobs.insert(blob);

    mergeCollidingBlobs(*data, blobs);
    ohklLog(Level::Debug, "PeakFinder::find: found blob collisions");

    if (_handler) {
        _handler->setProgress(100);
    }

    if (_handler) {
        _handler->setStatus("Computing bounding boxes...");
        _handler->setProgress(0);
    }

    int count = 0;

    const auto& kernel_size = _convolver->kernelSize();
    const auto& x_offset = kernel_size.first;
    const auto& y_offset = kernel_size.second;

    // AABB used for rejecting peaks which overlaps with detector boundaries
    AABB dAABB(
        Eigen::Vector3d(x_offset, y_offset, 0),
        Eigen::Vector3d(ncols - x_offset, nrows - y_offset, nframes - 1));

    static const double peaksTooLargeLimit = 1e5;
    static const double peaksTooSmallLimit = 1e-5;

    std::size_t numPeaksTooSmallOrLarge = 0;
    std::size_t numPeaksOutsideFrames = 0;
    std::size_t numPeaksNotInDetArea = 0;
    std::size_t numPeaksMasked = 0;

    for (auto& blob : blobs) {
        Eigen::Vector3d center, eigenvalues;
        Eigen::Matrix3d eigenvectors;

        blob.second.toEllipsoid(1.0, center, eigenvalues, eigenvectors);
        auto shape = Ellipsoid(center, eigenvalues, eigenvectors);

        auto p = sptrPeak3D(new Peak3D(data, shape));
        const auto extents = p->shape().aabb().extents();

        // peak overlaps with mask
        for (IMask* mask : data->masks()) {
            if (mask->collide(p->shape())) {
                p->setRejectionFlag(RejectionFlag::Masked);
                ++numPeaksMasked;
            }
        }

        // peak too small or too large
        if ((extents.maxCoeff() > peaksTooLargeLimit || extents.minCoeff() < peaksTooSmallLimit)
            && !p->enabled()) {
            p->setRejectionFlag(RejectionFlag::OutsideThreshold);
            ++numPeaksTooSmallOrLarge;
        }

        if (extents(2) > _params->maximum_frames && !p->enabled()) {
            p->setRejectionFlag(RejectionFlag::OutsideFrames);
            ++numPeaksOutsideFrames;
        }

        // peak's bounding box not completely contained in detector image
        if (!dAABB.contains(p->shape().aabb()) && !p->enabled()) {
            p->setRejectionFlag(RejectionFlag::OutsideDetector);
            ++numPeaksNotInDetArea;
        }

        peaks.push_back(p);
        _current_peaks.push_back(p);

        ++count;

        if (_handler) {
            double progress = count * 100.0 / blobs.size();
            _handler->setProgress(progress);
        }
    }

    ohklLog(Level::Debug, "PeakFinder::find: blob loop done");

    if (_handler) {
        _handler->setStatus(("Integrating " + std::to_string(peaks.size()) + " peaks...").c_str());
        _handler->setProgress(0);
    }

    ohklLog(
        Level::Info, "PeakFinder::find: ", peaks.size(), " peaks found,", numPeaksTooSmallOrLarge,
        " peaks too small, ", numPeaksOutsideFrames, " peaks outside frame range, ",
        numPeaksNotInDetArea, " peaks not fully on detector.");
    ohklLog(Level::Info, "PeakFinder::find: ", numPeaksMasked, " peaks masked");
    _peaks_found = peaks.size();

    data->close();

    if (_handler) {
        _handler->setStatus("Peak finding completed.");
        _handler->setProgress(100);
    }
    setPeakCollection("Found peaks", ohkl::PeakCollectionType::FOUND, _current_peaks, data);
    ohklLog(Level::Info, "PeakFinder::find: exit");
}

unsigned int PeakFinder::numberFound()
{
    return _peaks_found;
}

} // namespace ohkl
