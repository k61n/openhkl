#pragma once

#include <vector>

#include <Eigen/Dense>

#include "AABB.h"

namespace nsx {

//! \brief Class to store a 3d peak profile
class Profile3D {
public:
    //! Default constructor
    Profile3D();
    //! Construct with given bounding box and number of bins
    Profile3D(const AABB& aabb, int nx, int ny, int nz);
    //! Construct with given bounding box and shape tensor
    Profile3D(const AABB& aabb, const Eigen::Vector3i& shape);
    //! Return the value of the bin at the given indices
    double at(size_t i, size_t j, size_t k) const;
    //! Return the value of the bin at the given indices
    double& operator()(size_t i, size_t j, size_t k);
    //! Return the value of the bin at the given indices
    const double& operator()(size_t i, size_t j, size_t k) const;
    //! Add a value to the profile
    bool addValue(const Eigen::Vector3d& x, double y);
    //! Add a value to the profile, with subdivision
    size_t addSubdividedValue(const Eigen::Vector3d& x, double y, size_t subdivide);
    //! Compute the bin size
    const Eigen::Vector3d& dx() const;
    //! Return the number of points in the profile
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
    //! Return the bounding box of the profile
    const AABB& aabb() const;
    //! Return the shape of the underlying data
    const Eigen::Vector3i& shape() const;

private:
    AABB _aabb;
    Eigen::Vector3d _dx;
    Eigen::Vector3i _shape;
    size_t _count;
    std::vector<double> _profile;
};

} // end namespace nsx
