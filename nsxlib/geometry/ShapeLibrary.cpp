
#include "Ellipsoid.h"
#include "Peak3D.h"
#include "ShapeLibrary.h"

namespace nsx {

ShapeLibrary::ShapeLibrary(): _shapes(), _defaultShape()
{
    _defaultShape.setIdentity();
    _defaultShape /= 100;
}

ShapeLibrary::~ShapeLibrary()
{

}

bool ShapeLibrary::hasShape(const MillerIndex& hkl) const
{
    return _shapes.find(hkl) != _shapes.end();
}

void ShapeLibrary::addShape(const MillerIndex& hkl, const Eigen::Matrix3d& cov)
{
    _shapes[hkl].push_back(cov);
}

void ShapeLibrary::setDefaultShape(const Eigen::Matrix3d& cov)
{
    _defaultShape = cov;
}

Eigen::Matrix3d ShapeLibrary::predict(const MillerIndex& hkl, int dhkl) const
{
    Eigen::Matrix3d shape;
    shape.setZero();
    unsigned int num_neighbors = 0;

    // if found, don't need to find neighbors
    if (_shapes.find(hkl) != _shapes.end()) {
        dhkl = 0;
    }
    // iterate through neighbors, take average
    for (int h = hkl(0)-dhkl; h <= hkl(0)+dhkl; ++h) {
        for (int k = hkl(1)-dhkl; k <= hkl(1)+dhkl; ++k) {
            for (int l = hkl(2)-dhkl; l <= hkl(2)+dhkl; ++l) {
                auto it = _shapes.find(MillerIndex(h, k, l));
                // not found, skip to next neighbor
                if (it == _shapes.end()) {
                    continue;
                }
                const auto& entry = it->second;
                Eigen::Matrix3d shape_hkl;
                shape_hkl.setZero();

                for (const auto& shape: entry) {
                    shape_hkl += shape;
                }
                shape += shape_hkl / entry.size();
                ++num_neighbors;
            }
        }
    }
    if (num_neighbors == 0) {
        return _defaultShape;
    }
    return shape / num_neighbors;
}

bool ShapeLibrary::addPeak(sptrPeak3D peak)
{
    Ellipsoid q_shape;
    auto uc = peak->activeUnitCell();
    auto data = peak->data();

    if (!uc || !data) {
        return false;
    }   

    try {
        q_shape = peak->qShape();        
    } catch(...) {
        return false;
    }

    MillerIndex hkl(peak->q(), *uc);
    addShape(hkl, q_shape.inverseMetric());

    return true;
}

Eigen::Matrix3d ShapeLibrary::meanShape() const
{
    Eigen::Matrix3d mean;
    mean.setZero();
    
    for (const auto& entry: _shapes) {
        Eigen::Matrix3d mean_hkl;
        mean_hkl.setZero();

        for (const auto& shape: entry.second) {
            mean_hkl += shape;
        }
        mean += mean_hkl / entry.second.size();
    }
    return mean / _shapes.size();
}

} // end namespace nsx
