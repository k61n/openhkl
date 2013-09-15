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

namespace SX
{

namespace Geometry
{

template <typename T> class V2D
    {
    public:
      V2D();
      V2D(const V2D&);
      V2D& operator=(const V2D&);
      V2D(const double,const double);
      V2D(const double*);
      ~V2D();
      // Arithmetic operators overloaded
      V2D operator+(const V2D& v) const;
      V2D& operator+=(const V2D& v);
      void add(double x,double y,double z)
		{
    	  v[0]+=x;
    	  v[1]+=y;
    	  v[2]+=z;
		}
      V2D operator-(const V2D& v) const;
      V2D& operator-=(const V2D& v);
      // Inner product
      V2D operator*(const V2D& v) const;
      V2D& operator*=(const V2D& v);
      // Inner division
      V2D operator/(const V2D& v) const;
      V2D& operator/=(const V2D& v);
      // Scale
      V2D operator*(const double D) const;
      V2D& operator*=(const double D);
      V2D operator/(const double D) const;
      V2D& operator/=(const double D);
      // Simple Comparison
      bool operator==(const V2D&) const;
      bool operator<(const V2D&) const;
      // Access
      // Setting v[0], v[1] and v[2] values
      void operator()(const double, const double, const double);
      void operator()(const V2D& v);
      void spherical(double R, double theta, double phi);
      void addtimes(const V2D& _v,double factor)
      {
    	  v[0]+=_v.v[0]*factor;
    	  v[1]+=_v.v[1]*factor;
    	  v[2]+=_v.v[2]*factor;
    	  return ;
      }
      const double& x() const { return v[0]; } ///< Get v[0]
      const double& y() const { return v[1]; } ///< Get v[1]
      const double& z() const { return v[2]; } ///< Get v[2]

      const double& operator[](const int Index) const;
      double& operator[](const int Index);
      /// Make a normalized vector (return norm value)
      double normalize();            // Vec3D::makeUnit
      double norm() const;
      double norm2() const;
      // Scalar product
      inline double scalar_prod(double x, double y, double z) const { return (v[0]*x+v[1]*y+v[2]*z);}
      inline double scalar_prod(const V2D& _v) const { return (v[0]*_v.v[0]+v[1]*_v.v[1]+v[2]*_v.v[2]);}
      // Cross product
      V2D cross_prod(const V2D&) const;
      // Distance between two points defined as vectors
      double distance(const V2D&) const;
      // Send to a stream
      void printSelf(std::ostream&) const;
      void read(std::istream&);

      int nullVector(const double=1e-3) const;              ///< Determine if the point is null
      int coLinear(const V2D&,const V2D&) const;

    private:

      double v[3];       ///< v value [unitless]
    };

    // Overload operator <<
   std::ostream& operator<<(std::ostream&, const V2D&);
   std::istream& operator>>(std::istream&,V2D&);
   int colinear(const V2D& v1, const V2D& v2);

   // Unit vectors
   static const V2D UnitX=V2D(1,0,0);
   static const V2D UnitY=V2D(0,1,0);
   static const V2D UnitZ=V2D(0,0,1);

} // Namespace Geometry
} // namespace SX

#endif /*NXSTOOL_V2D_H_*/
