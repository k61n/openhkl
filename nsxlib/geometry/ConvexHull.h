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
#include <iostream>
#include <iterator>
#include <list>

#include <Eigen/Dense>

#include "AABB.h"
#include "Edge.h"
#include "Vertex.h"
#include "Face.h"
#include "Triangle.h"
#include "../kernel/Error.h"

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
template <typename T>
class ConvexHull
{

public:

    // Typedefs
    typedef Eigen::Matrix<T,3,1> vector3;
    typedef Eigen::Matrix<T,3,3> matrix33;
    typedef Vertex<T>* pVertex;
    typedef Face<T>* pFace;
    typedef Edge<T>* pEdge;

public:

    //! Checks whether three vertices are coplanar
    static bool isCoplanar(pVertex v0, pVertex v1, pVertex v2);

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
    pVertex addVertex(const vector3& coords);
    pVertex addVertex(T x, T y, T z);
    //! Remove vertex
    bool removeVertex(const vector3& coords,double tolerance);
    bool removeVertex(T x,T y,T z, double tolerance);
    //! Updates the hull. The first time it is called two seed-triangles with opposite orientation will be
    //! created on which the next hull faces will be built upon.
    //! Original name: ConstructHull
    void updateHull();

    //! Returns the vertices of the hull
    const std::list<pVertex>& getVertices() const;
    //! Returns the number of vertices of the hull
    unsigned int getNVertices() const;

    //! Returns the edges of the hull
    const std::list<pEdge>& getEdges() const;
    //! Returns the number of edges of the hull
    unsigned int getNEdges() const;

    //! Returns the faces of the hull
    const std::list<pFace>& getFaces() const;
    //! Returns the number of faces of the hull
    unsigned int getNFaces() const;

    //! Returns the center of gravity of the hull.
    Eigen::Vector3d getCenter() const;

    //! Translates the hull
    void translate(T x, T y, T z) const;

    //! Translates the hull to its center
    void translateToCenter() const;

    //! Isotropic scaling of the Hull
    void scale(T factor) const;

    //! Rotate the this ConvexHull
    void rotate(const matrix33& rotation) const;

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
    std::vector<Triangle> createFaceCache(const matrix33& rotation=Eigen::Matrix3d::Identity()) const;

private:

    //! Initializes the hull. The initialization consists in defines two triangles with opposite orientations that will
    //! serve as seeds for the faces of the hull to be built later
    void initalizeHull();

    //! Finds a set of three vertices that are not coplanar
    bool findInitialVertices(pVertex& v0, pVertex& v1, pVertex& v2) const;

    //! Processes a vertex to see whether it will be a vertex of the hull or ill be discarded.
    //! Original name: addOne
    void processVertex(pVertex v);

    //! Builds a null Face (that points to no edges and vertices)
    //! Original name: makeNullFace
    pFace buildNullFace();

    //! Builds a null Edge (that points to no vertices)
    //! Original name: makeNullEdge
    pEdge buildNullEdge();

    //! Builds a new hull face from three vertices. If fold is not nullptr then its edges will be used
    //! to defie the one of the new face to build.
    //! Original name: MakeFace
    pFace buildFace(pVertex v0, pVertex v1, pVertex v2, pFace fold=nullptr);

    //! Builds a new hull face from the edges of an existing face and a new vertex.
    //! Original name: MakeConeFace
    pFace buildConeFace(pEdge e, pVertex v);

    //! Orientates a face of the hull given a reference edge and vertex
    //! Original name: makeCcw
    void orientate(pFace f, pEdge e, pVertex v);

    //! Cleans the edges of the hull that are not visible anymore
    void cleanEdges();

    //! Cleans the faces of the hull that are not visible anymore
    void cleanFaces();

    //! Cleans the vertices of the hull that are not visible anymore
    void cleanVertices();

    //! Cleans the edges, faces and vertices of the hull that are not visible anymore
    void cleanUp();

    //! Get AABB of the convex hull
    AABB<T, 3> getAABB() const;

    //! Return whether a vertex is contained in the hull
    bool constains(const Vertex<T>& v) const;

private:

    //! A boolean that indicates whether or not the two seed-triangles of the hull have been already created
    bool _initialized;
    //! The list of the vertices of the hull. This is the std implementation of the double-linked chain.
    std::list<pVertex> _vertices;
    //! The list of the edges of the hull. This is the std implementation of the double-linked chain.
    std::list<pEdge> _edges;
    //! The list of the faces of the hull. This is the std implementation of the double-linked chain.
    std::list<pFace> _faces;

};

