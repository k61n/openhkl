/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon, Eric Pellegrini
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
