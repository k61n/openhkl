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
#ifndef NSXTOOL_Blob2D_H_
#define NSXTOOL_Blob2D_H_

#include <iostream>
#include "V2D.h"

namespace SX
{

namespace Geometry
{

/* !
 * \brief Class Blob2D.
 * Blobs are used to store region of interest in a 2D image.
 * A Blob is constructed by adding points in the image with coordinates x,y
 * and an associated mass that represents any scalar field such as intensity.
 * Blob objects in NSXTool records the total mass, the mass-weighted first and second moments
 * as new points are added to the blob. Knowledge about individual points is lost,
 * i.e Blob can only increase in size.
 * Blobs can be merged and maintain zero, first and second momentum.
 *
 */
class Blob2D
{
public:
  //!Initialize an empty blob
  Blob2D();
  //!Initialize a blob with a point of mass m at x,y
  Blob2D(double x, double y, double m);
  //! Copy constructor
  Blob2D(const Blob2D&);
  //! Assignment
  Blob2D& operator=(const Blob2D&);
  //! Add point to the Blob
  void addPoint(double x, double y, double m);
  //! Merge a second blob
  void merge(const Blob2D&);
  //! Return the total mass
  double getMass() const;
  //! Return the number of points
  int getComponents() const;
  //! Return the center of Mass
  V2D<double> getCenterOfMass() const;
  //! Compute the contour ellipse of the Blob
  void toEllipse(double& xc, double& yc, double& s_a, double& s_b, double& angle) const;
private:
  //! Total mass=zeroth order momentum
  double _m00;
  //! First moment= Sum of all weighted positions m_i.x_i, m_i.y_i
  double _m10, _m01;
  //! Second moments
  double _m20, _m02, _m11;
  //! Number of points contributing
  int _npoints;

};

} // Namespace Geometry
} // namespace SX

#endif /*NXSTOOL_Blob2D_H_*/