template <typename T>
ConvexHull<T>::ConvexHull(const ConvexHull<T>& other) : _initialized(other._initialized)
{
    for (auto v : other._vertices)
        _vertices.push_back(new Vertex<T>(v->_coords));

    for (unsigned int i=0;i<other._edges.size();++i)
        _edges.push_back(new Edge<T>());

    for (unsigned int i=0;i<other._faces.size();++i)
        _faces.push_back(new Face<T>());

    typename std::list<pEdge>::iterator eit(_edges.begin());
    typename std::list<pEdge>::const_iterator oeit(other._edges.begin());
    for (unsigned int i=0;i<_edges.size();++i,++eit,++oeit)
    {
        for (int j=0;j<2;++j)
        {
            pVertex pv = (*oeit)->_endPts[j];
            auto vit = std::find(other._vertices.begin(),other._vertices.end(),pv);
            if (vit != other._vertices.end())
            {
                int dis = std::distance(other._vertices.begin(),vit);
                auto it = _vertices.begin();
                std::advance(it,dis);
                (*eit)->_endPts[j] = *it;
            }

            pFace pf = (*oeit)->_adjFace[j];
            auto fit = std::find(other._faces.begin(),other._faces.end(),pf);
            if (fit != other._faces.end())
            {
                int dis = std::distance(other._faces.begin(),fit);
                auto it = _faces.begin();
                std::advance(it,dis);
                (*eit)->_adjFace[j] = *it;
            }
        }
    }

    typename std::list<pFace>::iterator fit(_faces.begin());
    typename std::list<pFace>::const_iterator ofit(other._faces.begin());

    for (unsigned int i=0;i<_faces.size();++i,++fit,++ofit)
    {
        for (int j=0;j<3;++j)
        {
            pVertex pv = (*ofit)->_vertices[j];
            auto vit = std::find(other._vertices.begin(),other._vertices.end(),pv);
            if (vit != other._vertices.end())
            {
                int dis = std::distance(other._vertices.begin(),vit);
                auto it = _vertices.begin();
                std::advance(it,dis);
                (*fit)->_vertices[j] = *it;
            }

            pEdge pe = (*ofit)->_edges[j];
            auto eit = std::find(other._edges.begin(),other._edges.end(),pe);
            if (eit != other._edges.end())
            {
                int dis = std::distance(other._edges.begin(),eit);
                auto it = _edges.begin();
                std::advance(it,dis);
                (*fit)->_edges[j] = *it;
            }
        }
    }
}

template <typename T>
ConvexHull<T>& ConvexHull<T>::operator=(const ConvexHull<T>& other)
{
    if (this!=&other)
    {
        for (auto v : other._vertices)
            _vertices.push_back(new Vertex<T>(v->_coords));

        for (unsigned int i=0;i<other._edges.size();++i)
            _edges.push_back(new Edge<T>());

        for (unsigned int i=0;i<other._faces.size();++i)
            _faces.push_back(new Face<T>());

        typename std::list<pEdge>::iterator eit(_edges.begin());
        typename std::list<pEdge>::const_iterator oeit(other._edges.begin());

        for (unsigned int i=0;i<_edges.size();++i,++eit,++oeit)
        {
            for (int j=0;j<2;++j)
            {
                pVertex pv = (*oeit)->_endPts[j];
                auto vit = std::find(other._vertices.begin(),other._vertices.end(),pv);
                if (vit != other._vertices.end())
                {
                    int dis = std::distance(other._vertices.begin(),vit);
                    auto it = _vertices.begin();
                    std::advance(it,dis);
                    (*eit)->_endPts[j] = *it;
                }

                pFace pf = (*oeit)->_adjFace[j];
                auto fit = std::find(other._faces.begin(),other._faces.end(),pf);
                if (fit != other._faces.end())
                {
                    int dis = std::distance(other._faces.begin(),fit);
                    auto it = _faces.begin();
                    std::advance(it,dis);
                    (*eit)->_adjFace[j] = *it;
                }
            }
        }

        typename std::list<pFace>::iterator fit(_faces.begin());
        typename std::list<pFace>::const_iterator ofit(other._faces.begin());

        for (unsigned int i=0;i<_faces.size();++i,++fit,++ofit)
        {
            for (int j=0;j<3;++j)
            {
                pVertex pv = (*ofit)->_vertices[j];
                auto vit = std::find(other._vertices.begin(),other._vertices.end(),pv);
                if (vit != other._vertices.end())
                {
                    int dis = std::distance(other._vertices.begin(),vit);
                    auto it = _vertices.begin();
                    std::advance(it,dis);
                    (*fit)->_vertices[j] = *it;
                }

                pEdge pe = (*ofit)->_edges[j];
                auto eit = std::find(other._edges.begin(),other._edges.end(),pe);
                if (eit != other._edges.end())
                {
                    int dis = std::distance(other._edges.begin(),eit);
                    auto it = _edges.begin();
                    std::advance(it,dis);
                    (*fit)->_edges[j] = *it;
                }
            }
        }
    }
    return *this;
}

