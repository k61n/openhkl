//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/analyse/PeakFinder.cpp
//! @brief     Implements class PeakFinder
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/analyse/PeakFinder.h"

#include "base/geometry/AABB.h"
#include "core/analyse/Octree.h"
#include "core/convolve/ConvolverFactory.h"
#include "core/experiment/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"
#include "core/peak/Peak3D.h"
#include "core/raw/IDataReader.h"
#include <Eigen/Dense>
#include <QDebug>
#include <QtGlobal>
#include <cstdio>
#include <utility>
#include <vector>

using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

//  ***********************************************************************************************
//  local functions
//  ***********************************************************************************************

namespace {
void registerEquivalence(int a, int b, nsx::EquivalenceList& equivalences)
{
    if (a < b)
        equivalences.emplace_back(nsx::EquivalenceList::value_type(b, a));
    else
        equivalences.emplace_back(nsx::EquivalenceList::value_type(a, b));
}

bool sortEquivalences(const nsx::EquivalencePair& pa, const nsx::EquivalencePair& pb)
{
    if (pa.first < pb.first)
        return true;
    if (pa.first > pb.first)
        return false;
    return (pa.second < pb.second);
}

std::map<int, int> removeDuplicates(nsx::EquivalenceList& equivalences)
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

namespace nsx {

//  ***********************************************************************************************
//  PeakFinder trivia
//  ***********************************************************************************************

PeakFinder::PeakFinder()
    : _handler(nullptr)
    , _threshold(80.0)
    , _peakScale(1.0)
    , _current_label(0)
    , _minSize(30)
    , _maxSize(10000)
    , _maxFrames(10)
    , _framesBegin(-1)
    , _framesEnd(-1)
{
    ConvolverFactory convolver_factory;
    _convolver.reset(convolver_factory.create("annular", {{"r1", 5.}, {"r2", 10.}, {"r3", 15.}}));
}

std::vector<Peak3D*> PeakFinder::currentPeaks()
{
    std::vector<Peak3D*> output;
    for (sptrPeak3D peak : _current_peaks) {
        output.push_back(peak.get());
    }
    return output;
}

void PeakFinder::setHandler(const sptrProgressHandler& handler)
{
    _handler = handler;
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

void PeakFinder::setFramesBegin(int framesBegin)
{
    _framesBegin = framesBegin;
}

int PeakFinder::framesBegin()
{
    return _framesBegin;
}

void PeakFinder::setFramesEnd(int framesEnd)
{
    _framesEnd = framesEnd;
}

int PeakFinder::framesEnd()
{
    return _framesEnd;
}

void PeakFinder::setConvolver(std::unique_ptr<Convolver> convolver)
{
    _convolver = std::move(convolver);
}

void PeakFinder::setConvolver(const Convolver& convolver)
{
    _convolver.reset(convolver.clone());
}

void PeakFinder::setThreshold(double value)
{
    _threshold = value;
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
        if (p.getComponents() < _minSize || p.getComponents() > _maxSize)
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

    if (_handler) {
        _handler->log("After elimination, " + std::to_string(blobs.size()) + " blobs remain");
        _handler->setProgress(100);
    }
}

void PeakFinder::mergeCollidingBlobs(sptrDataSet data, std::map<int, Blob3D>& blobs) const
{
    // serial section below
    size_t num_blobs;

    do {
        nsx::EquivalenceList equivalences;
        num_blobs = blobs.size();

        if (_handler)
            _handler->log("number of blobs is " + std::to_string(num_blobs));

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
    sptrDataSet data, std::map<int, Blob3D>& blobs, nsx::EquivalenceList& equivalences,
    size_t begin, size_t end)
{
    // update via handler if necessary
    if (_handler) {
        _handler->setStatus("Finding blobs...");
        _handler->setProgress(0);
    }

    auto dectector = data->reader()->diffractometer()->detector();
    int nrows = dectector->nRows();
    int ncols = dectector->nCols();

    // used to pass to progress handler
    double progress = 0.0;

    // Map of Blobs (key : label, value : blob)
    blobs.clear();

    nrows = data->nRows();
    ncols = data->nCols();

    // Store labels of current and previous frames.
    std::vector<int> labels(nrows * ncols, 0);
    std::vector<int> labels2(nrows * ncols, 0);

    // Create empty equivalence table
    equivalences.clear();
    equivalences.reserve(100000);

    // Labels of the left and top pixels with respect to current one and the one
    // above in previous frame
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
        ++nframes;

        RealMatrix frame_data;

        frame_data = data->frame(idx).cast<double>();

        RealMatrix filtered_frame = _convolver->convolve(frame_data);

        // Go the the beginning of data
        index2D = 0;
        for (unsigned int row = 0; row < nrows; ++row) {
            for (unsigned int col = 0; col < ncols; ++col) {
                // Discard pixel if value < threshold
                if (filtered_frame(row, col) < _threshold) {
                    labels[index2D] = labels2[index2D] = 0;
                    index2D++;
                    continue;
                }

                newlabel = false;

                // Gets labels of adjacent pixels
                left = (col == 0 ? 0 : labels[index2D - 1]);
                top = (row == 0 ? 0 : labels[index2D - ncols]);
                previous = (idx == begin ? 0 : labels2[index2D]);
                // Encode type of config.
                code = 0;
                code |= ((left != 0) << 0);
                code |= ((top != 0) << 1);
                code |= ((previous != 0) << 2);

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

        progress = static_cast<double>(nframes) / static_cast<double>(data->nFrames()) * 100.0;

        if (_handler)
            _handler->setProgress(progress);
    }

    if (_handler) {
        _handler->log("Found " + std::to_string(blobs.size()) + " blobs");
        _handler->setProgress(100);
    }
}

void PeakFinder::findCollisions(
    sptrDataSet data, std::map<int, Blob3D>& blobs, nsx::EquivalenceList& equivalences) const
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

        try {
            // toEllipsoid throws exception if mass is too small
            it->second.toEllipsoid(_peakScale, center, extents, axis);
        } catch (...) {
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
            _handler->log("blob loop: " + std::to_string(progress));
        }
    }

    auto dectector = data->reader()->diffractometer()->detector();
    int nrows = dectector->nRows();
    int ncols = dectector->nCols();
    int nframes = data->nFrames();

    Octree oct(
        Eigen::Vector3d(0.0, 0.0, 0.0),
        Eigen::Vector3d(double(ncols), double(nrows), double(nframes)));
    oct.setMaxDepth(6);
    oct.setMaxStorage(6);

    std::vector<const Ellipsoid*> xyz_sorted_ellipsoids;
    xyz_sorted_ellipsoids.reserve(boxes.size());
    for (auto&& it : boxes)
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

    for (auto it : xyz_sorted_ellipsoids)
        oct.addData(it);

    auto collisions = oct.getCollisions();

    // dummies used to help progress handler
    dummy = 0;
    magic = 0.02 * std::distance(collisions.begin(), collisions.end());

    if (magic < 1)
        magic = 1;

    for (auto&& it = collisions.begin(); it != collisions.end(); ++it) {
        auto&& bit1 = boxes.find(it->first);
        auto&& bit2 = boxes.find(it->second);
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
    if (_handler) {
        _handler->log("Found " + std::to_string(equivalences.size()) + " equivalences");
        _handler->setProgress(100);
    }

    // free memory stored in unordered map
    for (auto&& it : boxes)
        delete it.first;
}

void PeakFinder::mergeEquivalentBlobs(
    std::map<int, Blob3D>& blobs, nsx::EquivalenceList& equivalences) const
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
    for (auto it = blobs.begin(); it != blobs.end();) {
        ++dummy;

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

        // update progress handler
        if ((dummy % magic) == 0 && _handler) {
            double total_dist = std::distance(blobs.begin(), blobs.end());
            double current_dist = std::distance(blobs.begin(), it);
            int progress = static_cast<int>(100.0 * current_dist / total_dist);
            _handler->setProgress(progress);
        }
    }

    // finalize update handler
    if (_handler)
        _handler->log("After merging, " + std::to_string(blobs.size()) + " blobs remain.");
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
void PeakFinder::find(DataList numors)
{
    qDebug("PeakFinder::find ... with %li numors\n", numors.size());
    _current_peaks.clear();
    _current_data = numors;

    int i = 0;
    for (auto&& numor : numors) {
        if (numors.size() > 1)
            qDebug("  numor %i\n", i);
        PeakList numor_peaks;

        auto dectector = numor->reader()->diffractometer()->detector();
        int nrows = dectector->nRows();
        int ncols = dectector->nCols();
        int nframes = numor->nFrames();

        // The blobs found for this numor
        std::map<int, Blob3D> blobs;

        if (_handler) {
            _handler->log("min comp is " + std::to_string(_minSize));
            _handler->log("max comp is " + std::to_string(_maxSize));
            _handler->log("search scale is " + std::to_string(_peakScale));
        }

        _current_label = 0;

        int loop_begin = _framesBegin;
        int loop_end = _framesEnd;
        if (loop_begin == -1)
            loop_begin = 0;
        if (loop_end == -1)
            loop_end = numor->nFrames();

        std::map<int, Blob3D> local_blobs = {{}};
        nsx::EquivalenceList local_equivalences;

// determine begining and ending index of current thread
#pragma omp for
        for (size_t i = 0; i < numor->nFrames(); ++i) {
            if (loop_begin == -1) {
                loop_begin = i;
            }
            loop_end = i + 1;
        }

        // find blobs within the current frame range
        qDebug("PeakFinder::find: findPrimary\n");
        findPrimaryBlobs(numor, local_blobs, local_equivalences, loop_begin, loop_end);


        // merge adjacent blobs
        qDebug("PeakFinder::find: mergeBlobs\n");
        std::cout << local_blobs.size() << std::endl;
        mergeEquivalentBlobs(local_blobs, local_equivalences);

        qDebug("PeakFinder::find: blob loop\n");
        {
            // merge the blobs into the global set
            for (auto&& blob : local_blobs)
                blobs.insert(blob);
        }

        mergeCollidingBlobs(numor, blobs);
        qDebug("PeakFinder::find: Found blob collisions\n");

        if (_handler) {
            _handler->setStatus("Blob finding complete.");
            _handler->log("Blob finding complete.");
            _handler->log("Found " + std::to_string(blobs.size()) + " blobs");
            _handler->setProgress(100);
        }

        if (_handler) {
            _handler->setStatus("Computing bounding boxes...");
            _handler->setProgress(0);
        }

        int count = 0;

        auto&& kernel_size = _convolver->kernelSize();
        auto&& x_offset = kernel_size.first;
        auto&& y_offset = kernel_size.second;

        // AABB used for rejecting peaks which overlaps with detector boundaries
        AABB dAABB(
            Eigen::Vector3d(x_offset, y_offset, 0),
            Eigen::Vector3d(ncols - x_offset, nrows - y_offset, nframes - 1));

        for (auto& blob : blobs) {
            Eigen::Vector3d center, eigenvalues;
            Eigen::Matrix3d eigenvectors;

            blob.second.toEllipsoid(1.0, center, eigenvalues, eigenvectors);
            auto shape = Ellipsoid(center, eigenvalues, eigenvectors);

            auto p = sptrPeak3D(new Peak3D(numor, shape));
            const auto extents = p->shape().aabb().extents();

            // peak too small or too large
            if (extents.maxCoeff() > 1e5 || extents.minCoeff() < 1e-5)
                p->setSelected(false);

            if (extents(2) > _maxFrames)
                p->setSelected(false);

            // peak's bounding box not completely contained in detector image
            if (!dAABB.contains(p->shape().aabb()))
                p->setSelected(false);

            p->setPredicted(false);
            numor_peaks.push_back(p);
            _current_peaks.push_back(p);

            ++count;

            if (_handler) {
                double progress = count * 100.0 / blobs.size();
                _handler->setProgress(progress);
            }
        }
        qDebug("PeakFinder::find: blob loop done\n");
        if (_handler) {
            _handler->setStatus(
                ("Integrating " + std::to_string(numor_peaks.size()) + " peaks...").c_str());
            _handler->setProgress(0);
        }
        std::cout << "PeakFinder::find: " << std::to_string(numor_peaks.size()) << " peaks found."
                  << std::endl;
        numor->close();
        if (_handler)
            _handler->log("Found " + std::to_string(numor_peaks.size()) + " peaks.");
    }
    qDebug("\n");

    if (_handler) {
        _handler->setStatus("Peak finding completed.");
        _handler->setProgress(100);
    }
    qDebug("exit PeakFinder::find\n");
}

} // namespace nsx
