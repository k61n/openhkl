
#include "Ellipsoid.h"
#include "Peak3D.h"
#include "ShapeLibrary.h"

namespace nsx {

ShapeLibrary::ShapeLibrary(): _shapes(), _defaultShape()
{

}

ShapeLibrary::~ShapeLibrary()
{

}

bool ShapeLibrary::hasShape(const MillerIndex& hkl) const
{
    return _shapes.find(hkl) != _shapes.end();
}

void ShapeLibrary::addShape(const MillerIndex& hkl, const FitProfile& profile)
{   
    _shapes[hkl].push_back(profile);
    _shapes[hkl].back().normalize();
}

void ShapeLibrary::setDefaultShape(const FitProfile& profile)
{
    _defaultShape = profile;
}

FitProfile ShapeLibrary::predict(const MillerIndex& hkl, int dhkl) const
{
    FitProfile mean_shape;
    bool found = false;

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
                found = true;
                const auto& entry = it->second;

                for (const auto& shape: entry) {
                    mean_shape += shape;
                }
            }
        }
    }
    if (!found) {
        return _defaultShape;
    }
    mean_shape.normalize();
    return mean_shape;
}

bool ShapeLibrary::addPeak(sptrPeak3D peak)
{
    auto profile = peak->profile();
    auto uc = peak->activeUnitCell();

    if (!uc || !profile) {
        return false;
    }

    MillerIndex hkl(peak->q(), *uc);
    addShape(hkl, *profile);

    return true;
}

FitProfile ShapeLibrary::meanShape() const
{
    FitProfile mean;
   
    for (const auto& entry: _shapes) {
        for (const auto& shape: entry.second) {
            mean += shape;
        }
    }
    mean.normalize();
    return mean;
}

} // end namespace nsx
