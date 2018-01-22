#include <Eigen/Dense>

#include "MillerIndex.h"

namespace nsx {

MillerIndex::MillerIndex(int h, int k, int l) : _hkl(h,k,l)
{
}

MillerIndex::MillerIndex(const Eigen::RowVector3i& hkl) : _hkl(hkl)
{
}

const Eigen::RowVector3i& MillerIndex::rowVector() const
{
    return _hkl;
}

int MillerIndex::operator[](int index) const
{
    if (index < 0 || index > 2) {
        throw std::runtime_error("Invalid index for a 3D vector");
    }

    return _hkl[index];
}

int& MillerIndex::operator[](int index)
{
    if (index < 0 || index > 2) {
        throw std::runtime_error("Invalid index for a 3D vector");
    }

    return _hkl[index];
}

int MillerIndex::operator()(int index) const
{
    if (index < 0 || index > 2) {
        throw std::runtime_error("Invalid index for a 3D vector");
    }
    return _hkl[index];
}

int& MillerIndex::operator()(int index)
{
    if (index < 0 || index > 2) {
        throw std::runtime_error("Invalid index for a 3D vector");
    }
    return _hkl[index];
}

void MillerIndex::print(std::ostream& os) const
{
    os << _hkl;
}

std::ostream& operator<<(std::ostream& os, const MillerIndex& hkl)
{
    hkl.print(os);
    return os;
}

bool MillerIndex::operator<(const MillerIndex& other) const
{
    for (int i = 0; i < 3; i++) {
        if (_hkl(i) != other._hkl(i)) {
            return _hkl(i) < other._hkl(i);
        }
    }
    return false;
}

} // end namespace nsx
