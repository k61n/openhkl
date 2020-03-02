//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      base/hull/Face.h
//! @brief     Defines struct Face
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef BASE_HULL_FACE_H
#define BASE_HULL_FACE_H

namespace nsx {

struct Edge;
struct Vertex;

//! A face, used in the incremental convex hull algorithm.

struct Face {
    Face();
    Face(int id);
    Face(const Face& other) = delete;
    ~Face() = default;
    Face& operator=(const Face& other) = delete;

    int _id;

    //! Returns the int signed volume of the tetrahedron formed by this Face and a
    //! vertex. It is equal respectively to 1 and -1  if the vertex is on the
    //! negative or positive side of this Face with the positive side determined
    //! by the right hand rule. It is equal to 0 if the vertice is within the face
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
};

std::ostream& operator<<(std::ostream& os, const Face& face);

} // namespace nsx

#endif // BASE_HULL_FACE_H
