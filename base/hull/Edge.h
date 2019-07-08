//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      base/hull/Edge.h
//! @brief     Defines struct Edge
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef BASE_HULL_EDGE_H
#define BASE_HULL_EDGE_H

#include <ostream>
#include <vector>

namespace nsx {

struct Face;
struct Vertex;

//! An edge, used in the incremental convex hull algorithm.

struct Edge {
    Edge();

    Edge(int _id);
    Edge(const Edge& other) = delete;
    ~Edge() = default;

    Edge& operator=(const Edge& other) = delete;

    int _id;

    //! Send some information about this Edge on an output stream
    void print(std::ostream& os) const;

    //! The two pointers to the faces adjacent to this Edge
    std::vector<Face*> _faces;

    //! The two pointers to the vertices that makes this Edge
    std::vector<Vertex*> _vertices;

    //! When not null indicates the new face formed by this Edge and a new vertex
    //! of the hull
    Face* _newFace;

    //! If true this Edge is marked to be deleted at the next clean up step
    bool _delete;
    
};

std::ostream& operator<<(std::ostream& os, const Edge& edge);

} // namespace nsx

#endif // BASE_HULL_EDGE_H
