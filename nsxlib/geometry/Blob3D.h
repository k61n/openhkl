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

#pragma once

#include <iostream>

#include <Eigen/Dense>

namespace nsx {

/**
 * \class Blob3D
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
class Blob3D {
public:
  
  //! Initialize an empty blob
  Blob3D();

  //! Initialize a blob with a point of mass m at x,y,z
  Blob3D(double x, double y,double z, double m);

  //! Copy constructor
  Blob3D(const Blob3D&);
  Blob3D(Blob3D&&) = default;

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
  Eigen::Vector3d getCenterOfMass() const;

  //! Get the ellipsoid parameters
  void toEllipsoid(double confidence, Eigen::Vector3d& center,
                   Eigen::Vector3d& eigenvalues,Eigen::Matrix3d& eigenvectors) const;

  //! Print
  void printSelf(std::ostream& os) const;
 
  //! Get covariance matrix of the blob
  Eigen::Matrix3d covariance() const;

private:
  //! Zeroth moment (total mass)
  double _m0;

  //! First moments
  Eigen::Vector3d _m1;

  //! Second moments  
  Eigen::Matrix3d _m2;

  //! Number of points contributing
  unsigned int _npoints;

  //! Minimum mass value
  double _minValue;
  
  //! Maximum mass value
   double _maxValue;
};

#ifndef SWIG
std::ostream& operator<<(std::ostream& os, const Blob3D& b);
#endif
} // end namespace nsx