template <typename T>
void ConvexHull<T>::reset()
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
    _initialized=false;

}

template <typename T>
bool ConvexHull<T>::isCoplanar(pVertex v0, pVertex v1, pVertex v2)
{
    vector3 va=v1->_coords - v0->_coords;
    vector3 vb=v2->_coords - v0->_coords;

    T norm=va.cross(vb).norm();

    return (norm<1.0e-9);
}

template <typename T>
ConvexHull<T>::ConvexHull() : _initialized(false), _vertices(), _edges(), _faces()
{
}

template <typename T>
ConvexHull<T>::~ConvexHull()
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
}

template <typename T>
typename ConvexHull<T>::pVertex ConvexHull<T>::addVertex(const vector3& coords)
{

    for (const auto& v : _vertices)
    {
        if (std::abs(coords[0]-v->_coords[0])<1.0e-6 && std::abs(coords[1]-v->_coords[1])<1.0e-6 && std::abs(coords[2]-v->_coords[2])<1.0e-6)
            throw Error<ConvexHull>("Duplicate vertex (within 1.0e6 tolerance).");
    }

    pVertex v=new Vertex<T>(coords);
    _vertices.push_back(v);
    return _vertices.back();
}

template <typename T>
typename ConvexHull<T>::pVertex ConvexHull<T>::addVertex(T x,T y,T z)
{
    return addVertex(vector3(x,y,z));
}

template <typename T>
bool ConvexHull<T>::removeVertex(const vector3& coords, double tolerance)
{
    for (auto it=_vertices.begin();it!=_vertices.end();++it)
    {
        if (((*it)->_coords-coords).squaredNorm() < tolerance)
            _vertices.erase(it);
        return true;
    }
    return false;
}

template <typename T>
bool ConvexHull<T>::removeVertex(T x,T y,T z, double tolerance)
{
    return removeVertex(vector3(x,y,z),tolerance);
}

template <typename T>
typename ConvexHull<T>::pFace ConvexHull<T>::buildNullFace()
{
    pFace f=new Face<T>();
    return f;
}

template <typename T>
typename ConvexHull<T>::pEdge ConvexHull<T>::buildNullEdge()
{
    pEdge e=new Edge<T>();
    return e;
}

