#pragma once

#include <ostream>

#include <Eigen/Dense>

namespace nsx {

struct Edge;

/* !
 * \brief Class Vertex.
 * This class implements the Vertex object used in the incremental convex hull
 * algorithm.
 */
struct Vertex {

    //! Default constructor
    Vertex();

    //! Default constructor
    Vertex(int id);

    //! Copy constructor
    Vertex(const Vertex& other) = delete;

    //! Constructs a Vertex object from a vector of coordinates
    Vertex(int id, const Eigen::Vector3d& coords);

    //! Destructor
    ~Vertex() = default;

    //! Assignment operator
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

} // end namespace nsx
