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

#ifndef SX_GONIO_H_
#define SX_GONIO_H_
#include <string>
#include <cassert>
#include "RotAxis.h"
#include "Matrix33.h"
#include <vector>
#include <initializer_list>
namespace SX {
/* !
 * \brief Class defining a goniometer.
 *
 *  Allows to define a rotation axis in 3D space , constructed by a direction vector
 *  and a rotation direction (CW or CCW). The direction vector needs not to be normalized.
 *
 */


namespace Rotation
{
	enum Convention3Axis {EulerZXZ, EulerXYX, EulerYZY, EulerZYZ, EulerXZX, EulerYXY, BusingLevy};
}

template <unsigned int _N> class Gonio {
public:
	//! Labels of the axis, (e.g omega, chi, phi) are parsed as an initializer_list (C++11) {"omega","chi","phi"}
	Gonio(std::initializer_list<const char*> labels);
	//! Destructor
	~Gonio();
	//!
	template<unsigned int _I> const std::string& AxisLabel() const;
	//! Define axis number _I with a rotation axis
	template<unsigned int _I> void setAxis(const RotAxis&);
	//! Accesor to Axis number _I
	template<unsigned int _I> RotAxis& Axis();
	//! Const-accessor to Axis _I
	template<unsigned int _I> const RotAxis& Axis() const;
	//!
	Matrix33<double> setAngles(std::initializer_list<double> angles);
	template<unsigned int _U>
	friend std::ostream& operator<<(std::ostream& os,Gonio<_U>&);
private:
	unsigned int _naxes;
	std::vector<std::string> _labels;
	RotAxis _axes[_N];
};


template <unsigned int _N>
Gonio<_N>::Gonio(std::initializer_list<const char*> labels):_naxes(_N),_labels(labels.begin(),labels.end())
{
}
template <unsigned int _N>
Gonio<_N>::~Gonio()
{
}

template <unsigned int _N>
template <unsigned int _I>
const std::string& Gonio<_N>::AxisLabel() const
{
	return _labels[_I];
}

template<unsigned int _N>
template <unsigned int _I>
void Gonio<_N>::setAxis(const RotAxis& rot)
{
	assert(_I<_N);
	_axes[_I]=rot;
}


template<unsigned int _N>
template <unsigned int _I>
RotAxis& Gonio<_N>::Axis()
{
	assert(_I<_N);
	return _axes[_I];
}

template<unsigned int _N>
template <unsigned int _I>
const RotAxis& Gonio<_N>::Axis() const
{
	assert(_I<_N);
	return _axes[_I];
}

template<unsigned int _N>
Matrix33<double> Gonio<_N>::setAngles(std::initializer_list<double> l)
{
	if (l.size()!=_naxes)
		throw std::runtime_error("Number of arguments angles different from the number of axes on the goniometer");
	// Get rotation matrix of the right most angle
	auto it=l.begin();

	int i=0;
	Matrix33<double> result=_axes[i++].getMatrix(*it);
	it++;

	for(;it!=l.end();++it)
	{
		result=result*_axes[i++].getMatrix(*it);
	}
	return result;
}
template<unsigned int _N>
std::ostream& operator<<(std::ostream& os,Gonio<_N>& g)
{
	os << "Goniometer with rotation \n";
	for (int i=0;i<g._naxes;i++)
	{
		os << g._labels[i] << g._axes[i] << std::endl;
	}
	return os;
}


} // End namespace SX
#endif /* SX_GONIO_H_ */
