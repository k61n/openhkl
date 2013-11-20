/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
	BP 156
	6, rue Jules Horowitz
	38042 Grenoble Cedex 9
	France
	chapon[at]ill.fr
    pellegrini[at]ill.fr

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
#ifndef NSXTOOL_AABB_H_
#define NSXTOOL_AABB_H_

#include <initializer_list>
#include <iostream>
#include <vector>

#include <boost/iterator/zip_iterator.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/vector.hpp>

using namespace boost::numeric::ublas;

namespace SX
{

namespace Geometry
{

template<typename T, std::size_t D>
class AABB
{
public:

	//! default constructor
	AABB();

	//! copy constructor
	AABB(const AABB<T,D>& other);

	//! constructor from two ublas vectors
	AABB(const bounded_vector<T,D>& lb, const bounded_vector<T,D>& ub);

	//! constructor from two initializer_list
	AABB(const std::initializer_list<T>& lb, const std::initializer_list<T>& ub);

	//! assignment operator
	AABB<T,D>& operator=(const AABB<T,D>& other);

	//! check for interception with another AABB
	//! inline really makes a difference in speed here.
	inline bool intercept(const AABB<T,D>& other) const
	{
		for (std::size_t i=0; i<D; ++i)
			if (_upperBound[i] < other._lowerBound[i] || _lowerBound[i] > other._upperBound[i])
				return false;

		return true;
	}

	//! setter for the lower and upper bounds of the AABB
	void setBounds(const bounded_vector<T,D>& lb, const bounded_vector<T,D>& ub);

	//! send the object to a stream
	void printSelf(std::ostream&) const;

	typedef boost::tuple< std::initializer_list<T>,std::initializer_list<T> > tuple;

protected:
	// The lower bound point
	bounded_vector<T,D> _lowerBound;
	// The upper bound point
	bounded_vector<T,D> _upperBound;

};

template<typename T, std::size_t D>
AABB<T,D>::AABB()
{
	for (std::size_t i=0; i<D; ++i)
	{
		_lowerBound[i] = 0;
		_upperBound[i] = 1;
	}
}

template<typename T, std::size_t D>
AABB<T,D>::AABB(const AABB<T,D>& other)
{
	_lowerBound = other._lowerBound;
	_upperBound = other._upperBound;
}

template<typename T, std::size_t D>
AABB<T,D>::AABB(const bounded_vector<T,D>& lb, const bounded_vector<T,D>& ub) : _lowerBound(lb), _upperBound(ub)
{
}

template<typename T, std::size_t D>
AABB<T,D>::AABB(const std::initializer_list<T>& lb, const std::initializer_list<T>& ub)
{
	auto it1 = lb.begin();
	auto it2 = ub.begin();
	auto lbit = _lowerBound.begin();
	auto ubit = _upperBound.begin();

	for(;it1!=lb.end();it1++,it2++)
	{
		*(lbit++) = *it1;
		*(ubit++) = *it2;
	}

}


template<typename T, std::size_t D>
AABB<T,D>& AABB<T,D>::operator=(const AABB<T,D>& other)
{
  if (this != &other)
  {
	  _lowerBound = other._lowerBound;
	  _upperBound = other._upperBound;
  }
  return *this;
}

template<typename T, std::size_t D>
std::ostream& operator<<(std::ostream& os, const AABB<T,D>& aabb)
{
	aabb.printSelf(os);
	return os;
}

template<typename T, std::size_t D>
void AABB<T,D>::setBounds(const bounded_vector<T,D>& lb, const bounded_vector<T,D>& ub)
{
	_lowerBound = lb;
	_upperBound = ub;
}

template<typename T, std::size_t D>
void AABB<T,D>::printSelf(std::ostream& os) const
{
  os<<"AABB --> "<<"lower bound"<<_lowerBound<<" , "<<"upper bound"<<_upperBound;
  return;
}

} // namespace Geometry

} // namespace SX

#endif /*NSXTOOL_AABB_H_*/
