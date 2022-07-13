//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      tables/crystal/BrillouinZone.h
//! @brief     Defines class BrillouinZone
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_TABLES_CRYSTAL_BRILLOUINZONE_H
#define NSX_TABLES_CRYSTAL_BRILLOUINZONE_H

#include "base/geometry/ConvexHull.h"

namespace ohkl {

//! Calculates first Brillouin zone of a lattice, and converts it to a ConvexHull representation.

//! The first Brillouin zone of a lattice is
//! defined to be the set of all vectors which are closer to the origin than to
//! any other lattice point. From this definition, it comes that it is a convex
//! hull bounded by Bragg planes, i.e. planes with normal vector q passing
//! through the point q/2 for lattice vectors q. Hence, to construct the
//! Brillouin zone, one has to enumerate the finitely many q's which appear as
//! normals to the faces of the zone. Then, the bouding vertices are determined
//! by taking 3-fold intersections of the bounding planes.

class BrillouinZone {
 public:
    //! Create a Brillouin zone out of the given (row) basis matrix B.
    BrillouinZone(const Eigen::Matrix3d& B, double eps = 1e-6);
    //! Check if a reciprocal vector is inside the Brillouin zone.
    bool inside(const Eigen::RowVector3d& q) const;
    //! Returns the vertices of the polytope.
    const std::vector<Eigen::RowVector3d>& vertices() const;
    //! Returns the list of plane normals (q values for Bragg planes).
    const std::vector<Eigen::RowVector3d>& normals() const;
    //! Returns a convex hull representation.
    ConvexHull convexHull() const;

    //! Returns radius of largest sphere contained in the zone.
    double innerRadius() const;
    //! Returns radius of smallest sphere containing the zone.
    double outerRadius() const;

 private:
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
};

} // namespace ohkl

#endif // NSX_TABLES_CRYSTAL_BRILLOUINZONE_H
