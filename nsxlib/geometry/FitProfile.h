#pragma once


#include <vector>

#include <Eigen/Dense>

#include "AABB.h"

namespace nsx {

class FitProfile {
public:
    FitProfile() = default;
    FitProfile(const AABB& aabb, size_t nx, size_t ny, size_t nz);

    double at(size_t i, size_t j, size_t k) const;

    double& operator()(size_t i, size_t j, size_t k);
    const double& operator()(size_t i, size_t j, size_t k) const;

    bool addValue(const Eigen::Vector3d& x, double y);
    size_t addSubdividedValue(const Eigen::Vector3d& x, double y, size_t subdivide);

    const Eigen::Vector3d& dx() const;

    size_t count() const;

private:
    AABB _aabb;
    Eigen::Vector3d _dx;
    size_t _n[3];
    size_t _count;
    std::vector<double> _profile;
};

} // end namespace nsx
