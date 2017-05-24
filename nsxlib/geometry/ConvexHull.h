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

#ifndef NSXLIB_CONVEXHULL_H
#define NSXLIB_CONVEXHULL_H

#include <array>
#include <cmath>
#include <iterator>
#include <list>
#include <stdexcept>

#include <Eigen/Dense>

#include "../geometry/GeometryTypes.h"

namespace nsx {

/* !
 * \brief Class ConvexHull.
 * This class implements an templated object-oriented adaptation of the 3D incremental convex hull
 * algorithm whose implementation in C has been described in:
 *
 * o'Rourke, Joseph. Computational geometry in C. Cambridge university press, 1998. 115-144.
 *
 * Basically, the algorithm works on the same way than for the 2D case. It works by building counter clockwise
 * oriented faces (triangles) of the hull and at each step a new point is defined to be part of the hull if its
 * signed volume is negative for at least one face. Otherwise the point is discarded. When a point is added to
 * the hull, this one is extended by building new faces that link the point to the edges of the visible faces of
 * the hull from that point. After that step, the hull is updated by removing the faces (and their corresponding
 * edges) that has been completety "buried" by the hull growth.
 *
 * Some of the methods names have been renamed regarding to the C implementation because we felt more comfortable
 * with during those names in the curse of the implementation. In such case the original name will be recalled
 * when documenting those methods.
 */

class ConvexHull {

public:

    //! Checks whether three vertices are coplanar
    static bool isCoplanar(Vertex* v0, Vertex* v1, Vertex* v2);

public:

    //! Default constructor
    ConvexHull();

    //! Destructor
    ~ConvexHull();

    //! Copy Convex Hull
    ConvexHull(const ConvexHull&);
    ConvexHull& operator=(const ConvexHull&);
    //! Reset, eliminate all vertices, edges and faces
    void reset();

    //! Adds a new vertex to the list of points to be processed later when calling updateHull method.
    Vertex* addVertex(const Eigen::Vector3d& coords);
    Vertex* addVertex(double x, double y, double z);
    //! Remove vertex
    bool removeVertex(const Eigen::Vector3d& coords,double tolerance);
    bool removeVertex(double x, double y, double z, double tolerance);
    //! Updates the hull. The first time it is called two seed-triangles with opposite orientation will be
    //! created on which the next hull faces will be built upon.
    //! Original name: ConstructHull
    void updateHull();

    //! Returns the vertices of the hull
    const std::list<Vertex*>& getVertices() const;
    //! Returns the number of vertices of the hull
    unsigned int getNVertices() const;

    //! Returns the edges of the hull
    const std::list<Edge*>& getEdges() const;
    //! Returns the number of edges of the hull
    unsigned int getNEdges() const;

    //! Returns the faces of the hull
    const std::list<Face*>& getFaces() const;
    //! Returns the number of faces of the hull
    unsigned int getNFaces() const;

    //! Returns the center of gravity of the hull.
    Eigen::Vector3d getCenter() const;

    //! Translates the hull
    void translate(double x, double y, double z) const;

    //! Translates the hull to its center
    void translateToCenter() const;

    //! Isotropic scaling of the Hull
    void scale(double factor) const;

    //! Rotate the this ConvexHull
    void rotate(const Eigen::Matrix3d& rotation) const;

    //! Returns the volume of the hull. The volume is computed by summing the volumes of all the tethrahedrons
    //! made by each face of the convex hull and any internal point of the hull.
    //! A reasonable choice for the internal point is the center of gravity of the hull as, by definition of a
    //! convex object, its center is within the hull.
    double getVolume() const;

    //! Sends some informations on an output stream.
    void print(std::ostream& os) const;

    //! Checks that this Hull satisfies the Euler condition
    bool checkEulerConditions() const;

    //! Returns the Triangles that builds this ConvexHull if the Hull was to be rotated by the rotation matrix: rotation
    //! Triangles objects are very light object that can be used
    //! for further analysis such as Monte-Carlo based absorption correction.
    std::vector<Triangle> createFaceCache(const Eigen::Matrix3d& rotation=Eigen::Matrix3d::Identity()) const;

private:

    //! Initializes the hull. The initialization consists in defines two triangles with opposite orientations that will
    //! serve as seeds for the faces of the hull to be built later
    void initalizeHull();

    //! Finds a set of three vertices that are not coplanar
    bool findInitialVertices(Vertex*& v0, Vertex*& v1, Vertex*& v2) const;

    //! Processes a vertex to see whether it will be a vertex of the hull or ill be discarded.
    //! Original name: addOne
    void processVertex(Vertex* v);

    //! Builds a null Face (that points to no edges and vertices)
    //! Original name: makeNullFace
    Face* buildNullFace();

    //! Builds a null Edge (that points to no vertices)
    //! Original name: makeNullEdge
    Edge* buildNullEdge();

    //! Builds a new hull face from three vertices. If fold is not nullptr then its edges will be used
    //! to defie the one of the new face to build.
    //! Original name: MakeFace
    Face* buildFace(Vertex* v0, Vertex* v1, Vertex* v2, Face* fold=nullptr);

    //! Builds a new hull face from the edges of an existing face and a new vertex.
    //! Original name: MakeConeFace
    Face* buildConeFace(Edge* e, Vertex* v);

    //! Orientates a face of the hull given a reference edge and vertex
    //! Original name: makeCcw
    void orientate(Face* f, Edge* e, Vertex* v);

    //! Cleans the edges of the hull that are not visible anymore
    void cleanEdges();

    //! Cleans the faces of the hull that are not visible anymore
    void cleanFaces();

    //! Cleans the vertices of the hull that are not visible anymore
    void cleanVertices();

    //! Cleans the edges, faces and vertices of the hull that are not visible anymore
    void cleanUp();

    //! Get AABB of the convex hull
    AABB getAABB() const;

    //! Return whether a vertex is contained in the hull
    bool constains(const Vertex& v) const;

private:

    //! A boolean that indicates whether or not the two seed-triangles of the hull have been already created
    bool _initialized;
    //! The list of the vertices of the hull. This is the std implementation of the double-linked chain.
    std::list<Vertex*> _vertices;
    //! The list of the edges of the hull. This is the std implementation of the double-linked chain.
    std::list<Edge*> _edges;
    //! The list of the faces of the hull. This is the std implementation of the double-linked chain.
    std::list<Face*> _faces;

};

std::ostream& operator<<(std::ostream& os, const ConvexHull& chull);

} // end namespace nsx

#endif // NSXLIB_CONVEXHULL_H
