#pragma once


#include <vector>

#include <Eigen/Dense>

#include "AABB.h"

namespace nsx {

class FitProfile {
public:
    FitProfile() = default;
    FitProfile(const AABB& aabb, int nx, int ny, int nz);

    double at(size_t i, size_t j, size_t k) const;

    double& operator()(size_t i, size_t j, size_t k);
    const double& operator()(size_t i, size_t j, size_t k) const;

    bool addValue(const Eigen::Vector3d& x, double y);
    size_t addSubdividedValue(const Eigen::Vector3d& x, double y, size_t subdivide);

    const Eigen::Vector3d& dx() const;

    const Eigen::Vector3i& shape() const;

    size_t count() const;

    double predict(const Eigen::Vector3d& x) const;

    //! Normalize the profile so that the sum is equal to one. Returns false if it cannot be normalized.
    bool normalize();

    FitProfile& operator+=(const FitProfile& other);

    //! Compute the ellipsoid corresponding to the center of mass and inertia tensor.
    Ellipsoid ellipsoid() const;

    //! Return the bounding box of the profile
    const AABB& aabb() const;

private:
    AABB _aabb;
    Eigen::Vector3d _dx;
    Eigen::Vector3i _shape;
    size_t _count;
    std::vector<double> _profile;
};

} // end namespace nsx
