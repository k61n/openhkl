#include "FitProfile.h"

namespace nsx {


FitProfile::FitProfile(const AABB& aabb, int nx, int ny, int nz):
    _aabb(aabb), _shape(nx, ny, nz), _count(0), _profile(nx*ny*nz, 0.0),
    _dx()
{
    if (nx < 1 || ny < 1 || nz < 1) {
        throw std::runtime_error("FitProfile: size must be positive!");
    }

    _dx = aabb.upper() - aabb.lower();
    for (int i = 0; i < 3; ++i) {
        _dx(i) /= _shape[i];
    }
}

double FitProfile::at(size_t i, size_t j, size_t k) const
{
    if (i >= _shape[0] || j >= _shape[1] || k >= _shape[2]) {
        throw std::runtime_error("FitProfile::at() index out of bounds");
    }
    return _profile[i+_shape[0]*(j+_shape[1]*k)];
}

double& FitProfile::operator()(size_t i, size_t j, size_t k)
{
    assert(i < _shape[0]);
    assert(j < _shape[1]);
    assert(k < _shape[2]);
    return _profile[i+_shape[0]*(j+_shape[1]*k)];
}

const double& FitProfile::operator()(size_t i, size_t j, size_t k) const
{
    assert(i < _shape[0]);
    assert(j < _shape[1]);
    assert(k < _shape[2]);
    return _profile[i+_shape[0]*(j+_shape[1]*k)];
}

bool FitProfile::addValue(const Eigen::Vector3d& x, double y)
{
    const auto& ub = _aabb.upper();
    const auto& lb = _aabb.lower();
    int idx[3];
    
    for (int i = 0; i < 3; ++i) {     
        idx[i] = int((x(i)-lb(i))/_dx[i]);
        // point is out of bounds!
        if (idx[i] < 0 || idx[i] >= _shape[i]) {
            return false;
        }
    }
    _profile[idx[0]+_shape[0]*(idx[1]+_shape[1]*idx[2])] += y;
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

const Eigen::Vector3i& FitProfile::shape() const
{
    return _shape;
}

double FitProfile::predict(const Eigen::Vector3d& x) const
{
    const auto& ub = _aabb.upper();
    const auto& lb = _aabb.lower();
    int idx[3];
    
    for (int i = 0; i < 3; ++i) {     
        idx[i] = int((x(i)-lb(i))/_dx[i]);
        // point is out of bounds!
        if (idx[i] < 0 || idx[i] >= _shape[i]) {
            return 0.0;
        }
    }
    return _profile[idx[0]+_shape[0]*(idx[1]+_shape[1]*idx[2])];
}

void FitProfile::normalize()
{
    double sum = 0.0;

    for (const auto& value: _profile) {
        sum += value;
    }

    for (auto& value: _profile) {
        value /= sum;
    }
}

} // end namespace nsx
