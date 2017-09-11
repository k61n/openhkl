/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2017- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
          Jonathan Fisher, Forschungszentrum Juelich GmbH
    BP 156
    6, rue Jules Horowitz
    38042 Grenoble Cedex 9
    France
    chapon[at]ill.fr
    pellegrini[at]ill.fr
    j.fisher[at]fz-juelich.de

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

#include <Eigen/Core>
#include <vector>

#include "ConvexHull.h"

namespace nsx {

//! Class to calculate Brillouin zone of a lattice and convert to convex hull
class BrillouinZone {
    //! List of lattice vectors defining the Bragg planes of the Brillouin zone
    std::vector<Eigen::RowVector3d> _qs;  
    //! Parameter to control numerical stability
    double _eps;
    //! The basis matrix (row vectors)
    Eigen::Matrix3d _B;
    //! List of vertices generating the Brillouin zone as a convex hull
    std::vector<Eigen::RowVector3d> _vertices;
    //! squared radius of the smallest sphere containing the zone
    double _r2;

    //! Calculate bounding planes of the Brillouin zone
    void compute();
    //! Remove extraneous bounding planes
    void clean_qs();
    //! Compute the bounding vertices
    void compute_vertices();


public:
    BrillouinZone(const Eigen::Matrix3d& B, double eps=1e-6);
    //! Check if a reciprocal vector is inside the Brillouin zone
    bool inside(const Eigen::RowVector3d& q) const;
    //! Return the vertices of the polytope
    const std::vector<Eigen::RowVector3d>& vertices() const;
    //! Return the list of plane normals (q values for Bragg planes)
    const std::vector<Eigen::RowVector3d>& normals() const;
    //! Return a convex hull representation
    ConvexHull convexHull() const;

    double innerRadius() const;
    double outerRadius() const;


};

} // end namespace nsx
