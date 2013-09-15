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
#ifndef NSXTOOL_V2D_H_
#define NSXTOOL_V2D_H_

#include <iostream>
#include <stdexcept>
#include <cmath>

namespace SX
{

namespace Geometry
{

template <typename T> class V2D
    {
    public:
      V2D();
      V2D(const V2D<T>&);
      V2D& operator=(const V2D<T>&);
      V2D(const T,const T);
      V2D(const T*);
      ~V2D();
      // Arithmetic operators overloaded
      V2D<T> operator+(const V2D<T>& v) const;
      V2D<T>& operator+=(const V2D<T>& v);
      void add(T x,T y);
      V2D<T> operator-(const V2D<T>& v) const;
      V2D<T>& operator-=(const V2D<T>& v);
      // Inner product
      V2D<T> operator*(const V2D<T>& v) const;
      V2D<T>& operator*=(const V2D<T>& v);
      // Inner division
      V2D<T> operator/(const V2D<T>& v) const;
      V2D<T>& operator/=(const V2D<T>& v);
      // Scale
      V2D<T> operator*(const T D) const;
      V2D<T>& operator*=(const T D);
      V2D<T> operator/(const T D) const;
      V2D<T>& operator/=(const T D);
      // Simple Comparison
      bool operator==(const V2D<T>&) const;
      bool operator<(const V2D<T>&) const;
      // Access
      // Setting v[0], v[1] values
      void operator()(const T, const T);
      void operator()(const V2D<T>& v);
      void addtimes(const V2D<T>& _v,T factor);
      const T& x() const { return v[0]; } ///< Get v[0]
      const T& y() const { return v[1]; } ///< Get v[1]
      const T& operator[](const int Index) const;
      T& operator[](const int Index);
      /// Make a normalized vector (return norm value)
      double normalize();            // Vec3D::makeUnit
      double norm() const;
      double norm2() const;
      // Scalar product
      inline T scalar_prod(T x, T y) const { return (v[0]*x+v[1]*y);}
      inline T scalar_prod(const V2D<T>& _v) const { return (v[0]*_v.v[0]+v[1]*_v.v[1]);}
      // Distance between two points defined as vectors
      double distance(const V2D<T>&) const;
      //! Determine if the point is null
      int nullVector(const T=1e-3) const;
      // Send to a stream
      void printSelf(std::ostream&) const;
    private:
      static double Tolerance;
      T v[2];       ///< v value [unitless]
    };

template<typename T>
double V2D<T>::Tolerance=1e-7;

// Overload operator <<
template<typename T>
std::ostream& operator<<(std::ostream& os, const V2D<T>& v)
{
	v.printSelf(os);
	return os;
}
// Determine whether two vectors are collinear
template<typename T>
int colinear(const V2D<T>& v1, const V2D<T>& v2)
{
	T res=v1[0]*v2[1]-v1[1]*v2[0];
	return (res<1e-7) ? 1 : 0;
}
template<typename T>
V2D<T>::V2D()
{
	v[0]=0;
	v[1]=0;
	v[2]=0;
}

/// Value constructor
template<typename T>
V2D<T>::V2D(const T _x, const T _y)
{
	v[0]=_x;
	v[1]=_y;
}

/// Copv[1] constructor
template<typename T>
V2D<T>::V2D(const V2D<T>& _v)
{
	v[0]=_v.v[0];
	v[1]=_v.v[1];
}


///Assignment
template<typename T>
V2D<T>& V2D<T>::operator=(const V2D<T>& A)
{
  if (this!=&A)
    {
      v[0]=A.v[0];
      v[1]=A.v[1];
    }
  return *this;
}

/**
  Constructor from a pointer.
  requires that the point is assigned after this has
  been allocated since vPtr[v[0]] mav[1] throw.
*/
template<typename T>
V2D<T>::V2D(const T* vPtr)

{
  if (vPtr)
    {
      v[0]=vPtr[0];
      v[1]=vPtr[1];
    }
}

template<typename T>
V2D<T>::~V2D()
{}

  /**
    Addtion operator
     \param v :: Vector to add
     \return *this+v;
  */
template<typename T>
V2D<T> V2D<T>::operator+(const V2D<T>& _v) const
{
  V2D<T> out(*this);
  out+=_v;
  return out;
}

  /**
    Subtraction operator
    \param v :: Vector to sub.
    \return *this-v;
  */
template<typename T>
V2D<T> V2D<T>::operator-(const V2D<T>& _v) const
{
  V2D<T> out(*this);
  out-=_v;
  return out;
}

