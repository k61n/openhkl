#include "Ellipsoid.h"
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

FitProfile& FitProfile::operator+=(const FitProfile& other)
{
    // special case: current profile is empty
    if (_profile.empty()) {
        *this = other;
        return *this;
    }

    if (_shape != other._shape) {
        throw std::runtime_error("FitProfile: cannot add profiles of different dimensions");
    }
    
    double dx = _aabb.extents().squaredNorm();
    auto dlb = _aabb.lower() - other._aabb.lower();
    auto dub = _aabb.upper() - other._aabb.upper();

    if (dlb.squaredNorm() > 1e-6*dx || dub.squaredNorm() > 1e-6*dx) {
        throw std::runtime_error("FitProfile: cannot add profiles with different bounding boxes");
    }

    assert(_profile.size() == other._profile.size());

    for (size_t i = 0; i < _profile.size(); ++i) {
        _profile[i] += other._profile[i];
    }

    return *this;
}

Ellipsoid FitProfile::ellipsoid() const
{
    const Eigen::Vector3d lower = _dx / 2.0 + _aabb.lower();
    double mass = 0;
    Eigen::Matrix3d cov;
    Eigen::Vector3d com;

    for (int i = 0; i < _shape[0]; ++i) {
        for (int j = 0; j < _shape[1]; ++j) {
            for (int k = 0; k < _shape[2]; ++k) {
                const int idx = i+_shape[0]*(j+_shape[1]*k);
                Eigen::Vector3d x = lower;
                x(0) += i*_dx(0);
                x(1) += j*_dx(1);
                x(2) += k*_dx(2);

                const double dm = _profile[idx];

                com += dm*x;
                cov += dm*x*x.transpose();
                mass += dm;
            }
        }
    }

    com /= mass;
    cov /= mass;
    cov -= com * com.transpose();

    return Ellipsoid(com, cov.inverse());
}

} // end namespace nsx
