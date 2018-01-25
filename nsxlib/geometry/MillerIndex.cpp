#include <cmath>

#include <Eigen/Dense>

#include "MillerIndex.h"
#include "Peak3D.h"
#include "ReciprocalVector.h"
#include "UnitCell.h"

namespace nsx {

MillerIndex::MillerIndex(int h, int k, int l) : _hkl(h,k,l), _error(Eigen::RowVector3d::Zero())
{
}

MillerIndex::MillerIndex(const Eigen::RowVector3i& hkl) : _hkl(hkl), _error(Eigen::RowVector3d::Zero())
{
}

MillerIndex::MillerIndex(sptrPeak3D peak, sptrUnitCell unit_cell)
{
    auto&& q = peak->getQ();
    const Eigen::RowVector3d hkl = q.rowVector() * unit_cell->basis();

    auto h = std::lround(hkl[0]);
    auto k = std::lround(hkl[1]);
    auto l = std::lround(hkl[2]);

    _hkl = Eigen::RowVector3i(h,k,l);

    _error = q.rowVector() - _hkl.cast<double>();
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

const Eigen::RowVector3d& MillerIndex::error() const
{
    return _error;
}

} // end namespace nsx
