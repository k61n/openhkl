//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/geometry/ReciprocalVector.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_GEOMETRY_RECIPROCALVECTOR_H
#define CORE_GEOMETRY_RECIPROCALVECTOR_H

#include <iostream>

#include <Eigen/Dense>

namespace nsx {

//! \brief Class to define a vector defined in reciprocal space.
//!
//! This class allows to enforce dimensional and C++-type consistency when
//! dealing with vectors in algorithms dedicated to crystallographic data.
class ReciprocalVector {
public:
    ReciprocalVector() = default;
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

private:
    Eigen::RowVector3d _rvector;
};

//! Overload operator<< with ReciprocalVector type
std::ostream& operator<<(std::ostream& os, const ReciprocalVector& rvector);

} // end namespace nsx

#endif // CORE_GEOMETRY_RECIPROCALVECTOR_H
