//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/geometry/DirectVector.h
//! @brief     Defines class DirectVector
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_BASE_GEOMETRY_DIRECTVECTOR_H
#define OHKL_BASE_GEOMETRY_DIRECTVECTOR_H

#include <Eigen/Dense>

namespace ohkl {

//! A vector in direct space.

//! This class allows to enforce dimensional and C++-type consistency when
//! dealing with vectors in algorithms dedicated to crystallographic data.

class DirectVector {
 public:
    DirectVector() = default; // required by Python wrapper of DirectVectorList
    DirectVector(const DirectVector& other) = default;

    //! Constructor from its 3 components
    explicit DirectVector(double x, double y, double z);

    //! Construct a DirectVector from an Eigen column vector
    explicit DirectVector(const Eigen::Vector3d& dvector);
    DirectVector& operator=(const DirectVector& other) = default;
    ~DirectVector() = default;

    //! Returns the underlying Eigen::Vector3d stored in the DirectVector.
    const Eigen::Vector3d& vector() const;

    //! Returns the value of an element of the vector
    double operator[](int index) const;

    //! Returns a reference to an element of the vector
    const double& operator[](int index);

    //! Print information about a DirectVector to a stream
    void print(std::ostream& os) const;

 private:
    Eigen::Vector3d _dvector;
};

//! Overload operator<< with DirectVector type
std::ostream& operator<<(std::ostream& os, const DirectVector& dvector);

} // namespace ohkl

#endif // OHKL_BASE_GEOMETRY_DIRECTVECTOR_H
