#pragma once

#include <iostream>

#include <Eigen/Dense>

namespace nsx {

//! \brief Class to define the Miller indices (a.k.a hkl vector)
//!
//! This class allows to enforce dimensional and C++-type consistency when
//! dealing with vectors in algorithms dedicated to crystallographic data.
class MillerIndex
{
public:

    //! Default constructor
    MillerIndex()=default;

    //! Copy constructor
    MillerIndex(const MillerIndex& other)=default;

    //! Constructor from its 3 components
    MillerIndex(int h, int k, int l);

    //! Construct a MillerIndex from an Eigen row vector of integer
    explicit MillerIndex(const Eigen::RowVector3i& hkl);

    //! Assignment operator
    MillerIndex& operator=(const MillerIndex& other)=default;

    //! Destructor
    ~MillerIndex()=default;

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

private:
    Eigen::RowVector3i _hkl;
};

//! Overload operator<< with MillerIndex type
std::ostream& operator<<(std::ostream& os, const MillerIndex& hkl);

} // end namespace nsx
