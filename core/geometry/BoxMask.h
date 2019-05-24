#ifndef CORE_GEOMETRY_BOXMASK_H
#define CORE_GEOMETRY_BOXMASK_H

#include "AABB.h"
#include "IMask.h"

namespace nsx {

//! Construct a detector mask from an axis-aligned bounding box.
class BoxMask : public IMask {

public:
    BoxMask() = delete;

    BoxMask(const BoxMask& other) = default;

    BoxMask(const AABB& aabb);

    ~BoxMask() = default;

    BoxMask& operator=(const BoxMask& other) = default;

    IMask* clone() const override;

    bool collide(const Ellipsoid& ellipsoid) const override;

private:
    AABB _aabb;
};

} // end namespace nsx

#endif // CORE_GEOMETRY_BOXMASK_H
