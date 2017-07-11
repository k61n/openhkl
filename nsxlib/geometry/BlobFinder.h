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

#ifndef NSXLIB_BLOBFINDER_H
#define NSXLIB_BLOBFINDER_H

#include <algorithm>
#include <iterator>
#include <map>
#include <queue>
#include <vector>
#include <stdexcept>

#include "../data/DataTypes.h"
#include "../geometry/Blob3D.h"
#include "../geometry/Ellipsoid.h"
#include "../geometry/GeometryTypes.h"
#include "../mathematics/MathematicsTypes.h"
#include "../utils/UtilsTypes.h"

namespace nsx {

/* Class used for blob-finding, which is the first step of peak-finding.
 * The use of IFrameIterator allows for custom iterators, e.g. which work multi-threaded.
 * Since blob-finding may take some time on large data sets, ProgressHandler is used to give feedback to the GUI.
 *
 */
class BlobFinder {

public:

    BlobFinder(sptrDataSet data);

    Blob3DUMap find(unsigned int begin, unsigned int end);

    void findBlobs(std::unordered_map<int,Blob3D>& blobs, EquivalenceList& equivalences, unsigned int begin, unsigned int end);

    static void registerEquivalence(int a, int b, EquivalenceList& equivalences);

    static bool sortEquivalences(const std::pair<int,int>& pa, const std::pair<int,int>& pb);

    static std::map<int,int> removeDuplicates(EquivalenceList& equivalences);

    static void reassignEquivalences(std::map<int,int>& equivalences);

    //! sets progress handler callback function
    void setProgressHandler(sptrProgressHandler handler);

    void setThreshold(double threshold);

    void setConfidence(double confidence);

    void setMedian(double median);

    void setMinComp(int minComp);

    void setMaxComp(int maxComp);

    void setRelative(bool isRelative);

    void findCollisions(std::unordered_map<int,Blob3D>& blobs, EquivalenceList& equivalences) const;

    //! Sets the filter, which allows for more sophisticated blob-finding
    void setFilter(FilterCallback callback);

    void mergeBlobs(std::unordered_map<int,Blob3D>& blobs, EquivalenceList& equivalences) const;


    void eliminateBlobs(std::unordered_map<int,Blob3D>& blobs) const;

private:
    double _threshold;
    double _confidence;
    double _median;

    int _minComp;
    int _maxComp;

    bool _isRelative;

    mutable sptrDataSet _data;
    FilterCallback _filterCallback;
    mutable sptrProgressHandler _progressHandler;

    unsigned int _nrows, _ncols, _nframes;
    unsigned int _currentlabel;
};

} // end namespace nsx

#endif // NSXLIB_BLOBFINDER_H

