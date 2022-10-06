//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      tables/crystal/MillerIndex.cpp
//! @brief     Implements class MillerIndex
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/UnitCell.h"

#include <Eigen/Dense>
#include <cmath>

namespace ohkl {

MillerIndex::MillerIndex(int h, int k, int l)
    : _hkl(h, k, l), _error(Eigen::RowVector3d::Zero()) { }

MillerIndex::MillerIndex(const Eigen::RowVector3i& hkl)
    : _hkl(hkl), _error(Eigen::RowVector3d::Zero())
{
}

MillerIndex::MillerIndex(const Eigen::RowVector3i& hkl, const Eigen::RowVector3d& error)
    : _hkl(hkl), _error(error)
{
}

// MillerIndex::MillerIndex(const ReciprocalVector &q, const UnitCell &unit_cell) {
//   const Eigen::RowVector3d hkld = q.rowVector() * unit_cell.basis();

//   auto h = std::lround(hkld[0]);
//   auto k = std::lround(hkld[1]);
//   auto l = std::lround(hkld[2]);

//   _hkl = Eigen::RowVector3i(h, k, l);
//   _error = hkld - _hkl.cast<double>();
// }

MillerIndex::MillerIndex(const ReciprocalVector q, const UnitCell& unit_cell)
{
    const Eigen::RowVector3d hkld = q.rowVector() * unit_cell.basis();

    auto h = std::lround(hkld[0]);
    auto k = std::lround(hkld[1]);
    auto l = std::lround(hkld[2]);

    _hkl = Eigen::RowVector3i(h, k, l);
    _error = hkld - _hkl.cast<double>();
}

const Eigen::RowVector3i& MillerIndex::rowVector() const
{
    return _hkl;
}

int MillerIndex::operator[](int index) const
{
    if (index < 0 || index > 2)
        throw std::runtime_error("Invalid index for a 3D vector");
    return _hkl[index];
}

int& MillerIndex::operator[](int index)
{
    if (index < 0 || index > 2)
        throw std::runtime_error("Invalid index for a 3D vector");
    return _hkl[index];
}

int MillerIndex::operator()(int index) const
{
    if (index < 0 || index > 2)
        throw std::runtime_error("Invalid index for a 3D vector");
    return _hkl[index];
}

int& MillerIndex::operator()(int index)
{
    if (index < 0 || index > 2)
        throw std::runtime_error("Invalid index for a 3D vector");
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
        if (_hkl(i) != other._hkl(i))
            return _hkl(i) < other._hkl(i);
    }
    return false;
}

const Eigen::RowVector3d& MillerIndex::error() const
{
    return _error;
}

bool MillerIndex::indexed(double tolerance) const
{
    return (std::fabs(_error[0]) < tolerance) && (std::fabs(_error[1]) < tolerance)
        && (std::fabs(_error[2]) < tolerance);
}


int MillerIndex::h() const
{
    return _hkl[0];
}

int MillerIndex::k() const
{
    return _hkl[1];
}

int MillerIndex::l() const
{
    return _hkl[2];
}

} // namespace ohkl
