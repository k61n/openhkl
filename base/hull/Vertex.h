//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      base/hull/Vertex.h
//! @brief     Defines struct Vertex
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_HULL_VERTEX_H
#define CORE_HULL_VERTEX_H

#include <Eigen/Dense>

namespace nsx {

struct Edge;

/* !
 * Class Vertex.
 * This class implements the Vertex object used in the incremental convex hull
 * algorithm.
 */
struct Vertex {
    Vertex();
    Vertex(int id);
    Vertex(const Vertex& other) = delete;

    //! Constructs a Vertex object from a vector of coordinates
    Vertex(int id, const Eigen::Vector3d& coords);
    ~Vertex() = default;
    Vertex& operator=(const Vertex& other) = delete;

    //! Send some informations about this Vertex on an output stream
    void print(std::ostream& os) const;

    //! The coordinates of this Vertex
    Eigen::Vector3d _coords;

    //! A pointer to the incident cone edge (or nullptr)
    Edge* _duplicate;

    //! True if this Vertex is on the hull
    bool _onHull;

    //! True if the vertex has been processed
    bool _mark;

    int _id;
};

std::ostream& operator<<(std::ostream& os, const Vertex& vertex);

} // namespace nsx

#endif // CORE_HULL_VERTEX_H
