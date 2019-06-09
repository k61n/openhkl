//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/crystal/MillerIndex.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_CRYSTAL_MILLERINDEX_H
#define CORE_CRYSTAL_MILLERINDEX_H


#include "core/experiment/CrystalTypes.h"
#include "core/geometry/ReciprocalVector.h"

namespace nsx {

//! \brief Class to define the Miller indices (a.k.a hkl index)
//!
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
    MillerIndex(const ReciprocalVector q, const UnitCell unit_cell);

    //! Construct a MillerIndex from an Eigen row vector of integer
    explicit MillerIndex(const Eigen::RowVector3i& hkl);
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

} // namespace nsx

#endif // CORE_CRYSTAL_MILLERINDEX_H
