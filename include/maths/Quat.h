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
#ifndef  NSXTOOL_QUAT_H_
#define NSXTOOL_QUAT_H_
#include <iostream>
#include "Matrix33.h"

namespace SX
{
    //Forward declarations
	namespace Geometry{
    class V3D;
	}
    /** @class Quat Quat.h maths/Quat.h
    @brief Class for quaternions
    @version 1.0
    @author Laurent C Chapon, ISIS RAL
    @date 10/10/2007

    Templated class for quaternions.
    Quaternions are the 3D generalization of complex numbers
    Quaternions are used for roations in 3D spaces and
    often implemented for computer graphics applications.
    Quaternion can be written q=W+ai+bj+ck where
    w is the scalar part, and a, b, c the 3 imaginary parts.
    Quaternion multiplication is non-commutative.<br>
    i*j=-j*i=k<br>
    j*k=-k*j=i<br>
    k*i=-i*k=j<br>
    Rotation of an angle theta around a normalized axis (u,v,w) can be simply
    written W=cos(theta/2), a=u*sin(theta/2), b=v*sin(theta/2), c=w*sin(theta/2)
    This class support all arithmetic operations for quaternions
    */
    class Quat
    {

    public:
    	//! Unit quaternion
      Quat();
      //! Quaternion from the real and 3 imaginary values
      Quat(const double, const double, const double, const double);
      //! Quaternion between two vectors
      Quat(const V3D& vec1,const V3D& vec2);
      //! Copy constructor
      Quat(const Quat&);
      //! Assignement constructor
      Quat& operator=(const Quat&);
      //! Set quaternion form an angle in radians and an axis
      Quat(const double _rad, const V3D& _axis);
      ~Quat();
      void operator()(const Quat&);
      void operator()(const double ww, const double aa, const double bb, const double cc);
      void operator()(const double angle, const V3D&);
      // Set quaternion from a 3x3 matrix
      //void operator()(const M33&);
      void set(const double ww, const double aa, const double bb, const double cc);
      //! Set quaterion from an angle and an axis.
      void setAngleAxis(const double _rad, const V3D& _axis);
      void getAngleAxis(double& _rad,double& _axis1,double& _axis2,double& axis3) const;
      /// Set the rotation (both don't change rotation axis)
      void setRotation(double rad);
      //! Norm of a quaternion
      double len() const;
      //! Norm squared
      double len2() const;
      //! Re-initialize to identity
      void init();
      //! Normalize
      void normalize();
      //! Take the complex conjugate
      void conjugate();
      //! Inverse a quaternion (in the sense of rotation inversion)
      void inverse();
      //! Is the quaternion representing a null rotation
      bool isNull(const double tolerance=0.001) const;
      //! Convert quaternion rotation to an OpenGL matrix [4x4] matrix
      //! stored as an linear array of 16 double
      //! The function glRotated must be called
      void GLMatrix(double* glmat) const;
      Matrix33<double> toMatrix() const;
      //! Convert GL Matrix into Quat
      void setQuat(double[16]);
      //! Rotate a vector
      void rotate(V3D&) const;
      //! Overload operators
      Quat  operator+(const Quat&) const;
      Quat& operator+=(const Quat&);
      Quat  operator-(const Quat&) const;
      Quat& operator-=(const Quat&);
      Quat  operator*(const Quat&) const;
      Quat& operator*=(const Quat&);
      bool   operator==(const Quat&) const;
      bool   operator!=(const Quat&) const;
      const double& operator[](int) const;
      double& operator[](int);
      /** @name Element access. */
      //@{
      /// Access the real part
      inline double real() const { return w; }
      /// Access the coefficient of i
      inline double imagI() const { return a; }
      /// Access the coefficient of j
      inline double imagJ() const { return b; }
      /// Access the coefficient of k
      inline double imagK() const { return c; }
      //@}
      void printSelf(std::ostream&) const;
      void readSelf(std::istream&);
    private:
      /// Internal value
      double w;
      /// Internal value
      double a;
      /// Internal value
      double b;
      /// Internal value
      double c;
    };

    std::ostream& operator<<(std::ostream&, const Quat&);
    std::istream& operator>>(std::istream&,Quat& q);

} // namespace SX

#endif /*NSXTOOL_QUAT_H_*/
