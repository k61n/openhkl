//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/shape/Profile3D.h
//! @brief     Defines class Profile3D
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_SHAPE_PROFILE3D_H
#define CORE_SHAPE_PROFILE3D_H

#include "base/geometry/AABB.h"

namespace nsx {

//! Stores a 3d peak profile

class Profile3D {
public:
    Profile3D();
    //! Construct with given bounding box and number of bins
    Profile3D(const AABB& aabb, int nx, int ny, int nz);
    //! Construct with given bounding box and shape tensor
    Profile3D(const AABB& aabb, const Eigen::Vector3i& shape);
    //! Returns the value of the bin at the given indices
    double at(size_t i, size_t j, size_t k) const;
    //! Returns the value of the bin at the given indices
    double& operator()(size_t i, size_t j, size_t k);
    //! Returns the value of the bin at the given indices
    const double& operator()(size_t i, size_t j, size_t k) const;
    //! Add a value to the profile
    bool addValue(const Eigen::Vector3d& x, double y);
    //! Add a value to the profile, with subdivision
    size_t addSubdividedValue(const Eigen::Vector3d& x, double y, size_t subdivide);
    //! Compute the bin size
    const Eigen::Vector3d& dx() const;
    //! Returns the number of points in the profile
    size_t count() const;
    //! Make a prediction based on the profile
    double predict(const Eigen::Vector3d& x) const;
    //! Normalize the profile so that the sum is equal to one. Returns false if it
    //! cannot be normalized.
    bool normalize();
    //! Add the given profile to this one, with the specified weight
    void addProfile(const Profile3D& other, double weight);
    //! Compute the ellipsoid corresponding to the center of mass and inertia
    //! tensor.
    Ellipsoid ellipsoid() const;
    //! Returns the bounding box of the profile
    const AABB& aabb() const;
    //! Returns the shape of the underlying data
    const Eigen::Vector3i& shape() const;

private:
    AABB _aabb;
    Eigen::Vector3d _dx;
    Eigen::Vector3i _shape;
    size_t _count;
    std::vector<double> _profile;
};

} // namespace nsx

#endif // CORE_SHAPE_PROFILE3D_H
