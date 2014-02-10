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
#ifndef NSXTOOL_BLOB3D_H_
#define NSXTOOL_BLOB3D_H_
#include <iostream>
#include <Eigen/Dense>

namespace SX
{

namespace Geometry
{

using Eigen::Vector3d;
/* !
 * \brief Class Blob3D.
 * Blob3D are used to store region of interest in a 3D image.
 * A Blob is constructed by adding points in the image with coordinates x,y,z
 * and an associated mass that represents any scalar field such as intensity.
 * Blob objects records the total mass, the mass-weighted first and second moments
 * as new points are added to the blob. Knowledge about individual points is lost,
 * i.e Blob can only increase in size.Blobs can be merged and maintain zero, first and second momentum.
 * Blob3D can be transformed into an Ellipsoid, by diagonalizing the variance tensor.
 *
 */
class Blob3D
{
public:
  //!Initialize an empty blob
  Blob3D();
  //!Initialize a blob with a point of mass m at x,y,z
  Blob3D(double x, double y,double z, double m);
  //! Copy constructor
  Blob3D(const Blob3D&);
  //! Assignment
  Blob3D& operator=(const Blob3D&);
  //! Add point to the Blob
  void addPoint(double x, double y, double z, double m);
  //! Merge a second blob
  void merge(const Blob3D&);
  //! Return the total mass
  double getMass() const;
  //! Return the number of points
  int getComponents() const;
  //! Return the minimumMass
  double getMinimumMass() const;
  //! Return the minimumMass
  double getMaximumMass() const;
  //! Return the center of Mass
  Vector3d getCenterOfMass() const;
  //! Get the ellipsoid parameters
  void toEllipsoid(Vector3d& center,Vector3d& semi_axes, Vector3d& v0, Vector3d& v1, Vector3d& v2) const;
  //! Print
  void printSelf(std::ostream& os) const;
  //! Compute the intersection with a plane of equation \f$ax+by+cz+d=0\f$
  bool intersectionWithPlane(double a, double b, double c, double d, Vector3d& center, Vector3d& semi_axes, Vector3d& axis1, Vector3d& axis2, double confidence) const;

private:
  //! Total mass=zeroth order momentum
  double _m000;
  //! First moment= Sum of all weighted positions m_i.x_i, m_i.y_i, m_i.z_i
  double _m100, _m010, _m001;
  //! Second moments
  double _m200, _m020, _m002, _m110, _m101, _m011;
  //! Number of points contributing
  unsigned int _npoints;
  double _minValue, _maxValue;

};

std::ostream& operator<<(std::ostream& os, const Blob3D& b);

} // Namespace Geometry
} // namespace SX

#endif /*NXSTOOL_Blob3D_H_*/
