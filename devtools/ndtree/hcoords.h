/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef NSXTOOL_HCoords_H_
#define NSXTOOL_HCoords_H_

#include <initializer_list>

#include <boost/numeric/ublas/vector.hpp>

namespace nsx {

namespace Geometry {

namespace ublas = boost::numeric::ublas;

template <typename T, std::size_t D>
class HCoords : public ublas::bounded_vector<T, D> {
public:
  //! constructor
  HCoords();

  //! copy constructor
  HCoords(const HCoords<T, D> &other);

  //! constructor from a bounded_vector
  HCoords(const ublas::bounded_vector<T, D> &other);

  //! constructor from an initializer list
  HCoords(const std::initializer_list<T> &other);

  //! constructor from a single value
  HCoords(T value);

  //! move constructor
  HCoords(HCoords &&other);

  //! assignment operator
  HCoords<T, D> &operator=(const HCoords<T, D> &other);

  //! move assignment operator
  HCoords<T, D> &operator=(HCoords<T, D> &&other);

  //! normalize an homogeneous vector
  void normalize();
};

template <typename T, std::size_t D>
HCoords<T, D>::HCoords()
    : ublas::bounded_vector<T, D>(ublas::zero_vector<T>(D)) {}

template <typename T, std::size_t D>
HCoords<T, D>::HCoords(const HCoords<T, D> &other)
    : ublas::bounded_vector<T, D>(other) {}

template <typename T, std::size_t D>
HCoords<T, D>::HCoords(const ublas::bounded_vector<T, D> &other)
    : ublas::bounded_vector<T, D>(other) {}

template <typename T, std::size_t D>
HCoords<T, D>::HCoords(const std::initializer_list<T> &other) {
  auto dataIt = this->data().begin();
  for (auto it = other.begin(); it != other.end(); ++it)
    *(dataIt++) = *it;
}

template <typename T, std::size_t D>
HCoords<T, D>::HCoords(T value)
    : ublas::bounded_vector<T, D>(ublas::scalar_vector<T>(D, value)) {}

template <typename T, std::size_t D> HCoords<T, D>::HCoords(HCoords &&other) {
  this->assign_temporary(other);
}

template <typename T, std::size_t D>
HCoords<T, D> &HCoords<T, D>::operator=(const HCoords<T, D> &other) {
  if (this != &other)
    ublas::bounded_vector<T, D>::operator=(other);

  return *this;
}

template <typename T, std::size_t D>
HCoords<T, D> &HCoords<T, D>::operator=(HCoords<T, D> &&other) {
  if (this != &other)
    this->assign_temporary(other);

  return *this;
}

template <typename T, std::size_t D> void HCoords<T, D>::normalize() {
  if (this->data()[D - 1] == 0)
    throw("HCoords: zero division error");

  this->operator/=(this->data()[D - 1]);
}

} // namespace Geometry

} // end namespace nsx

#endif /* NSXTOOL_HCoords_H_ */
