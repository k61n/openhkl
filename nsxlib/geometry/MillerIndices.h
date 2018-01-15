#pragma once

#include <iostream>

#include <Eigen/Dense>

namespace nsx {

//! \brief Class to define the Miller indices (a.k.a hkl vector)
//!
//! This class allows to enforce dimensional and C++-type consistency when
//! dealing with vectors in algorithms dedicated to crystallographic data.
class MillerIndices
{
public:

    //! Default constructor
    MillerIndices()=default;

    //! Copy constructor
    MillerIndices(const MillerIndices& other)=default;

    //! Constructor from its 3 components
    explicit MillerIndices(int h, int k, int l);

    //! Construct a MillerIndices from an Eigen row vector of integer
    explicit MillerIndices(const Eigen::RowVector3i& hkl);

    //! Assignment operator
    MillerIndices& operator=(const MillerIndices& other)=default;

    //! Destructor
    ~MillerIndices()=default;

    //! Cast operator to an Eigen row vector of integer
    explicit operator const Eigen::RowVector3i& () const;

    //! Cast operator to an Eigen row vector of double
    operator Eigen::RowVector3d () const;

    //! Return the value of an element of the vector
    int operator[](int index) const;

    //! Return a reference to an element of the vector
    int& operator[](int index);

    //! Return the value of an element of the vector
    int operator()(int index) const;

    //! Return a reference to an element of the vector
    int& operator()(int index);

    //! Print information about a MillerIndices to a stream
    void print(std::ostream& os) const;

private:
    Eigen::RowVector3i _hkl;
};

//! Overload operator<< with MillerIndices type
std::ostream& operator<<(std::ostream& os, const MillerIndices& hkl);

} // end namespace nsx
