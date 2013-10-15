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
#ifndef NSXTOOL_MATRIX33_H_
#define NSXTOOL_MATRIX33_H_
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <utility>
#include "V3D.h"

using SX::Geometry::V3D;

namespace SX
{
/* @Brief: Template class for 3x3 matrices.
 */
template<class T>
class Matrix33
{
  public:
  //! Null matrix
  Matrix33();    
  //! Set matrix with elements in rows
  Matrix33(T,T,T,T,T,T,T,T,T);        
  //! Copy
  Matrix33(const Matrix33<T>&);
  //! Assignment
  Matrix33& operator=(const Matrix33<T>&);
  //
  ~Matrix33();
  //! Set all matrix elements with values given in rows
  void set( T,T,T,T,T,T,T,T,T);
  //! Nullify the matrix
  void null();           
  //! Set the matrix to identity
  void identity();                 
  //! Return the determinant
  T determinant() const; 
  //! Access the element i,j -const
  const T& operator()(int i, int j) const;
  //! Access the element i,j -non const
  T& operator()(int i, int j);
  //! Transpose the matrix
  void transpose_inplace();
  // Return a copy of the transposed matrix
  Matrix33<T> transpose() const;
  //! Cofactor matrix
  void cofactor();
  //! Return the adjoint
  void adjoint();
  //! Return the trace
  T trace() const;
  //! Invert the matrix
  void invert();                                                                   
  Matrix33<T> operator*(const Matrix33<T>&) const;                              
  Matrix33<T>& operator*=(const Matrix33<T>&);
  void printSelf(std::ostream&) const;
  //! Right multiply by a vetor
  V3D operator*(const V3D&) const;
  //! Left multiply by a vector
  template <typename U> friend V3D operator*(const V3D&, const Matrix33<U>& m);
  std::pair<V3D,V3D> operator*(const std::pair<V3D,V3D>&);
  void transformV(V3D&) const;
  //! Does this matrix represents a rotation
  bool isRotation() const {return (std::fabs(determinant()-1)<1e-6);}
  private:
  T el[3][3];
};
template<class T>
std::ostream& operator<<(std::ostream& os,const Matrix33<T>& m)
{
	m.printSelf(os);
	return os;
}
template<class T>
Matrix33<T>::Matrix33()
{
  null();
}
template<class T>
Matrix33<T>::Matrix33(T a00, T a01, T a02, T a10, T a11, T a12, T a20, T a21, T a22)
{
	el[0][0]=a00;el[0][1]=a01;el[0][2]=a02;
	el[1][0]=a10;el[1][1]=a11;el[1][2]=a12;
	el[2][0]=a20;el[2][1]=a21;el[2][2]=a22;
}
template<class T>
Matrix33<T>::Matrix33(const Matrix33<T>& m)
{
	for (int i=0;i<3;i++)
	{
		for (int j=0;j<3;j++)
		{
			this->el[i][j]=m.el[i][j];
		}
	}
	return;
}
template<class T>
Matrix33<T>& Matrix33<T>::operator=(const Matrix33<T>& m)
{
	if (this!=&m)
	{
		for (int i=0;i<3;i++)
		{
			for (int j=0;j<3;j++)
			{
				this->el[i][j]=m.el[i][j];
			}
		}
	}
	return *this;
}
template<class T>
void Matrix33<T>::set(T a00,T a01,T a02,
		                              T a10,T a11,T a12,
		                              T a20,T a21,T a22)
{
	el[0][0]=a00;el[0][1]=a01;el[0][2]=a02;
	el[1][0]=a10;el[1][1]=a11;el[1][2]=a12;
	el[2][0]=a20;el[2][1]=a21;el[2][2]=a22;
}
template<class T>
Matrix33<T>::~Matrix33()
{
}
template<class T>
void Matrix33<T>::null()
{
  for (int i=0;i<3;i++)
  {
		for (int j=0;j<3;j++)
		el[i][j]=0;
	}
}
template<class T>
void Matrix33<T>::identity()
{
	el[0][0]=el[1][1]=el[2][2]=1;
	el[0][1]=el[0][2]=el[1][0]=
	el[1][2]=el[2][0]=el[2][1]=0;
}
template<class T>
T Matrix33<T>::determinant() const
{
	return (el[0][0]*el[1][1]*el[2][2]+el[0][1]*el[1][2]*el[2][0]+el[0][2]*el[1][0]*el[2][1]-
	        el[2][0]*el[1][1]*el[0][2]-el[1][0]*el[0][1]*el[2][2]-el[0][0]*el[2][1]*el[1][2]);
}
template<class T>
void Matrix33<T>::transpose_inplace()
{
	T a01,a02,a10,a12,a20,a21;
	a01=el[1][0];a02=el[2][0];
	a10=el[0][1];a12=el[2][1];
	a20=el[0][2];a21=el[1][2];
	el[0][1]=a01;el[0][2]=a02;
	el[1][0]=a10;el[1][2]=a12;
	el[2][0]=a20;el[2][1]=a21;
	return;
}
template<class T>
Matrix33<T> Matrix33<T>::transpose() const
{
	Matrix33<T> out = *this;
	out.transpose_inplace();
	return out;
}
template<class T>
void Matrix33<T>::cofactor()
{
	T a00,a01,a02,a10,a11,a12,a20,a21,a22;
	a00=el[1][1]*el[2][2]-el[2][1]*el[1][2];
	a01=el[2][0]*el[1][2]-el[1][0]*el[2][2];
	a02=el[1][0]*el[2][1]-el[2][0]*el[1][1];
	a10=el[2][1]*el[0][2]-el[0][1]*el[2][2];
	a11=el[0][0]*el[2][2]-el[2][0]*el[0][2];
	a12=el[2][0]*el[0][1]-el[0][0]*el[2][1];
	a20=el[0][1]*el[1][2]-el[1][1]*el[0][2];
	a21=el[1][0]*el[0][2]-el[0][0]*el[1][2];
	a22=el[0][0]*el[1][1]-el[1][0]*el[0][1];
	
	el[0][0]=a00;el[0][1]=a01;el[0][2]=a02;
	el[1][0]=a10;el[1][1]=a11;el[1][2]=a12;
	el[2][0]=a20;el[2][1]=a21;el[2][2]=a22;
	return;
}
template<class T>
void Matrix33<T>::adjoint()
{
	cofactor();
	transpose_inplace();
	return;
}
template<class T>
T Matrix33<T>::trace() const
{
	return (el[0][0]+el[1][1]+el[2][2]);
}
template<class T>
void Matrix33<T>::invert()
{
	T det;
	det=determinant();
	if (det!=0)
	{
		adjoint();
		for(int i=0;i<3;i++)
		{
			for (int j=0;j<3;j++)
			el[i][j]/=det;
		}
	}
}
template<class T>
Matrix33<T> Matrix33<T>::operator*(const Matrix33<T>& myM) const
{
	Matrix33<T> out;
	for (int i=0;i<3;i++)
	{
		for (int j=0;j<3;j++)
		out.el[i][j]=el[i][0]*myM.el[0][j]+el[i][1]*myM.el[1][j]+el[i][2]*myM.el[2][j];
	}
	return out;
}
template<class T>
Matrix33<T>& Matrix33<T>::operator*=(const Matrix33<T>& myM)
{
	for (int i=0;i<3;i++)
	{
		for (int j=0;j<3;j++)
		this->el[i][j]=el[i][0]*myM.el[0][j]+el[i][1]*myM.el[1][j]+el[i][2]*myM.el[2][j];
	}
	return *this;
}

template<class T>
const T& Matrix33<T>::operator()(int i, int j) const
{
	if (i<0 || i>2 || j<0 || j>2)
		throw std::range_error(" Matrix33<T>::operator()(int i, int j)");
	else
		return el[i][j];
}

template<class T>
T& Matrix33<T>::operator()(int i, int j) 
{
	if (i<0 || i>2 || j<0 || j>2)
			throw std::range_error(" Matrix33<T>::operator()(int i, int j)");
	else
		return el[i][j];
}
template<class T>
void Matrix33<T>::printSelf(std::ostream& os) const
{
	os << "[" << el[0][0] << "," << el[0][1] << "," << el[0][2] << std::endl;
	os << el[1][0] << "," << el[1][1] << "," << el[1][2] << std::endl;
	os << el[2][0] << "," << el[2][1] << "," << el[2][2] << "]";
}

template<class T>
V3D Matrix33<T>::operator *(const V3D& vect) const
{
	double xx=el[0][0]*vect[0]+el[0][1]*vect[1]+el[0][2]*vect[2];
	double yy=el[1][0]*vect[0]+el[1][1]*vect[1]+el[1][2]*vect[2];
	double zz=el[2][0]*vect[0]+el[2][1]*vect[1]+el[2][2]*vect[2];
	
	return V3D(xx,yy,zz);
}

template<typename U>
V3D operator *(const V3D& vect,const Matrix33<U>& m)
{
	double xx=m.el[0][0]*vect[0]+m.el[1][0]*vect[1]+m.el[2][0]*vect[2];
	double yy=m.el[0][1]*vect[0]+m.el[1][1]*vect[1]+m.el[2][1]*vect[2];
	double zz=m.el[0][2]*vect[0]+m.el[1][2]*vect[1]+m.el[2][2]*vect[2];

	return V3D(xx,yy,zz);
}

template<class T>
void Matrix33<T>::transformV(V3D& vect) const
{
	double xx=el[0][0]*vect[0]+el[0][1]*vect[1]+el[0][2]*vect[2];
	double yy=el[1][0]*vect[0]+el[1][1]*vect[1]+el[1][2]*vect[2];
	double zz=el[2][0]*vect[0]+el[2][1]*vect[1]+el[2][2]*vect[2];
	vect(xx,yy,zz);
}
template<class T>
std::pair<V3D,V3D> Matrix33<T>::operator*(const std::pair<V3D,V3D>& tin)
{
	V3D v0=this->operator*(tin.first);
	V3D v1=this->operator*(tin.second);
	return std::pair<V3D,V3D>(v0,v1);
}


}// namespace SX

#endif // NSXTOOL_MATRIX33_H_
