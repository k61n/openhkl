#pragma once

#include <iostream>

#include <Eigen/Dense>

namespace nsx {

class DirectVector
{
public:

    //! Default constructor
    DirectVector()=default;

    //! Copy constructor
    DirectVector(const DirectVector& other)=default;

    //! Construct a DirectVector from an Eigen column vector
    explicit DirectVector(const Eigen::Vector3d& dvector);

    //! Assignment operator
    DirectVector& operator=(const DirectVector& other)=default;

    ~DirectVector()=default;

    explicit operator const Eigen::Vector3d& () const;

    void print(std::ostream& os) const;

private:
    Eigen::Vector3d _dvector;
};

std::ostream& operator<<(std::ostream& os, const DirectVector& dvector);

} // end namespace nsx
