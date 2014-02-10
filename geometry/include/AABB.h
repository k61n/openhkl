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
#include <stdexcept>
#include <Eigen/Dense>
#include <iostream>
#include <vector>

namespace SX
{

namespace Geometry
{

typedef unsigned int uint;


/*! \brief Axis-Aligned Bounding-Box in D dimension.
 *
 * AABB are used to bound objects in a simple-way since
 * their axes are aligned with the coordinates of the world.
 * Used for fast collision detection test, as well as a
 * way to iterate quickly over region of interest in data
 * from images or volumes.
 */
template<typename T, uint D>
class AABB
{
public:
	typedef typename Eigen::Matrix<T,D,1> vector;
	//! Default constructor
	AABB();

	//! Copy constructor
	AABB(const AABB<T,D>& other);

	//! Constructor from two ublas vectors
	AABB(const vector& lb, const vector& ub);

	//! Constructor from two initializer_lists
	AABB(const std::initializer_list<T>& lb, const std::initializer_list<T>& ub);

	//! Destructor
	virtual ~AABB();

	//! Assignment operator
	AABB<T,D>& operator=(const AABB<T,D>& other);

	//! Check for intersection with another AABB
	//! Return true if touch or overlap
	inline bool intercept(const AABB<T,D>& other) const
	{
		for (uint i=0; i<D; ++i)
		{
			if (_upperBound(i) < other._lowerBound(i) || _lowerBound(i) > other._upperBound(i))
				return false;
		}
		return true;
	}

	//! Check whether a given point is inside the AABB
	bool is_inside_aabb(const std::initializer_list<T>& point) const;

	//! Setter for the lower and upper bounds of the AABB
	void setBounds(const vector& lb, const vector& ub);

	//! Return the center of the bounding box
	vector getCenter() const;

	//! Return the extends of the bounding box
	vector getExtents() const;

	//! Send the object to a stream
	void printSelf(std::ostream&) const;

protected:
	// The lower bound point
	vector _lowerBound;
	// The upper bound point
	vector _upperBound;

};

template<typename T, uint D>
AABB<T,D>::AABB()
{
 // The bounded_vectors are left non-initialized.
}

template<typename T, uint D>
AABB<T,D>::AABB(const AABB<T,D>& other)
{
	_lowerBound = other._lowerBound;
	_upperBound = other._upperBound;
}

template<typename T, uint D>
AABB<T,D>::AABB(const vector& lb, const vector& ub) : _lowerBound(lb), _upperBound(ub)
{
	for (uint i=0;i<D;++i)
	{
		if (_lowerBound(i)>_upperBound(i))
			throw std::invalid_argument("AABB: upper limit must be > lower limit");
	}
}

template<typename T, uint D>
AABB<T,D>::AABB(const std::initializer_list<T>& lb, const std::initializer_list<T>& ub)
{
	auto it1 = lb.begin();
	auto it2 = ub.begin();
	auto lbit = _lowerBound.data();
	auto ubit = _upperBound.data();

	for(;it1!=lb.end();it1++,it2++)
	{
		if ((*it1)>(*it2))
			throw std::invalid_argument("AABB: upper limit must be > lower limit");
		*(lbit++) = *it1;
		*(ubit++) = *it2;
	}
}

template<typename T, uint D>
AABB<T,D>::~AABB()
{
}


template<typename T, uint D>
AABB<T,D>& AABB<T,D>::operator=(const AABB<T,D>& other)
{
  if (this != &other)
  {
	  _lowerBound = other._lowerBound;
	  _upperBound = other._upperBound;
  }
  return *this;
}

template<typename T, uint D>
std::ostream& operator<<(std::ostream& os, const AABB<T,D>& aabb)
{
	aabb.printSelf(os);
	return os;
}

template<typename T,uint D>
bool AABB<T,D>::is_inside_aabb(const std::initializer_list<T>& point) const
{

	if (point.size() != D)
		throw("AABB: invalid point size");

	auto it = point.begin();
	auto lbit = _lowerBound.begin();
	auto ubit = _upperBound.begin();

	for(; it!=point.end(); it++,lbit++,ubit++)
	{
		if (*it < *lbit || *it > *ubit)
			return false;
	}

	return true;
}

template<typename T, uint D>
void AABB<T,D>::setBounds(const vector& lb, const vector& ub)
{
	for (uint i=0;i<D;++i)
	{
		if (lb(i)>ub(i))
			throw std::invalid_argument("AABB: upper limit must be > lower limit");
	}
	_lowerBound = lb;
	_upperBound = ub;
}

template<typename T, uint D>
typename AABB<T,D>::vector AABB<T,D>::getCenter() const
{
	vector center((_lowerBound + _upperBound)*0.5);
	return center;
}

template<typename T, uint D>
typename AABB<T,D>::vector AABB<T,D>::getExtents() const
{
	vector dim(_upperBound - _lowerBound);
	return dim;
}

template<typename T, uint D>
void AABB<T,D>::printSelf(std::ostream& os) const
{
  os<<"AABB --> "<<"lower bound: "<<_lowerBound<<" , upper bound: "<<_upperBound;
  return;
}

} // namespace Geometry

} // namespace SX

#endif /*NSXTOOL_AABB_H_*/
