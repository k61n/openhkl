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

#ifndef NSXTOOL_HMatrix_H_
#define NSXTOOL_HMatrix_H_

#include <initializer_list>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

namespace SX
{

namespace Geometry
{

namespace ublas = boost::numeric::ublas;

template <typename T, std::size_t D>
class HMatrix : public ublas::bounded_matrix<T,D,D>
{
public:

	//! constructor
	HMatrix();

	//! copy constructor
	HMatrix(const HMatrix& other);

	//! move copy constructor
	HMatrix(HMatrix&& other);

	//! assignment operator
	HMatrix<T,D>& operator=(const HMatrix& other);

	//! move assignment operator
	HMatrix<T,D>& operator=(HMatrix&& other);

	//! set a scaling tranformation from a ublas vector
	void scaling(const ublas::bounded_vector<T,D-1>& scaleVector);

	//! set a scaling tranformation from an initializer list
	void scaling(const std::initializer_list<T>& list);

	//! set a scaling tranformation from a single value
	void scaling(T value);

	//! set a translation tranformation from a ublas vector
	void translation(const ublas::bounded_vector<T,D-1>& scaleVector);

	//! set a translation tranformation from an initializer list
	void translation(const std::initializer_list<T>& list);

	//! set a translation tranformation from a single value
	void translation(T value);

	//! set a rotation tranformation from an an axis and an angle
	void rotation(const ublas::bounded_vector<T,D-1>& axis, double angle);


};

template <typename T, std::size_t D>
HMatrix<T,D>::HMatrix() : ublas::bounded_matrix<T,D,D>(ublas::zero_matrix<T>(D,D))
{
}

template <typename T, std::size_t D>
HMatrix<T,D>::HMatrix(const HMatrix& other)
{
	this->data() = other.data();
}

template <typename T, std::size_t D>
HMatrix<T,D>::HMatrix(HMatrix&& other)
{
	this->assign_temporary(other);
}

template<typename T, std::size_t D>
HMatrix<T,D>& HMatrix<T,D>::operator=(const HMatrix<T,D>& other)
{
	if (this != &other)
		this->data() = other.data();

	return *this;
}

template<typename T, std::size_t D>
HMatrix<T,D>& HMatrix<T,D>::operator=(HMatrix<T,D>&& other)
{
	if (this != &other)
		this->assign_temporary(other);

	return *this;
}

template<typename T, std::size_t D>
void HMatrix<T,D>::scaling(const ublas::bounded_vector<T,D-1>& scaleVector)
{
	this->clear();
	for (std::size_t i=0; i<D-1; ++i)
		this->operator()(i,i) = scaleVector[i];
	this->operator()(D-1,D-1) = 1.0;
}

template<typename T, std::size_t D>
void HMatrix<T,D>::scaling(const std::initializer_list<T>& list)
{
	this->clear();
	std::size_t comp(0);
	for (auto it=list.begin();list.end();++it)
	{
		this->operator()(comp,comp) = *it;
		++comp;
	}
	this->operator()(D-1,D-1) = 1.0;
}

template<typename T, std::size_t D>
void HMatrix<T,D>::scaling(T value)
{
	this->clear();
	for (std::size_t i=0; i<D-1;++i)
		this->operator()(i,i) = value;
	this->operator()(D-1,D-1) = 1.0;
}

template<typename T, std::size_t D>
void HMatrix<T,D>::translation(const ublas::bounded_vector<T,D-1>& transVector)
{
	this->assign(ublas::identity_matrix<T>(D));
	for (std::size_t i=0; i<D-1;++i)
		this->operator()(i,D-1) = transVector[i];
}

template<typename T, std::size_t D>
void HMatrix<T,D>::translation(const std::initializer_list<T>& list)
{
	this->assign(ublas::identity_matrix<T>(D));
	std::size_t comp(0);
	for (auto it=list.begin();list.end();++it)
	{
		this->operator()(comp,D-1) = *it;
		++comp;
	}
}

template<typename T, std::size_t D>
void HMatrix<T,D>::translation(T value)
{
	this->assign(ublas::identity_matrix<T>(D));
	for (std::size_t i=0; i<D-1;++i)
		this->operator()(i,D-1) = value;
}

} // namespace Geometry

} // namespace SX

#endif /* NSXTOOL_HMatrix_H_ */
