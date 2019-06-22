//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      base/hull/ConvexHull.cpp
//! @brief     Implements class ConvexHull
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <limits>

#include "base/geometry/AABB.h"
#include "base/hull/ConvexHull.h"
#include "base/hull/Edge.h"
#include "base/hull/Face.h"
#include "base/hull/Triangle.h"
#include "base/hull/Vertex.h"

namespace nsx {

ConvexHull::ConvexHull(const ConvexHull& other)
{
    *this = other;
}

ConvexHull& ConvexHull::operator=(const ConvexHull& other)
{
    if (this != &other) {
        for (auto p : other._vertices) {
            Vertex* other_vertex = p.second;
            Vertex* new_vertex = new Vertex(other_vertex->_id, other_vertex->_coords);
            new_vertex->_onHull = other_vertex->_onHull;
            new_vertex->_mark = other_vertex->_mark;
            _vertices.emplace(new_vertex->_id, new_vertex);
        }

        for (auto p : other._edges) {
            Edge* other_edge = p.second;
            Edge* new_edge = new Edge(other_edge->_id);
            new_edge->_delete = other_edge->_delete;
            _edges.emplace(new_edge->_id, new_edge);
        }

        for (auto p : other._faces) {
            Face* other_face = p.second;
            Face* new_face = new Face(other_face->_id);
            new_face->_visible = other_face->_visible;
            _faces.emplace(new_face->_id, new_face);
        }

        for (auto p : _vertices) {
            Vertex* new_vertex = p.second;
            Vertex* other_vertex = other._vertices.at(p.first);
            new_vertex->_duplicate =
                other_vertex->_duplicate ? _edges[other_vertex->_duplicate->_id] : nullptr;
        }

        for (auto p : _edges) {
            Edge* new_edge = p.second;
            Edge* other_edge = other._edges.at(p.first);

            new_edge->_newFace = other_edge->_newFace ? _faces[other_edge->_newFace->_id] : nullptr;

            new_edge->_faces[0] = _faces[other_edge->_faces[0]->_id];
            new_edge->_faces[1] = _faces[other_edge->_faces[1]->_id];

            new_edge->_vertices[0] = _vertices[other_edge->_vertices[0]->_id];
            new_edge->_vertices[1] = _vertices[other_edge->_vertices[1]->_id];
        }

        for (auto p : _faces) {
            Face* new_face = p.second;
            Face* other_face = other._faces.at(p.first);

            new_face->_edges[0] = _edges[other_face->_edges[0]->_id];
            new_face->_edges[1] = _edges[other_face->_edges[1]->_id];
            new_face->_edges[2] = _edges[other_face->_edges[2]->_id];

            new_face->_vertices[0] = _vertices[other_face->_vertices[0]->_id];
            new_face->_vertices[1] = _vertices[other_face->_vertices[1]->_id];
            new_face->_vertices[2] = _vertices[other_face->_vertices[2]->_id];
        }

        _initialized = other._initialized;

        _planes = other._planes;

        _innerR2 = other._innerR2;
        _outerR2 = other._outerR2;

        _face_id = other._face_id;
        _edge_id = other._edge_id;
        _vertex_id = other._vertex_id;
    }
    return *this;
}

void ConvexHull::reset()
{
    for (auto p : _vertices)
        delete p.second;
    for (auto p : _edges)
        delete p.second;
    for (auto p : _faces)
        delete p.second;

    _vertices.clear();
    _edges.clear();
    _faces.clear();
    _planes.clear();

    _initialized = false;
}

bool ConvexHull::isCoplanar(Vertex* v0, Vertex* v1, Vertex* v2)
{
    Eigen::Vector3d va = v1->_coords - v0->_coords;
    Eigen::Vector3d vb = v2->_coords - v0->_coords;

    double norm = va.cross(vb).norm();

    return (norm < 1.0e-9);
}

ConvexHull::ConvexHull()
    : _initialized(false)
    , _vertices()
    , _edges()
    , _faces()
    , _planes()
    , _innerR2(0.0)
    , _outerR2(0.0)
    , _vertex_id(0)
    , _edge_id(0)
    , _face_id(0)
{
}

ConvexHull::~ConvexHull()
{
    reset();
}

Vertex* ConvexHull::addVertex(const Eigen::Vector3d& coords)
{
    for (auto p : _vertices) {
        Vertex* v = p.second;
        if ((coords - v->_coords).norm() / coords.norm() < 1.0e-6)
            throw std::runtime_error("Duplicate vertex (within 1.0e6 tolerance).");
    }

    Vertex* new_vertex = new Vertex(_vertex_id++, coords);
    _vertices.emplace(new_vertex->_id, new_vertex);

    return new_vertex;
}

bool ConvexHull::removeVertex(const Eigen::Vector3d& coords, double tolerance)
{
    for (auto it = _vertices.begin(); it != _vertices.end(); ++it) {
        Vertex* v = it->second;
        if ((v->_coords - coords).squaredNorm() < tolerance)
            _vertices.erase(it);
        return true;
    }
    return false;
}

Face* ConvexHull::buildNullFace()
{
    Face* f = new Face(_face_id++);
    return f;
}

Edge* ConvexHull::buildNullEdge()
{
    Edge* e = new Edge(_edge_id++);
    return e;
}

bool ConvexHull::findInitialVertices(int& ri, int& rj, int& rk) const
{
    if (_vertices.size() < 3)
        throw std::runtime_error("Can not set the initial polytope with less than 3 vertices.");

    for (auto it1 = _vertices.begin(); it1 != _vertices.end(); ++it1) {
        Vertex* vi = it1->second;
        for (auto it2 = ++it1; it2 != _vertices.end(); ++it2) {
            Vertex* vj = it2->second;
            for (auto it3 = ++it2; it3 != _vertices.end(); ++it3) {
                Vertex* vk = it3->second;
                if (!isCoplanar(vi, vj, vk)) {
                    ri = vi->_id;
                    rj = vj->_id;
                    rk = vk->_id;
                    return true;
                }
            }
        }
    }

    return false;
}

void ConvexHull::initalizeHull()
{
    // Find 3 non colinear vertices
    int ri, rj, rk;
    if (!findInitialVertices(ri, rj, rk))
        throw std::runtime_error("All vertices are coplanar 1.");

    Vertex* v0 = _vertices[ri];
    Vertex* v1 = _vertices[rj];
    Vertex* v2 = _vertices[rk];

    // Mark the three non colinear vertices found as processed
    v0->_mark = true;
    v1->_mark = true;
    v2->_mark = true;

    // Create the two twin faces
    auto f0 = buildFace(v0, v1, v2, nullptr);
    auto f1 = buildFace(v2, v1, v0, f0);

    // Link adjacent face
    f0->_edges[0]->_faces[1] = f1;
    f0->_edges[1]->_faces[1] = f1;
    f0->_edges[2]->_faces[1] = f1;
    f1->_edges[0]->_faces[1] = f0;
    f1->_edges[1]->_faces[1] = f0;
    f1->_edges[2]->_faces[1] = f0;

    // Find a fourth noncoplanar point to form tetrahedron
    for (auto p : _vertices) {
        Vertex* v = p.second;
        if (v->_mark)
            continue;

        if (f0->volumeSign(v) != 0) {
            processVertex(v);
            cleanUp();
            return;
        }
    }
}

Face* ConvexHull::buildFace(Vertex* v0, Vertex* v1, Vertex* v2, Face* fold)
{
    Edge *e0(nullptr), *e1(nullptr), *e2(nullptr);

    if (!fold) {
        e0 = buildNullEdge();
        e1 = buildNullEdge();
        e2 = buildNullEdge();

        _edges.emplace(e0->_id, e0);
        _edges.emplace(e1->_id, e1);
        _edges.emplace(e2->_id, e2);
    } else {
        e0 = fold->_edges[2];
        e1 = fold->_edges[1];
        e2 = fold->_edges[0];
    }

    e0->_vertices[0] = v0;
    e0->_vertices[1] = v1;

    e1->_vertices[0] = v1;
    e1->_vertices[1] = v2;

    e2->_vertices[0] = v2;
    e2->_vertices[1] = v0;

    // Create a new face
    auto f = buildNullFace();
    f->_edges[0] = e0;
    f->_edges[1] = e1;
    f->_edges[2] = e2;
    f->_vertices[0] = v0;
    f->_vertices[1] = v1;
    f->_vertices[2] = v2;

    _faces.emplace(f->_id, f);

    // Link the edges to the face
    e0->_faces[0] = f;
    e1->_faces[0] = f;
    e2->_faces[0] = f;

    return f;
}

void ConvexHull::updateHull()
{
    if (!_initialized) {
        initalizeHull();
        _initialized = true;
    }

    if (_vertices.size() < 4)
        throw std::runtime_error("Not enough vertices to build a convex hull.");

    // note: omitted it++ is intentional
    for (auto it = _vertices.begin(); it != _vertices.end();) {
        Vertex* v = it->second;
        if (!(v->_mark)) {
            processVertex(v);
            ++it;
            cleanUp();
            it = _vertices.begin();
        } else {
            ++it;
        }
    }

    // build cache of bounding planes
    _planes.clear();
    // inner and outer radius
    _innerR2 = std::numeric_limits<double>::max();
    _outerR2 = std::numeric_limits<double>::min();

    for (auto p : _faces) {
        Face* f = p.second;
        // note: vertices are stored right-handed as viewed from _outside_ the hull
        Eigen::Vector3d u = f->_vertices[1]->_coords - f->_vertices[0]->_coords;
        Eigen::Vector3d v = f->_vertices[2]->_coords - f->_vertices[0]->_coords;
        Eigen::Vector3d n = u.cross(v);
        n.normalize();
        double d = n.dot(f->_vertices[0]->_coords);
        _planes.emplace_back(n, d);

        // update inner radius
        _innerR2 = std::min(_innerR2, d * d);

        // update outer radius
        for (auto i = 0; i < 3; ++i)
            _outerR2 = std::max(_outerR2, f->_vertices[i]->_coords.squaredNorm());
    }
}

void ConvexHull::processVertex(Vertex* v)
{
    v->_mark = true;
    // Mark the faces that are visible from vertex v
    bool visible = false;
    for (auto p : _faces) {
        Face* f = p.second;
        if (f->volumeSign(v) < 0) {
            f->_visible = true;
            visible = true;
        }
    }

    // If no faces are visible from vertex v, then v is inside the hull
    if (!visible) {
        v->_onHull = false;
        return;
    }

    bool visible1, visible2;

    // note: omitted it++ is intentional
    for (auto it = _edges.begin(); it != _edges.end();) {
        auto e = it->second;
        if (e->_faces[0])
            visible1 = e->_faces[0]->_visible;
        else
            visible1 = false;
        if (e->_faces[1])
            visible2 = e->_faces[1]->_visible;
        else
            visible2 = false;
        if (visible1 && visible2)
            e->_delete = true;
        else if (visible1 || visible2)
            e->_newFace = buildConeFace(e, v);

        ++it;
    }
}

Face* ConvexHull::buildConeFace(Edge* e, Vertex* v)
{
    std::vector<Edge*> newEdges(2, nullptr);

    // Make two new edges (if they don't already exist)
    for (unsigned int i = 0; i < 2; ++i) {
        if (!(newEdges[i] = e->_vertices[i]->_duplicate)) {
            newEdges[i] = buildNullEdge();
            newEdges[i]->_vertices[0] = e->_vertices[i];
            newEdges[i]->_vertices[1] = v;
            _edges.emplace(newEdges[i]->_id, newEdges[i]);
            e->_vertices[i]->_duplicate = newEdges[i];
        };
    }

    // Make the new face
    auto newFace = buildNullFace();
    newFace->_edges[0] = e;
    newFace->_edges[1] = newEdges[0];
    newFace->_edges[2] = newEdges[1];
    orientate(newFace, e, v);

    _faces.emplace(newFace->_id, newFace);

    // Sets the adjacent faces
    for (unsigned int i = 0; i < 2; ++i) {
        for (unsigned int j = 0; j < 2; ++j) {
            if (!newEdges[i]->_faces[j]) {
                newEdges[i]->_faces[j] = newFace;
                break;
            }
        }
    }

    return newFace;
}

void ConvexHull::orientate(Face* f, Edge* e, Vertex* v)
{
    Face* fv;

    if (e->_faces[0]) {
        if (e->_faces[0]->_visible)
            fv = e->_faces[0];
        else
            fv = e->_faces[1];
    } else {
        fv = e->_faces[1];
    }

    size_t idx;
    for (idx = 0; idx < 3; ++idx) {
        if (fv->_vertices[idx] == e->_vertices[0])
            break;
    }

    // Orient f the same as fv
    if (fv->_vertices[(idx + 1) % 3] != e->_vertices[1]) {
        f->_vertices[0] = e->_vertices[1];
        f->_vertices[1] = e->_vertices[0];
    } else {
        f->_vertices[0] = e->_vertices[0];
        f->_vertices[1] = e->_vertices[1];
        Edge* temp;
        temp = f->_edges[1];
        f->_edges[1] = f->_edges[2];
        f->_edges[2] = temp;
    }

    f->_vertices[2] = v;
}

const std::map<int, Vertex*>& ConvexHull::vertices() const
{
    return _vertices;
}

const std::map<int, Edge*>& ConvexHull::edges() const
{
    return _edges;
}

const std::map<int, Face*>& ConvexHull::faces() const
{
    return _faces;
}

void ConvexHull::cleanEdges()
{
    for (auto& p : _edges) {
        Edge* e = p.second;
        if (e->_newFace) {
            if (e->_faces[0]->_visible)
                e->_faces[0] = e->_newFace;
            else
                e->_faces[1] = e->_newFace;
            e->_newFace = nullptr;
        }
    }

    for (auto it = _edges.begin(); it != _edges.end();) {
        Edge* e = it->second;
        if (e->_delete) {
            delete e;
            it = _edges.erase(it);
        } else {
            ++it;
        }
    }
}

void ConvexHull::cleanFaces()
{
    for (auto it = _faces.begin(); it != _faces.end();) {
        Face* f = it->second;
        if (f->_visible) {
            delete f;
            it = _faces.erase(it);
        } else {
            ++it;
        }
    }
}

void ConvexHull::cleanVertices()
{
    for (auto& p : _edges) {
        Edge* e = p.second;
        e->_vertices[0]->_onHull = true;
        e->_vertices[1]->_onHull = true;
    }

    for (auto it = _vertices.begin(); it != _vertices.end();) {
        Vertex* v = it->second;
        if (v->_mark && !v->_onHull) {
            delete v;
            it = _vertices.erase(it);
        } else {
            ++it;
        }
    }

    for (auto p : _vertices) {
        Vertex* v = p.second;
        v->_duplicate = nullptr;
        v->_onHull = false;
    }
}

void ConvexHull::cleanUp()
{
    cleanEdges();
    cleanFaces();
    cleanVertices();
}

AABB ConvexHull::aabb() const
{
    if (_vertices.empty())
        return {};

    Eigen::Vector3d lower(
        std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(),
        std::numeric_limits<double>::infinity());

    Eigen::Vector3d upper = -lower;

    for (auto p : _vertices) {
        Vertex* v = p.second;
        for (auto i = 0; i < 3; ++i) {
            if (v->_coords(i) < lower(i))
                lower(i) = v->_coords(i);
            if (v->_coords(i) > upper(i))
                upper(i) = v->_coords(i);
        }
    }

    return AABB(lower, upper);
}

Eigen::Vector3d ConvexHull::center() const
{
    Eigen::Vector3d center = Eigen::Vector3d::Zero();
    for (auto p : _vertices)
        center += p.second->_coords;
    center /= _vertices.size();

    return center;
}

void ConvexHull::translate(double x, double y, double z) const
{
    for (auto p : _vertices) {
        Vertex* v = p.second;
        v->_coords[0] += x;
        v->_coords[1] += y;
        v->_coords[2] += z;
    }
}

void ConvexHull::scale(double factor) const
{
    for (auto p : _vertices) {
        Vertex* v = p.second;
        v->_coords *= factor;
    }
}

void ConvexHull::rotate(const Eigen::Matrix3d& rotation) const
{
    for (auto p : _vertices) {
        Vertex* v = p.second;
        v->_coords = rotation * v->_coords;
    }
}

void ConvexHull::translateToCenter() const
{
    Eigen::Vector3d c = -center();
    translate(c[0], c[1], c[2]);
}

double ConvexHull::volume() const
{
    Eigen::Vector3d c = center();

    double volume = 0.0;
    for (auto p : _faces) {
        Face* f = p.second;
        volume += f->volume(c);
    }

    return volume;
}

size_t ConvexHull::nVertices() const
{
    return _vertices.size();
}

size_t ConvexHull::nEdges() const
{
    return _edges.size();
}

size_t ConvexHull::nFaces() const
{
    return _faces.size();
}

bool ConvexHull::checkEulerConditions() const
{
    unsigned int nVertices = _vertices.size();
    unsigned int nEdges = _edges.size();
    unsigned int nFaces = _faces.size();

    if (nVertices < 4)
        return false;

    if ((nVertices - nEdges + nFaces) != 2)
        return false;

    if (nFaces != (2 * nVertices - 4))
        return false;

    if ((2 * nEdges) != (3 * nFaces))
        return false;

    return true;
}

std::vector<Triangle> ConvexHull::createFaceCache(const Eigen::Matrix3d& rotation) const
{
    if (_vertices.size() < 4)
        throw std::runtime_error("Hull is flat or undefined, can not construct faces information");

    std::vector<Triangle> triangles;
    triangles.reserve(_faces.size());
    for (auto p : _faces) {
        Face* f = p.second;
        Eigen::Vector3d v0 = rotation * Eigen::Vector3d(f->_vertices[0]->_coords);
        Eigen::Vector3d v1 = rotation * Eigen::Vector3d(f->_vertices[1]->_coords);
        Eigen::Vector3d v2 = rotation * Eigen::Vector3d(f->_vertices[2]->_coords);
        triangles.push_back(Triangle(v0, v1, v2));
    }
    return triangles;
}

void ConvexHull::print(std::ostream& os) const
{
    os << "Convex Hull:\n" << std::endl;
    for (auto p : _vertices) {
        Vertex* v = p.second;
        os << *v << std::endl;
    }
}

std::ostream& operator<<(std::ostream& os, const ConvexHull& chull)
{
    chull.print(os);
    return os;
}

// Note: this function is absolutely performance critical.
// Make changes with caution, and remember to profile!
bool ConvexHull::contains(const Eigen::Vector3d& v) const
{
    const double r2 = v.squaredNorm();

    if (r2 > _outerR2)
        return false;

    if (r2 <= _innerR2)
        return true;

    for (const auto& pair : _planes) {
        double dot = 0.0;
        for (size_t i = 0; i < 3; ++i)
            dot += v(i) * pair.first(i);
        if (dot > pair.second)
            return false;
    }
    return true;
}

} // namespace nsx
