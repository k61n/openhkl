#pragma once

#include <Eigen/Dense>
#include <iostream>
#include <vector>

#include "ReciprocalVector.h"

namespace nsx {

class MillerIndex;
using MillerIndexList = std::vector<MillerIndex>;

class UnitCell;

//! \brief Class to define the Miller indices (a.k.a hkl index)
//!
//! This class allows to enforce dimensional and C++-type consistency when
//! dealing with vectors in algorithms dedicated to crystallographic data.
class MillerIndex {
public:
    //! Default constructor
    MillerIndex() = default;

    //! Copy constructor
    MillerIndex(const MillerIndex& other) = default;

    //! Constructor from its 3 components and the error on hkl
    MillerIndex(int h, int k, int l);

    //! Construct from a given q vector and unit cell
    MillerIndex(const ReciprocalVector& q, const UnitCell& unit_cell);

    //! Construct a MillerIndex from an Eigen row vector of integer
    explicit MillerIndex(const Eigen::RowVector3i& hkl);

    //! Assignment operator
    MillerIndex& operator=(const MillerIndex& other) = default;

    //! Destructor
    ~MillerIndex() = default;

    //! Return indices in the form of an Eigen row vector
    const Eigen::RowVector3i& rowVector() const;

    //! Return the value of an element of the vector
    int operator[](int index) const;
    //! Return a reference to an element of the vector
    int& operator[](int index);
    //! Return the value of an element of the vector
    int operator()(int index) const;
    //! Return a reference to an element of the vector
    int& operator()(int index);
    //! Print information about a MillerIndex to a stream
    void print(std::ostream& os) const;

    bool operator<(const MillerIndex& other) const;

    //! Return the indexing error (difference between "true" hkl and integers)
    const Eigen::RowVector3d& error() const;

    //! Return true if it is indexed, i.e. hkl are integral to within the tolerance
    bool indexed(double tolerance) const;

private:
    Eigen::RowVector3i _hkl;
    Eigen::RowVector3d _error;
};

//! Overload operator<< with MillerIndex type
std::ostream& operator<<(std::ostream& os, const MillerIndex& hkl);

} // end namespace nsx
