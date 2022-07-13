//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/geometry/ReciprocalVector.h
//! @brief     Defines class ReciprocalVector
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_BASE_GEOMETRY_RECIPROCALVECTOR_H
#define OHKL_BASE_GEOMETRY_RECIPROCALVECTOR_H

#include <Eigen/Dense>

namespace ohkl {

//! A vector in reciprocal space.

//! This class allows to enforce dimensional and C++-type consistency when
//! dealing with vectors in algorithms dedicated to crystallographic data.
//! Note that it can have an invalid state if frame interpolation fails
//! (_valid = false)

class ReciprocalVector {
 public:
    ReciprocalVector();
    ReciprocalVector(const ReciprocalVector& other) = default;

    //! Constructor from its 3 components
    explicit ReciprocalVector(double x, double y, double z);

    //! Construct a ReciprocalVector from an Eigen row vector
    explicit ReciprocalVector(const Eigen::RowVector3d& rvector);
    ReciprocalVector& operator=(const ReciprocalVector& other) = default;
    ~ReciprocalVector() = default;

    //! Cast operator to an Eigen row vector
    const Eigen::RowVector3d& rowVector() const;

    //! Returns the value of an element of the vector
    double operator[](int index) const;

    //! Returns a reference to an element of the vector
    const double& operator[](int index);

    //! Print information about a ReciprocalVector to a stream
    void print(std::ostream& os) const;

    bool isValid() const;

 private:
    Eigen::RowVector3d _rvector;
    bool _valid;
};

//! Overload operator<< with ReciprocalVector type
std::ostream& operator<<(std::ostream& os, const ReciprocalVector& rvector);

} // namespace ohkl

#endif // OHKL_BASE_GEOMETRY_RECIPROCALVECTOR_H
