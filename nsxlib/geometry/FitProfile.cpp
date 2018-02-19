#include "FitProfile.h"

namespace nsx {


FitProfile::FitProfile(const AABB& aabb, size_t nx, size_t ny, size_t nz):
    _aabb(aabb), _n({nx, ny, nz}), _count(0), _profile(nx*ny*nz, 0.0),
    _dx()
{
    _dx = aabb.upper() - aabb.lower();
    for (int i = 0; i < 3; ++i) {
        _dx(i) /= _n[i];
    }
}

double FitProfile::at(size_t i, size_t j, size_t k) const
{
    if (i >= _n[0] || j >= _n[1] || k >= _n[2]) {
        throw std::runtime_error("FitProfile::at() index out of bounds");
    }
    return _profile[i+_n[0]*(j+_n[1]*k)];
}

double& FitProfile::operator()(size_t i, size_t j, size_t k)
{
    assert(i < _n[0]);
    assert(j < _n[1]);
    assert(k < _n[2]);
    return _profile[i+_n[0]*(j+_n[1]*k)];
}

const double& FitProfile::operator()(size_t i, size_t j, size_t k) const
{
    assert(i < _n[0]);
    assert(j < _n[1]);
    assert(k < _n[2]);
    return _profile[i+_n[0]*(j+_n[1]*k)];
}

bool FitProfile::addValue(const Eigen::Vector3d& x, double y)
{
    const auto& ub = _aabb.upper();
    const auto& lb = _aabb.lower();
    int idx[3];
    
    for (int i = 0; i < 3; ++i) {     
        idx[i] = int((x(i)-lb(i))/_dx[i]);
        // point is out of bounds!
        if (idx[i] < 0 && idx[i] >= _n[i]) {
            return false;
        }
    }
    _profile[idx[0]+_n[0]*(idx[1]+_n[1]*idx[2])] += y;
    ++_count;
    return true;
}

const Eigen::Vector3d& FitProfile::dx() const
{
    return _dx;
}

size_t FitProfile::count() const
{
    return _count;
}

size_t FitProfile::addSubdividedValue(const Eigen::Vector3d& x, double y, size_t subdivide)
{
    size_t n = 0;
    const Eigen::Vector3d lower = x-_dx/2.0+_dx/2.0/subdivide;
    const double scale = 1.0 / (subdivide*subdivide*subdivide);

    for (size_t i = 0; i < subdivide; ++i) {
        for (size_t j = 0; j < subdivide; ++j) {
            for (size_t k = 0; k < subdivide; ++k) {
                Eigen::Vector3d delta(i, j, k);
                delta /= subdivide;
                delta(0) *= _dx(0);
                delta(1) *= _dx(1);
                delta(2) *= _dx(2);

                if (addValue(lower+delta, scale*y)) {
                    ++n;
                }
            }   
        }   
    }
    _count += n;
    return n;
}


} // end namespace nsx