  /**
    Inner product
    \param v :: Vector to sub.
    \return *this * v;
  */
template<typename T>
V2D<T> V2D<T>::operator*(const V2D<T>& _v) const
{
  V2D<T> out(*this);
  out*=_v;
  return out;
}

  /**
    Inner division
    \param v :: Vector to divide
    \return *this * v;
  */
template<typename T>
V2D<T> V2D<T>::operator/(const V2D<T>& _v) const
{
  V2D<T> out(*this);
  out/=_v;
  return out;
}

  /**
    Self-Addition operator
    \param v :: Vector to add.
    \return *this+=v;
  */
template<typename T>
V2D<T>& V2D<T>::operator+=(const V2D<T>& _v)
{
  v[0]+=_v.v[0];
  v[1]+=_v.v[1];
  return *this;
}

template<typename T>
void V2D<T>::add(T x,T y)
{
	  v[0]+=x;
	  v[1]+=y;
}

  /**
    Self-Subtraction operator
    \param v :: Vector to sub.
    \return *this-v;
  */
template<typename T>
V2D<T>& V2D<T>::operator-=(const V2D<T>& _v)
{
  v[0]-=_v.v[0];
  v[1]-=_v.v[1];
  return *this;
}

template<typename T>
V2D<T>& V2D<T>::operator*=(const V2D<T>& _v)
{
  v[0]*=_v.v[0];
  v[1]*=_v.v[1];
  return *this;
}
template<typename T>
V2D<T>& V2D<T>::operator/=(const V2D<T>& _v)
{
  v[0]/=_v.v[0];
  v[1]/=_v.v[1];
  return *this;
}

template<typename T>
V2D<T> V2D<T>::operator*(const T D) const
{
  V2D<T> out(*this);
  out*=D;
  return out;
}

template<typename T>
V2D<T> V2D<T>::operator/(const T D) const
{
  V2D<T> out(*this);
  out/=D;
  return out;
}

template<typename T>
V2D<T>& V2D<T>::operator*=(const T D)
{
  v[0]*=D;
  v[1]*=D;
  return *this;
}

template<typename T>
V2D<T>& V2D<T>::operator/=(const T D)
{
  if (D!=0.0)
    {
      v[0]/=D;
      v[1]/=D;
    }
  return *this;
}
template<typename T>
void V2D<T>::addtimes(const V2D<T>& _v,T factor)
{
	v[0]+=_v.v[0]*factor;
	v[1]+=_v.v[1]*factor;
	return ;
}

template<typename T>
bool V2D<T>::operator==(const V2D<T>& v) const
{
  return (fabs(v[0]-v.v[0])>Tolerance ||
	  fabs(v[1]-v.v[1])>Tolerance )  ?
    false : true;
}

template<typename T>
bool V2D<T>::operator<(const V2D<T>& V) const
{
  if (v[0]!=V.v[0])
    return v[0]<V.v[0];
  return v[1]<V.v[1];

}

template<typename T>
void V2D<T>::operator()(const T _x, const T _y)
{
  v[0]=_x;
  v[1]=_y;
  return;
}

template<typename T>
void V2D<T>::operator()(const V2D<T>& _v)
{
  v[0]=_v.v[0];
  v[1]=_v.v[1];
  return;
}

template<typename T>
const T& V2D<T>::operator[](const int Index) const
{
  switch (Index)
    {
    case 0: return v[0];
    case 1: return v[1];
    default:
      throw std::runtime_error("V2D::operator[] range error");
    }
}

template<typename T>
T& V2D<T>::operator[](const int Index)
{
  switch (Index)
    {
    case 0: return v[0];
    case 1: return v[1];
    default:
      throw std::runtime_error("V2D::operator[] range error");
    }
}

template<typename T>
double V2D<T>::norm() const
{
   return sqrt(static_cast<double>(v[0]*v[0]+v[1]*v[1]));
}

template<typename T>
double V2D<T>::norm2() const
{
	return static_cast<double>(v[0]*v[0]+v[1]*v[1]);
}

template<typename T>
double V2D<T>::normalize()
{
  const double ND(norm());
  this->operator/=(ND);
  return ND;
}

template<typename T>
double V2D<T>::distance(const V2D<T>& _v) const
{
  V2D<T> dif(*this);
  dif-=_v;
  return dif.norm();
}
  /**
    Prints a tev[0]t representation of itself
    \param os the Stream to output to
  */
template<typename T>
void V2D<T>::printSelf(std::ostream& os) const
{
  os  <<"[" << v[0] <<"," << v[1]  << "]";
  return;
}

} // Namespace Geometry
} // namespace SX

#endif /*NXSTOOL_V2D_H_*/
