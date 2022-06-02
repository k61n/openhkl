//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/geometry/ConvexHull.h
//! @brief     Defines class ConvexHull
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_BASE_HULL_H
#define NSX_BASE_HULL_H

#include <Eigen/Core>
#include <vector>

#include "base/geometry/AABB.h"


namespace nsx {

class ConvexHull {
 public:
    ConvexHull() = default;

    ConvexHull(const ConvexHull&);
    ConvexHull& operator=(const ConvexHull&);

    //! Reset, eliminate all vertices, edges and faces
    void reset();

    //! Adds a new vertex to the list of points to be processed later when calling
    //! updateHull method.
    bool addVertex(const Eigen::Vector3d& coords, double tolerance = 1e-6);

    //! Remove vertex
    bool removeVertex(const Eigen::Vector3d& coords, double tolerance = 1e-6);

    //! Updates the hull.
    bool updateHull(double tolerance = 1e-6);

    //! Returns whether a vertex is contained in the hull
    bool contains(const Eigen::Vector3d& v) const;

    const AABB& aabb() const { return _aabb; };
    const std::vector<Eigen::Vector3d>& vertices() const { return _vertices; }
    const std::vector<std::vector<Eigen::Vector3d>>& faces() const { return _faces; }
    const std::vector<Eigen::Vector3d>& normals() const { return _normals; }
    const std::vector<double>& distances() const { return _dists; }
    const Eigen::Vector3d& center() const { return _center; }
    double volume() const { return _volume; }

 private:
    // hull vertices
    std::vector<Eigen::Vector3d> _vertices;

    // hull faces
    std::vector<std::vector<Eigen::Vector3d>> _faces;

    // hull plane normals and distances
    std::vector<Eigen::Vector3d> _normals;
    std::vector<double> _dists;

    // center of hull polyhedron
    Eigen::Vector3d _center;

    double _volume = 0.;

    // hull bounding radius
    double _outerR2 = -1.;

    AABB _aabb;
};

}
#endif
