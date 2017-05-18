/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon Eric Pellegrini
 Institut Laue-Langevin
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

#ifndef NSXLIB_LATTICEFINDER_H
#define NSXLIB_LATTICEFINDER_H

#include <vector>
#include <map>
#include <Eigen/Dense>
#include "../geometry/Cluster.h"
#include <tuple>

namespace nsx {

typedef std::tuple<Eigen::Vector3d,Eigen::Vector3d,Eigen::Vector3d,double> LatticeVectors;

class LatticeFinder {

public:

    LatticeFinder(double threshold, double tolerance);

    void addPoint(double x, double y, double z);

    void addPoint(const Eigen::Vector3d& v);

    void addPoints(const std::vector<Eigen::Vector3d>&);

    void run(double cellmin=2.0);

    std::vector<LatticeVectors> determineLattice(std::size_t clustermax=20,int numberofsolutions=50) const;

    int getNumberOfClusters() { return _clusters.size();}

    const std::multimap<double,Cluster>& getClusters() const { return _clusters;}

    double costFunction(const Eigen::Vector3d& v1, const Eigen::Vector3d& v2, const Eigen::Vector3d& v3, double epsilon,  double delta) const;

private:

    double _threshold;
    double _tolerance;
    std::vector<Eigen::Vector3d> _peaks;

public:

    std::multimap<double,Cluster> _clusters;

};

} // end namespace nsx

#endif // NSXLIB_LATTICEFINDER_H
