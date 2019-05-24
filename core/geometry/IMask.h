#ifndef CORE_GEOMETRY_IMASK_H
#define CORE_GEOMETRY_IMASK_H

namespace nsx {

class Ellipsoid;

//! Abstract class for detector mask
class IMask {

public:
    virtual ~IMask() = 0;

    virtual bool collide(const Ellipsoid& e) const = 0;

    virtual IMask* clone() const = 0;

protected:
    IMask() = default;

    IMask(const IMask& other) = default;

    IMask& operator=(const IMask& other) = default;
};

} // end namespace nsx

#endif // CORE_GEOMETRY_IMASK_H
