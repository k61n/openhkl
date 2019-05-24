#ifndef CORE_GEOMETRY_EDGE_H
#define CORE_GEOMETRY_EDGE_H

#include <ostream>

#include <vector>

namespace nsx {

struct Face;
struct Vertex;

//! \brief Class Edge.
//!
//! This class implements the Edge object used in the incremental convex hull
//! algorithm.
struct Edge {

    //! Default constructor
    Edge();

    Edge(int _id);

    //! Copy constructor
    Edge(const Edge& other) = delete;

    //! Destructor
    ~Edge() = default;

    //! Assignment operator
    Edge& operator=(const Edge& other) = delete;

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

    int _id;
};

std::ostream& operator<<(std::ostream& os, const Edge& edge);

} // end namespace nsx

#endif // CORE_GEOMETRY_EDGE_H