template <typename T>
bool ConvexHull<T>::findInitialVertices(pVertex& v0, pVertex& v1, pVertex& v2) const
{
    if (_vertices.size()<3)
        throw Error<ConvexHull>("Can not set the initial polytope with less than 3 vertices.");

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

template <typename T>
void ConvexHull<T>::initalizeHull()
{

    // Find 3 non colinear vertices
    Vertex<T> *v0(nullptr),*v1(nullptr),*v2(nullptr);
    if (!findInitialVertices(v0,v1,v2))
        throw Error<ConvexHull>("All vertices are coplanar 1.");

    // Mark the three non colinear vertices found as processed
    v0->_mark=true;
    v1->_mark=true;
    v2->_mark=true;

    // Create the two twin faces
    pFace f0=buildFace(v0,v1,v2);
    pFace f1=buildFace(v2,v1,v0,f0);

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

template <typename T>
typename ConvexHull<T>::pFace ConvexHull<T>::buildFace(pVertex v0, pVertex v1, pVertex v2, pFace fold)
{
    Edge<T> *e0(nullptr),*e1(nullptr),*e2(nullptr);

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
    pFace f=buildNullFace();
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

template <typename T>
void ConvexHull<T>::updateHull()
{

    if (!_initialized)
    {
        initalizeHull();
        _initialized=true;
    }

    if (_vertices.size()<4)
        throw Error<ConvexHull>("Not enough vertices to build a convex hull.");

    auto it=_vertices.begin();
    while(it!=_vertices.end())
    {
        if (!((*it)->_mark))
        {
            processVertex(*it);
            ++it;
            cleanUp();
            it=_vertices.begin();
        }
        else
            ++it;
    }
}

template <typename T>
void ConvexHull<T>::processVertex(pVertex v)
{
    v->_mark=true;
    // Mark the faces that are visible from vertex v
    bool visible=false;
    for (auto& f : _faces)
    {
        if (f->volumeSign(v)<0)
        {
            f->_visible=true;
            visible=true;
        }
    }

    // If no faces are visible from vertex v, then v is inside the hull
    if (!visible)
    {
        v->_onHull=false;
        return;
    }

    bool visible1, visible2;

    auto it=_edges.begin();
    while (it!=_edges.end())
    {
        pEdge e=*it;
        if (e->_adjFace[0])
            visible1=e->_adjFace[0]->_visible;
        else
            visible1=false;

        if (e->_adjFace[1])
            visible2=e->_adjFace[1]->_visible;
        else
            visible2=false;

        if (visible1 && visible2)
            e->_delete=true;
        else if (visible1 || visible2)
            e->_newFace=buildConeFace(e,v);

        ++it;
    }
}

template <typename T>
typename ConvexHull<T>::pFace ConvexHull<T>::buildConeFace(pEdge e, pVertex v)
{

    std::array<pEdge,2> newEdges;
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
    pFace newFace = buildNullFace();
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

template <typename T>
void ConvexHull<T>::orientate(pFace f, pEdge e, pVertex v)
{

    pFace fv;

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
        pEdge temp;
        temp         = f->_edges[1];
        f->_edges[1] = f->_edges[2];
        f->_edges[2] = temp;
    }


    f->_vertices[2] = v;

}

template <typename T>
const std::list<typename ConvexHull<T>::pVertex>& ConvexHull<T>::getVertices() const
{
    return _vertices;
}

template <typename T>
const std::list<typename ConvexHull<T>::pEdge>& ConvexHull<T>::getEdges() const
{
    return _edges;
}

template <typename T>
const std::list<typename ConvexHull<T>::pFace>& ConvexHull<T>::getFaces() const
{
    return _faces;
}

template <typename T>
void ConvexHull<T>::cleanEdges()
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

template <typename T>
void ConvexHull<T>::cleanFaces()
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

template <typename T>
void ConvexHull<T>::cleanVertices()
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

template <typename T>
void ConvexHull<T>::cleanUp()
{
    cleanEdges();
    cleanFaces();
    cleanVertices();
}

template<typename T>
AABB<T, 3> ConvexHull<T>::getAABB() const
{
    using vector = typename AABB<T,3>::vector;
    vector lower, upper;
    lower.setZero();
    upper.setZero();

    for (auto&& v: getVertices()) {
        for (auto i = 0; i < 3; ++i) {
            if ( (*v)(i) < lower(i)) {
                lower(i) = (*v)(i);
            }
            if ( (*v)(i) > upper(i)) {
                upper(i) = (*v)(i);
            }
        }
    }

    return AABB<T, 3>(lower, upper);
}

template <typename T>
Eigen::Vector3d ConvexHull<T>::getCenter() const
{
    Eigen::Vector3d center=Eigen::Vector3d::Zero();
    for (auto & v : _vertices)
        center += v->_coords;
    center/=_vertices.size();

    return center;
}

template <typename T>
void ConvexHull<T>::translate(T x, T y, T z) const
{
    for (auto& v : _vertices)
    {
        v->_coords[0] += x;
        v->_coords[1] += y;
        v->_coords[2] += z;
    }
}

template <typename T>
void ConvexHull<T>::scale(T factor) const
{
    for (auto& v : _vertices)
    {
        v->_coords*=factor;
    }
}

template <typename T>
void ConvexHull<T>::rotate(const matrix33& rotation) const
{
    for (auto& v : _vertices)
        v->_coords = rotation*v->_coords;
}

template <typename T>
void ConvexHull<T>::translateToCenter() const
{

    Eigen::Vector3d center=-getCenter();
    translate(center[0],center[1],center[2]);
}

template <typename T>
double ConvexHull<T>::getVolume() const
{

    Eigen::Vector3d center=getCenter();

    double volume=0.0;
    for (auto& f : _faces)
        volume += f->volume(center);

    return volume;

}

template<typename T>
unsigned int ConvexHull<T>::getNVertices() const
{
    return _vertices.size();
}

template<typename T>
unsigned int ConvexHull<T>::getNEdges() const
{
    return _edges.size();
}

template<typename T>
unsigned int ConvexHull<T>::getNFaces() const
{
    return _faces.size();
}

template<typename T>
bool ConvexHull<T>::checkEulerConditions() const
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

template<typename T>
std::vector<Triangle > ConvexHull<T>::createFaceCache(const matrix33& rotation) const
{
    if (_vertices.size()<4)
        throw Error<ConvexHull<T> >("Hull is flat or undefined, can not construct faces information");

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

template<typename T>
void ConvexHull<T>::print(std::ostream& os) const
{
    os<<"Convex Hull:\n"<<std::endl;
    for (auto v : _vertices)
        os<< *v << std::endl;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const ConvexHull<T>& chull)
{
    chull.print(os);
    return os;
}

} // end namespace nsx

#endif // NSXLIB_CONVEXHULL_H
