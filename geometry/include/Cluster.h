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
#ifndef NSXTOOL_Cluster_H_
#define NSXTOOL_Cluster_H_

#include <vector>
#include <map>
#include <Eigen/Dense>

namespace SX
{

namespace Geometry
{

class Cluster
{
public:
  Cluster();
  Cluster(double tol);
  Cluster(const Eigen::Vector3d& v, double tol);
  Eigen::Vector3d getCenter() const { return _center/static_cast<double>(_size);}
  int getSize() const { return _size;}
  bool addVector(const Eigen::Vector3d& v);
  bool operator==(const Cluster& c) const;
  Cluster& operator+=(const Cluster& c);
private:
  //! center of the
  Eigen::Vector3d _center;
  int _size;
  double _tolerance;

};

class LatticeFinder
{
public:
	LatticeFinder(double threshold, double tolerance);
	void addPoint(double x, double y, double z);
	void addPoint(const Eigen::Vector3d& v);
	void addPoints(const std::vector<Eigen::Vector3d>&);
	void run(double cellmin=2.0);
	bool determineLattice(Eigen::Vector3d& a, Eigen::Vector3d& b,Eigen::Vector3d& c,int clustermax=20) const;
	int getNumberOfClusters() { return _clusters.size();}
	const std::multimap<double,Cluster>& getClusters() const { return _clusters;}
	double costFunction(const Eigen::Vector3d& v1, const Eigen::Vector3d& v2, const Eigen::Vector3d& v3, double epsilon,  double delta) const;
private:
    double _threshold;
    double _tolerance;
	std::vector<Eigen::Vector3d> _peaks;
	std::multimap<double,Cluster> _clusters;
};

} // Namespace Geometry
} // Namespace SX

#endif /*NXSTOOL_Cluster_H_*/

 
