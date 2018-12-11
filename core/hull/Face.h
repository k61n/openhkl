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
#include <vector>

#include <Eigen/Dense>

namespace nsx {

struct Edge;
struct Vertex;

/* !
 * \brief Class Face.
 * This class implements the Face object used in the incremental convex hull algorithm.
 */
struct Face {

    //! Default constructor
    Face();

    //! Default constructor
    Face(int id);

    //! Copy constructor
    Face(const Face& other) = delete;

    //! Destructor
    ~Face() = default;

    //! Assignment operator
    Face& operator=(const Face& other) = delete;

    //! Returns the int signed volume of the tetrahedron formed by this Face and a vertex.
    //! It is equal respectively to 1 and -1  if the vertex is on the negative or positive side of
    //! this Face with the positive side determined by the right hand rule. It is equal to 0 if the
    //! vertice is within the face
    int volumeSign(Vertex* v) const;

    //! Returns the volume of the tetrahedron formed by this Face and any point.
    double volume(const Eigen::Vector3d& pos) const;

    //! Send some informations about this Face on an output stream
    void print(std::ostream& os) const;

    //! The three pointer to the edges of this Face
    std::vector<Edge*> _edges;

    //! The three pointers to the vertices of this Face
    std::vector<Vertex*> _vertices;

    //! Indicates whether or not this Face is visible
    bool _visible;

    int _id;
};

std::ostream& operator<<(std::ostream& os, const Face& face);

} // end namespace nsx
