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

#ifndef NSXLIB_CLUSTER_H
#define NSXLIB_CLUSTER_H

#include <Eigen/Dense>

namespace nsx {

//! Class to group peaks in clusters in 3Dimensions.
class Cluster {
public:
	//! Create a cluster with a tolerance on the norm
	//! Create a cluster from a fist vector with a tolerance on the norm
  Cluster(const Eigen::Vector3d& v, double tol);
  Cluster(const Cluster&);
  Cluster& operator=(const Cluster&);

  //! Get the center of a cluster
  Eigen::Vector3d getCenter() const;
  //! Get the number of vectors in the Cluster
  int getSize() const { return _size;}
  //! Add a vector
  bool addVector(const Eigen::Vector3d& v);
  //! Check whether two clusters are equivalent within their norms
  bool operator==(const Cluster& c) const;
  //! Merge two clusters
  Cluster& operator+=(const Cluster& c);
private:
  //! center of the
  Eigen::Vector3d _center;
  int _size;
  double _tolerance;

};

} // end namespace nsx

#endif // NSXLIB_CLUSTER_H

 
