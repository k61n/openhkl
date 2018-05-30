#include "AABB.h"
#include "ConvexHull.h"
#include "Edge.h"
#include "Face.h"
#include "Triangle.h"
#include "Vertex.h"

namespace nsx {

ConvexHull::ConvexHull(const ConvexHull& other): ConvexHull()
{
    *this = other;
}

ConvexHull& ConvexHull::operator=(const ConvexHull& other)
{
    reset();
    _initialized = false;
    
    for (auto v: other._vertices) {
        addVertex(v->_coords);
    }

    if (other._initialized) {
        updateHull();
    }
    return *this;
}

void ConvexHull::reset()
{
    for (auto v : _vertices)
        delete v;
    for (auto e : _edges)
        delete e;
    for (auto f : _faces)
        delete f;
    _vertices.clear();
    _edges.clear();
    _faces.clear();
    _planes.clear();
    _initialized=false;
}

bool ConvexHull::isCoplanar(Vertex* v0, Vertex* v1, Vertex* v2)
{
    Eigen::Vector3d va=v1->_coords - v0->_coords;
    Eigen::Vector3d vb=v2->_coords - v0->_coords;

    double norm=va.cross(vb).norm();

    return (norm<1.0e-9);
}

ConvexHull::ConvexHull() : _initialized(false), _vertices(), _edges(), _faces(), _planes()
{
}

ConvexHull::~ConvexHull()
{
    reset();
}

Vertex* ConvexHull::addVertex(const Eigen::Vector3d& coords)
{

    for (const auto& v : _vertices) {
        if (std::abs(coords[0]-v->_coords[0])<1.0e-6 && std::abs(coords[1]-v->_coords[1])<1.0e-6 && std::abs(coords[2]-v->_coords[2])<1.0e-6)
            throw std::runtime_error("Duplicate vertex (within 1.0e6 tolerance).");
    }

    auto v=new Vertex(coords);
    _vertices.push_back(v);
    return _vertices.back();
}

bool ConvexHull::removeVertex(const Eigen::Vector3d& coords, double tolerance)
{
    for (auto it=_vertices.begin();it!=_vertices.end();++it)
    {
        if (((*it)->_coords-coords).squaredNorm() < tolerance)
            _vertices.erase(it);
        return true;
    }
    return false;
}

Face* ConvexHull::buildNullFace()
{
    Face* f=new Face();
    return f;
}

Edge* ConvexHull::buildNullEdge()
{
    auto e=new Edge();
    return e;
}

bool ConvexHull::findInitialVertices(Vertex*& v0, Vertex*& v1, Vertex*& v2) const
{
    if (_vertices.size()<3)
        throw std::runtime_error("Can not set the initial polytope with less than 3 vertices.");

    for (auto it1=_vertices.begin();it1!=_vertices.end();++it1)
    {
        v0=*it1;
        for (auto it2=++it1;it2!=_vertices.end();++it2)
        {
            v1=*it2;
            for (auto it3=++it2;it3!=_vertices.end();++it3)
            {
                v2=*it3;
                if (!isCoplanar(v0,v1,v2))
                    return true;
            }
        }
    }
    return false;
}

void ConvexHull::initalizeHull()
{

    // Find 3 non colinear vertices
    Vertex *v0(nullptr),*v1(nullptr),*v2(nullptr);
    if (!findInitialVertices(v0,v1,v2))
        throw std::runtime_error("All vertices are coplanar 1.");

    // Mark the three non colinear vertices found as processed
    v0->_mark=true;
    v1->_mark=true;
    v2->_mark=true;

    // Create the two twin faces
    auto f0=buildFace(v0,v1,v2);
    auto f1=buildFace(v2,v1,v0,f0);

    // Link adjacent face
    f0->_edges[0]->_adjFace[1]=f1;
    f0->_edges[1]->_adjFace[1]=f1;
    f0->_edges[2]->_adjFace[1]=f1;
    f1->_edges[0]->_adjFace[1]=f0;
    f1->_edges[1]->_adjFace[1]=f0;
    f1->_edges[2]->_adjFace[1]=f0;

    // Find a fourth noncoplanar point to form tetrahedron
    for (auto v : _vertices)
    {
        if (v->_mark)
            continue;

        if (f0->volumeSign(v)!=0)
        {
            processVertex(v);
            cleanUp();
            return;
        }
    }
}

Face* ConvexHull::buildFace(Vertex* v0, Vertex* v1, Vertex* v2, Face* fold)
{
    Edge *e0(nullptr),*e1(nullptr),*e2(nullptr);

    if (!fold)
    {
        e0=buildNullEdge();
        e1=buildNullEdge();
        e2=buildNullEdge();
        _edges.push_back(e0);
        _edges.push_back(e1);
        _edges.push_back(e2);
    }
    else
    {
        e0=fold->_edges[2];
        e1=fold->_edges[1];
        e2=fold->_edges[0];
    }

    e0->_endPts[0]=v0;
    e0->_endPts[1]=v1;

    e1->_endPts[0]=v1;
    e1->_endPts[1]=v2;

    e2->_endPts[0]=v2;
    e2->_endPts[1]=v0;

    // Create a new face
    auto f=buildNullFace();
    f->_edges[0]=e0;
    f->_edges[1]=e1;
    f->_edges[2]=e2;
    f->_vertices[0]=v0;
    f->_vertices[1]=v1;
    f->_vertices[2]=v2;

    _faces.push_back(f);

    // Link the edges to the face
    e0->_adjFace[0]=f;
    e1->_adjFace[0]=f;
    e2->_adjFace[0]=f;

    return _faces.back();

}

void ConvexHull::updateHull()
{
    if (!_initialized) {
        initalizeHull();
        _initialized=true;
    }

    if (_vertices.size()<4)
        throw std::runtime_error("Not enough vertices to build a convex hull.");

    // note: omitted it++ is intentional
    for (auto it = _vertices.begin(); it != _vertices.end(); ) {
        if (!((*it)->_mark)) {
            processVertex(*it);
            ++it;
            cleanUp();
            it=_vertices.begin();
        } else {
            ++it;
        }
    }

    // build cache of bounding planes
    _planes.clear();
    // inner and outer radius
    _innerR2 = std::numeric_limits<double>::max();
    _outerR2 = std::numeric_limits<double>::min();

    for (auto face: _faces) {
        // note: vertices are stored right-handed as viewed from _outside_ the hull
        Eigen::Vector3d u = face->_vertices[1]->_coords - face->_vertices[0]->_coords;
        Eigen::Vector3d v = face->_vertices[2]->_coords - face->_vertices[0]->_coords;
        Eigen::Vector3d n = u.cross(v);
        n.normalize();
        double d = n.dot(face->_vertices[0]->_coords);
        _planes.emplace_back(n, d);

        // update inner radius
        _innerR2 = std::min(_innerR2, d*d);

        // update outer radius
        for (auto i = 0; i < 3; ++i) {
            _outerR2 = std::max(_outerR2, face->_vertices[i]->_coords.squaredNorm());
        }
    }
}

void ConvexHull::processVertex(Vertex* v)
{
    v->_mark = true;
    // Mark the faces that are visible from vertex v
    bool visible = false;
    for (auto& f : _faces) {
        if (f->volumeSign(v)<0) {
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
        auto e = *it;
        if (e->_adjFace[0]) {
            visible1 = e->_adjFace[0]->_visible;
        } else {
            visible1 = false;
        }
        if (e->_adjFace[1]) {
            visible2 = e->_adjFace[1]->_visible;
        } else {
            visible2 = false;
        }
        if (visible1 && visible2) {
            e->_delete = true;
        } else if (visible1 || visible2) {
            e->_newFace = buildConeFace(e,v);
        }
        ++it;
    }
}

Face* ConvexHull::buildConeFace(Edge* e, Vertex* v)
{

    std::array<Edge*,2> newEdges;
    newEdges.fill(nullptr);

    // Make two new edges (if they don't already exist)
    for (unsigned int i=0;i<2;++i)
    {
        if (!(newEdges[i]=e->_endPts[i]->_duplicate))
        {
            newEdges[i] = buildNullEdge();
            newEdges[i]->_endPts[0] = e->_endPts[i];
            newEdges[i]->_endPts[1] = v;
            _edges.push_back(newEdges[i]);
            e->_endPts[i]->_duplicate = newEdges[i];
        };
    }

    // Make the new face
    auto newFace = buildNullFace();
    newFace->_edges[0] = e;
    newFace->_edges[1] = newEdges[0];
    newFace->_edges[2] = newEdges[1];
    orientate(newFace,e,v);

    _faces.push_back(newFace);

    // Set the adjacent faces
    for (unsigned int i=0;i<2;++i)
    {
        for (unsigned int j=0;j<2;++j)
        {
            if (!newEdges[i]->_adjFace[j])
            {
                newEdges[i]->_adjFace[j] = newFace;
                break;
            }
        }
    }

    return _faces.back();
}

void ConvexHull::orientate(Face* f, Edge* e, Vertex* v)
{

    Face* fv;

    if (e->_adjFace[0])
    {
        if (e->_adjFace[0]->_visible)
            fv = e->_adjFace[0];
        else
            fv = e->_adjFace[1];
    }
    else
        fv = e->_adjFace[1];

    unsigned int idx;
    for (idx=0;fv->_vertices[idx]!=e->_endPts[0];++idx)
    {

    }

    // Orient f the same as fv
    if (fv->_vertices[(idx+1)%3] != e->_endPts[1])
    {
        f->_vertices[0] = e->_endPts[1];
        f->_vertices[1] = e->_endPts[0];
    }
    else
    {
        f->_vertices[0] = e->_endPts[0];
        f->_vertices[1] = e->_endPts[1];
        Edge* temp;
        temp         = f->_edges[1];
        f->_edges[1] = f->_edges[2];
        f->_edges[2] = temp;
    }


    f->_vertices[2] = v;

}

const std::list<Vertex*>& ConvexHull::getVertices() const
{
    return _vertices;
}

const std::list<Edge*>& ConvexHull::getEdges() const
{
    return _edges;
}

const std::list<Face*>& ConvexHull::getFaces() const
{
    return _faces;
}

void ConvexHull::cleanEdges()
{
    for (auto& e : _edges)
    {
        if (e->_newFace)
        {
            if (e->_adjFace[0]->_visible)
                e->_adjFace[0] = e->_newFace;
            else
                e->_adjFace[1] = e->_newFace;
            e->_newFace=nullptr;
        }
    }

    for (auto it=_edges.begin();it!=_edges.end();)
    {
        if ((*it)->_delete)
        {
            delete *it;
            it=_edges.erase(it);
        }
        else
            ++it;
    }
}

void ConvexHull::cleanFaces()
{
    for (auto it=_faces.begin();it!=_faces.end();)
    {
        if ((*it)->_visible)
        {
            delete *it;
            it=_faces.erase(it);
        }
        else
            ++it;
    }
}

void ConvexHull::cleanVertices()
{
    for (auto& e : _edges)
    {
        e->_endPts[0]->_onHull=true;
        e->_endPts[1]->_onHull=true;
    }

    for (auto it=_vertices.begin();it!=_vertices.end();)
    {
        if ((*it)->_mark && !(*it)->_onHull)
        {
            delete *it;
            it=_vertices.erase(it);
        }
        else
            ++it;
    }

    for (auto& v : _vertices)
    {
        v->_duplicate=nullptr;
        v->_onHull=false;
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
    if (_vertices.empty()) {
        return {};
    }

    Eigen::Vector3d lower, upper;
    lower = _vertices.front()->_coords;
    upper = lower;

    for (const auto& v: _vertices) {
        for (auto i = 0; i < 3; ++i) {
            if ( v->_coords(i) < lower(i)) {
                lower(i) = v->_coords(i);
            }
            if ( v->_coords(i) > upper(i)) {
                upper(i) = v->_coords(i);
            }
        }
    }

    return AABB(lower, upper);
}

Eigen::Vector3d ConvexHull::getCenter() const
{
    Eigen::Vector3d center=Eigen::Vector3d::Zero();
    for (auto & v : _vertices)
        center += v->_coords;
    center/=_vertices.size();

    return center;
}

void ConvexHull::translate(double x, double y, double z) const
{
    for (auto& v : _vertices)
    {
        v->_coords[0] += x;
        v->_coords[1] += y;
        v->_coords[2] += z;
    }
}

void ConvexHull::scale(double factor) const
{
    for (auto& v : _vertices)
    {
        v->_coords*=factor;
    }
}

void ConvexHull::rotate(const Eigen::Matrix3d& rotation) const
{
    for (auto& v : _vertices)
        v->_coords = rotation*v->_coords;
}

void ConvexHull::translateToCenter() const
{

    Eigen::Vector3d center=-getCenter();
    translate(center[0],center[1],center[2]);
}

double ConvexHull::getVolume() const
{

    Eigen::Vector3d center=getCenter();

    double volume=0.0;
    for (auto& f : _faces)
        volume += f->volume(center);

    return volume;

}

unsigned int ConvexHull::getNVertices() const
{
    return _vertices.size();
}

unsigned int ConvexHull::getNEdges() const
{
    return _edges.size();
}

unsigned int ConvexHull::getNFaces() const
{
    return _faces.size();
}

bool ConvexHull::checkEulerConditions() const
{
    unsigned int nVertices=_vertices.size();
    unsigned int nEdges=_edges.size();
    unsigned int nFaces=_faces.size();

    if (nVertices<4)
        return false;

    if ( (nVertices - nEdges + nFaces) != 2 )
        return false;

    if ( nFaces != (2 * nVertices - 4) )
        return false;

    if ( (2 * nEdges) != (3 * nFaces) )
        return false;

    return true;
}

std::vector<Triangle > ConvexHull::createFaceCache(const Eigen::Matrix3d& rotation) const
{
    if (_vertices.size()<4)
        throw std::runtime_error("Hull is flat or undefined, can not construct faces information");

    std::vector<Triangle > triangles;
    triangles.reserve(_faces.size());
    for (const auto& f: _faces)
    {
        Eigen::Vector3d v0=rotation*Eigen::Vector3d(f->_vertices[0]->_coords);
        Eigen::Vector3d v1=rotation*Eigen::Vector3d(f->_vertices[1]->_coords);
        Eigen::Vector3d v2=rotation*Eigen::Vector3d(f->_vertices[2]->_coords);
        triangles.push_back(Triangle(v0,v1,v2));
    }
    return triangles;
}

void ConvexHull::print(std::ostream& os) const
{
    os<<"Convex Hull:\n"<<std::endl;
    for (auto v : _vertices)
        os<< *v << std::endl;
}

std::ostream& operator<<(std::ostream& os, const ConvexHull& chull)
{
    chull.print(os);
    return os;
}

// Note: this function is absolutely performance critical. 
// Make changes with caution, and remember to profile!
bool ConvexHull::contains(const Eigen::Vector3d& v) const {

    const double r2 = v.squaredNorm();

    if (r2 > _outerR2) {
        return false;
    }

    if (r2 <= _innerR2) {
        return true;
    }

    for (const auto& pair: _planes) {
        double dot = 0.0;
        for (size_t i = 0; i < 3; ++i) {
            dot += v(i)*pair.first(i);
        }
        if (dot > pair.second) {
            return false;
        }
    }
    return true;
}

} // end namespace nsx

