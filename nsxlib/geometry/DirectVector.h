#pragma once

#include <iostream>

#include <Eigen/Dense>

namespace nsx {

//! \brief Class to define a vector defined in direct space.
//!
//! This class allows to enforce dimensional and C++-type consistency when
//! dealing with vectors in algorithms dedicated to crystallographic data.
class DirectVector
{
public:

    //! Default constructor
    DirectVector()=default;

    //! Copy constructor
    DirectVector(const DirectVector& other)=default;

    //! Constructor from its 3 components
    explicit DirectVector(double x, double y, double z);

    //! Construct a DirectVector from an Eigen column vector
    explicit DirectVector(const Eigen::Vector3d& dvector);

    //! Assignment operator
    DirectVector& operator=(const DirectVector& other)=default;

    //! Destructor
    ~DirectVector()=default;

    //! Return the underlying Eigen::Vector3d stored in the DirectVector.
    const Eigen::Vector3d& vector() const;

    //! Return a reference to an element of the vector
    double& operator[](int index);

    //! Return the value of an element of the vector
    double operator[](int index) const;

    //! Return a reference to an element of the vector
    double& operator()(int index);

    //! Return the value of an element of the vector
    double operator()(int index) const;

    //! Print information about a DirectVector to a stream
    void print(std::ostream& os) const;
   

private:
    Eigen::Vector3d _dvector;
};

//! Overload operator<< with DirectVector type
std::ostream& operator<<(std::ostream& os, const DirectVector& dvector);

} // end namespace nsx
