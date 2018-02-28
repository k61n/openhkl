
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


void ShapeLibrary::addShape(const DetectorEvent& ev, const FitProfile& profile)
{   
    _shapes.emplace_back(std::make_pair(ev, profile));
    _shapes.back().second.normalize();
}

void ShapeLibrary::setDefaultShape(const FitProfile& profile)
{
    _defaultShape = profile;
}

FitProfile ShapeLibrary::average(const DetectorEvent& ev, double radius, double nframes) const
{
    FitProfile mean_shape;
    bool found = false;

    // iterate through shapes
    for (const auto& shape: _shapes) {  

        const auto& event = shape.first;
        const auto& profile = shape.second;

        double dx = ev._px - event._px;
        double dy = ev._py - event._py;

        // too far away on detector
        if (dx*dx + dy*dy > radius*radius) {
            continue;
        }

        // frames differ too much
        if (std::fabs(ev._frame - event._frame) > nframes) {
            continue;
        }

        found = true;
        mean_shape += profile;
    }
    if (!found) {
        throw std::runtime_error("ShapeLibrary::average() could not find shape within the specified conditions");
    }
    mean_shape.normalize();
    return mean_shape;
}

FitProfile ShapeLibrary::meanShape() const
{
    FitProfile mean;
   
    for (const auto& entry: _shapes) {
        mean += entry.second;
    }
    mean.normalize();
    return mean;
}

} // end namespace nsx
