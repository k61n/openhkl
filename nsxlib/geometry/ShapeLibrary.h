#pragma once

#include <Eigen/Dense>
#include <map>
#include <vector>

#include "MillerIndex.h"

namespace nsx {

//! \brief Class to store a library of peak shapes _as covariance matrices n q space_.
//!
class ShapeLibrary {
public:
    ShapeLibrary();
    ~ShapeLibrary();

    //! Return true if there is a shape corresponding to given hkl.
    bool hasShape(const MillerIndex& hkl) const;

    //! Add a shape to the library. Note that if an entry exists, this
    //! Method _adds_ the shape to the existing list, and does _not_ replace it.
    void addShape(const MillerIndex& hkl, const Eigen::Matrix3d& cov);

    //! Add the shape of the given peak. May return false if the peak's q-shape cannot be computed.
    bool addPeak(sptrPeak3D peak);

    //! Set the default shape
    void setDefaultShape(const Eigen::Matrix3d& cov);

    //! Predict the shape for a given hkl. If an entry for the specified hkl exists, we simply retrieve it.
    //! If not, then take the average shape of peaks with Miller indices in the range hkl +- dhkl. 
    //! If no neighbors can be found, the default shape is used.
    Eigen::Matrix3d predict(const MillerIndex& hkl, int dhkl) const;

    //! Return the average of all shapes
    Eigen::Matrix3d meanShape() const;

private:
    std::map<MillerIndex, std::vector<Eigen::Matrix3d>> _shapes;
    Eigen::Matrix3d _defaultShape;
};

} // end namespace nsx
