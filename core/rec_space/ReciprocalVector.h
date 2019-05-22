#pragma once

#include <iostream>

#include <Eigen/Dense>

namespace nsx {

//! \brief Class to define a vector defined in reciprocal space.
//!
//! This class allows to enforce dimensional and C++-type consistency when
//! dealing with vectors in algorithms dedicated to crystallographic data.
class ReciprocalVector {
public:
  //! Default constructor
  ReciprocalVector() = default;

  //! Copy constructor
  ReciprocalVector(const ReciprocalVector &other) = default;

  //! Constructor from its 3 components
  explicit ReciprocalVector(double x, double y, double z);

  //! Construct a ReciprocalVector from an Eigen row vector
  explicit ReciprocalVector(const Eigen::RowVector3d &rvector);

  //! Assignment operator
  ReciprocalVector &operator=(const ReciprocalVector &other) = default;

  //! Destructor
  ~ReciprocalVector() = default;

  //! Cast operator to an Eigen row vector
  const Eigen::RowVector3d &rowVector() const;

  //! Return the value of an element of the vector
  double operator[](int index) const;

  //! Return a reference to an element of the vector
  const double &operator[](int index);

  //! Return the value of an element of the vector
  double operator()(int index) const;

  //! Return a reference to an element of the vector
  double &operator()(int index);

  //! Print information about a ReciprocalVector to a stream
  void print(std::ostream &os) const;

private:
  Eigen::RowVector3d _rvector;
};

//! Overload operator<< with ReciprocalVector type
std::ostream &operator<<(std::ostream &os, const ReciprocalVector &rvector);

} // end namespace nsx
