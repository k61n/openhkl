//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/hull/ConvexHull.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_HULL_CONVEXHULL_H
#define CORE_HULL_CONVEXHULL_H

#include <map>


#include "core/geometry/AABB.h"

namespace nsx {

struct Edge;
struct Face;
struct Vertex;

//! \brief Class ConvexHull.
//! This class implements an templated object-oriented adaptation of the 3D
//! incremental convex hull algorithm whose implementation in C has been
//! described in: o'Rourke, Joseph. Computational geometry in C. Cambridge
//! university press, 1998. 115-144.
//!
//! Basically, the algorithm works on the same way than for the 2D case. It
//! works by building counter clockwise oriented faces (triangles) of the hull
//! and at each step a new point is defined to be part of the hull if its signed
//! volume is negative for at least one face. Otherwise the point is discarded.
//! When a point is added to the hull, this one is extended by building new
//! faces that link the point to the edges of the visible faces of the hull from
//! that point. After that step, the hull is updated by removing the faces (and
//! their corresponding edges) that has been completety "buried" by the hull
//! growth.
//!
//! Some of the methods names have been renamed regarding to the C
//! implementation because we felt more comfortable with during those names in
//! the curse of the implementation. In such case the original name will be
//! recalled when documenting those methods.
class ConvexHull {

public:
    //! Checks whether three vertices are coplanar
    static bool isCoplanar(Vertex* v0, Vertex* v1, Vertex* v2);
    ConvexHull();
    ~ConvexHull();

    //! Copy Convex Hull
    ConvexHull(const ConvexHull&);

    //! Assignment
    ConvexHull& operator=(const ConvexHull&);

    //! Reset, eliminate all vertices, edges and faces
    void reset();

    //! Adds a new vertex to the list of points to be processed later when calling
    //! updateHull method.
    Vertex* addVertex(const Eigen::Vector3d& coords);

    //! Remove vertex
    bool removeVertex(const Eigen::Vector3d& coords, double tolerance);

    //! Updates the hull. The first time it is called two seed-triangles with
    //! opposite orientation will be created on which the next hull faces will be
    //! built upon. Original name: ConstructHull
    void updateHull();

    //! Returns the vertices of the hull
    const std::map<int, Vertex*>& vertices() const;

    //! Returns the number of vertices of the hull
    size_t nVertices() const;

    //! Returns the edges of the hull
    const std::map<int, Edge*>& edges() const;

    //! Returns the number of edges of the hull
    size_t nEdges() const;

    //! Returns the faces of the hull
    const std::map<int, Face*>& faces() const;

    //! Returns the number of faces of the hull
    size_t nFaces() const;

    //! Returns the center of gravity of the hull.
    Eigen::Vector3d center() const;

    //! Translates the hull
    void translate(double x, double y, double z) const;

    //! Translates the hull to its center
    void translateToCenter() const;

    //! Isotropic scaling of the Hull
    void scale(double factor) const;

    //! Rotate the this ConvexHull
    void rotate(const Eigen::Matrix3d& rotation) const;

    //! Returns the volume of the hull. The volume is computed by summing the
    //! volumes of all the tethrahedrons made by each face of the convex hull and
    //! any internal point of the hull. A reasonable choice for the internal point
    //! is the center of gravity of the hull as, by definition of a convex object,
    //! its center is within the hull.
    double volume() const;

    //! Send some informations on an output stream.
    void print(std::ostream& os) const;

    //! Checks that this Hull satisfies the Euler condition
    bool checkEulerConditions() const;

    //! Returns the Triangles that builds this ConvexHull if the Hull was to be
    //! rotated by the rotation matrix: rotation Triangles objects are very light
    //! object that can be used for further analysis such as Monte-Carlo based
    //! absorption correction.
    std::vector<Triangle>
    createFaceCache(const Eigen::Matrix3d& rotation = Eigen::Matrix3d::Identity()) const;

    //! Returns whether a vertex is contained in the hull
    bool contains(const Eigen::Vector3d& v) const;

    //! Gets AABB of the convex hull
    AABB aabb() const;

private:
    //! Initializes the hull. The initialization consists in defines two triangles
    //! with opposite orientations that will serve as seeds for the faces of the
    //! hull to be built later
    void initalizeHull();

    //! Finds a set of three vertices that are not coplanar
    bool findInitialVertices(int& ri, int& rj, int& rk) const;

    //! Processes a vertex to see whether it will be a vertex of the hull or ill
    //! be discarded. Original name: addOne
    void processVertex(Vertex* v);

    //! Builds a null Face (that points to no edges and vertices)
    //! Original name: makeNullFace
    Face* buildNullFace();

    //! Builds a null Edge (that points to no vertices)
    //! Original name: makeNullEdge
    Edge* buildNullEdge();

    //! Builds a new hull face from three vertices. If fold is not nullptr then
    //! its edges will be used to defie the one of the new face to build. Original
    //! name: MakeFace
    Face* buildFace(Vertex* v0, Vertex* v1, Vertex* v2, Face* fold);

    //! Builds a new hull face from the edges of an existing face and a new
    //! vertex. Original name: MakeConeFace
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

    //! Cleans the edges, faces and vertices of the hull that are not visible
    //! anymore
    void cleanUp();

private:
    //! A boolean that indicates whether or not the two seed-triangles of the hull
    //! have been already created
    bool _initialized;

    //! The list of the vertices of the hull. This is the std implementation of
    //! the double-linked chain.
    std::map<int, Vertex*> _vertices;

    //! The list of the edges of the hull. This is the std implementation of the
    //! double-linked chain.
    std::map<int, Edge*> _edges;

    //! The list of the faces of the hull. This is the std implementation of the
    //! double-linked chain.
    std::map<int, Face*> _faces;

    //! Cached bounding planes of the hull.
    std::vector<std::pair<Eigen::Vector3d, double>> _planes;

    //! Cached inner squared raddi
    double _innerR2;

    //! Cached outer squared raddi
    double _outerR2;

    int _vertex_id;

    int _edge_id;

    int _face_id;
};

std::ostream& operator<<(std::ostream& os, const ConvexHull& chull);

} // end namespace nsx

#endif // CORE_HULL_CONVEXHULL_H
