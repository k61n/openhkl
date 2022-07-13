//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      tables/crystal/MillerIndex.h
//! @brief     Defines class MillerIndex
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_TABLES_CRYSTAL_MILLERINDEX_H
#define OHKL_TABLES_CRYSTAL_MILLERINDEX_H

#include "base/geometry/ReciprocalVector.h"

#include <Eigen/Dense>
#include <vector>

namespace ohkl {

class UnitCell;

//! Class to define the Miller hkl indices.

//! This class allows to enforce dimensional and C++-type consistency when
//! dealing with vectors in algorithms dedicated to crystallographic data.

class MillerIndex {
 public:
    MillerIndex() = default;
    MillerIndex(const MillerIndex& other) = default;

    //! Constructor from its 3 components and the error on hkl
    MillerIndex(int h, int k, int l);

    //! Construct from a given q vector and unit cell
    // MillerIndex(const ReciprocalVector &q, const UnitCell &unit_cell);

    //! (Python) Construct from a given q vector and unit cell
    MillerIndex(const ReciprocalVector q, const UnitCell& unit_cell);

    //! Construct a MillerIndex from an Eigen row vector of integer
    explicit MillerIndex(const Eigen::RowVector3i& hkl);
    MillerIndex(const Eigen::RowVector3i& hkl, const Eigen::RowVector3d& error);
    MillerIndex& operator=(const MillerIndex& other) = default;
    ~MillerIndex() = default;

    //! Returns indices in the form of an Eigen row vector
    const Eigen::RowVector3i& rowVector() const;

    //! Returns the value of an element of the vector
    int operator[](int index) const;
    //! Returns a reference to an element of the vector
    int& operator[](int index);
    //! Returns the value of an element of the vector
    int operator()(int index) const;
    //! Returns a reference to an element of the vector
    int& operator()(int index);
    //! Print information about a MillerIndex to a stream
    void print(std::ostream& os) const;

    bool operator<(const MillerIndex& other) const;

    //! Returns the indexing error (difference between "true" hkl and integers)
    const Eigen::RowVector3d& error() const;

    //! Returns true if it is indexed, i.e. hkl are integral to within the
    //! tolerance
    bool indexed(double tolerance) const;

    //! direct h output for python
    int h() const;
    //! direct k output for python
    int k() const;
    //! direct l output for python
    int l() const;

 private:
    Eigen::RowVector3i _hkl;
    Eigen::RowVector3d _error;
};

//! Overload operator<< with MillerIndex type
std::ostream& operator<<(std::ostream& os, const MillerIndex& hkl);

using MillerIndexList = std::vector<MillerIndex>;

} // namespace ohkl

#endif // OHKL_TABLES_CRYSTAL_MILLERINDEX_H
