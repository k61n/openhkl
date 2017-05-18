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
#include <map>
#include <queue>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <iterator>
#include <memory>

#include "Blob2D.h"
#include "Blob3D.h"
#include "Ellipsoid.h"
#include "IShape.h"

#include "../utils/Types.h"

#include "../data/IData.h"

#include "../utils/ProgressHandler.h"

namespace nsx {

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

typedef std::unordered_map<const IShape2D*,int> shape2Dmap;
typedef std::unordered_map<const IShape3D*,int> shape3Dmap;

/* Class used for blob-finding, which is the first step of peak-finding.
 * The use of IFrameIterator allows for custom iterators, e.g. which work multi-threaded.
 * Since blob-finding may take some time on large data sets, ProgressHandler is used to give feedback to the GUI.
 *
 */
class BlobFinder {

public:

    using FilterCallback = std::function<RealMatrix(const RealMatrix&)>;

    BlobFinder(std::shared_ptr<DataSet> data);

    blob3DCollection find(unsigned int begin, unsigned int end);

    void findBlobs(std::unordered_map<int,Blob3D>& blobs, vipairs& equivalences, unsigned int begin, unsigned int end);

    static void registerEquivalence(int a, int b, vipairs& equivalences);

    static bool sortEquivalences(const ipair& pa, const ipair& pb);

    static imap removeDuplicates(vipairs& equivalences);

    static void reassignEquivalences(imap& equivalences);

    //void findBlobs() const;

    //void mergeBlobs();

    //! sets progress handler callback function
    void setProgressHandler(std::shared_ptr<ProgressHandler> handler);

    void setThreshold(double threshold);

    void setConfidence(double confidence);

    void setMedian(double median);

    void setMinComp(int minComp);

    void setMaxComp(int maxComp);

    void setRelative(bool isRelative);


    void findCollisions(std::unordered_map<int,Blob3D>& blobs, vipairs& equivalences) const;

    //! Sets the filter, which allows for more sophisticated blob-finding
    void setFilter(FilterCallback callback);

    void mergeBlobs(std::unordered_map<int,Blob3D>& blobs, vipairs& equivalences) const;


    void eliminateBlobs(std::unordered_map<int,Blob3D>& blobs) const;



private:
    double _threshold;
    double _confidence;
    double _median;

    int _minComp;
    int _maxComp;

    bool _isRelative;

    mutable std::shared_ptr<DataSet> _data;
    FilterCallback _filterCallback;
    mutable std::shared_ptr<ProgressHandler> _progressHandler;

    unsigned int _nrows, _ncols, _nframes;
    unsigned int _currentlabel;
};

} // end namespace nsx

#endif // NSXLIB_BLOBFINDER_H

