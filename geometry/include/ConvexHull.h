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

#ifndef NSXTOOL_CONVEXHULL_H_
#define NSXTOOL_CONVEXHULL_H_

#include <array>
#include <cmath>
#include <list>

#include <Eigen/Dense>

#include "Face.h"
#include "Triangle.h"
#include "Error.h"

//

namespace SX
{

namespace Geometry
{

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

	//! Adds a new vertex to the list of points to be processed later when calling updateHull method.
	pVertex addVertex(const vector3& coords);

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

	//! Returns the volume of the hull. The volume is computed by summing the volumes of all the tethrahedrons
	//! made by each face of the convex hull and any internal point of the hull.
	//! A reasonable choice for the internal point is the center of gravity of the hull as, by definition of a
	//! convex object, its center is within the hull.
	double getVolume() const;

	//! Sends some informations on an output stream.
	void print(std::ostream& os) const;

	//!

	std::vector<Triangle> createFaceCache() const;

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

	//! Orienttates a face of the hull given a reference edge and vertex
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
	for (auto& v : _vertices)
		delete v;

	for (auto& e : _edges)
		delete e;

	for (auto& f : _faces)
		delete f;
}

template <typename T>
typename ConvexHull<T>::pVertex ConvexHull<T>::addVertex(const vector3& coords)
{
	pVertex v=new Vertex<T>(coords);
	_vertices.push_back(v);
	return _vertices.back();
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
		throw SX::Kernel::Error<ConvexHull>("Can not set the initial polytope with less than 3 vertices.");

	for (auto it1=_vertices.begin();it1!=_vertices.end();++it1)
	{
		v0=*it1;
		for (auto it2=std::next(it1);it2!=_vertices.end();++it2)
		{
			v1=*it2;
			for (auto it3=std::next(it2);it3!=_vertices.end();++it3)
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
		throw SX::Kernel::Error<ConvexHull>("All vertices are coplanar 1.");

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
	for (auto& v : _vertices)
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
		throw SX::Kernel::Error<ConvexHull>("Not enough vertices to build a convex hull.");

	for (auto rit=_vertices.rbegin();rit!=_vertices.rend();rit++)
	{
		if (!((*rit)->_mark))
		{
			processVertex(*rit);
			cleanUp();
		}
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

	for (auto rit=_edges.end();rit!=_edges.begin();)
	{
		--rit;
		pEdge e=*rit;
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
		else
			continue;

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
	for (idx=0;fv->_vertices[idx]!=e->_endPts[0];++idx);

//	std::cout <<"INDEX="<<idx << std::endl;

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

	for (auto rit=_edges.rbegin();rit!=_edges.rend();++rit)
	{
		if ((*rit)->_delete)
		{
			delete *rit;
			_edges.erase(--(rit.base()));
		}
	}
}

template <typename T>
void ConvexHull<T>::cleanFaces()
{
	for (auto rit=_faces.rbegin();rit!=_faces.rend();++rit)
	{
		if ((*rit)->_visible)
		{
			delete *rit;
			_faces.erase(--(rit.base()));
		}
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

	for (auto rit=_vertices.rbegin();rit!=_vertices.rend();++rit)
	{
		if ((*rit)->_mark && !(*rit)->_onHull)
		{
			delete *rit;
			_vertices.erase(--(rit.base()));
		}
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
std::vector<Triangle > ConvexHull<T>::createFaceCache() const
{
	if (_vertices.size()<4)
		throw SX::Kernel::Error<ConvexHull<T> >("Hull is flat or undefined, can not construct faces information");

	std::vector<Triangle > triangles;
	triangles.reserve(_faces.size());
	for (const auto& f: _faces)
	{
		triangles.push_back(Triangle(f->_vertices[0]->_coords,f->_vertices[1]->_coords,f->_vertices[2]->_coords));
	}
	return triangles;
}

template<typename T>
void ConvexHull<T>::print(std::ostream& os) const
{
	os<<"Convex Hull:\n"<<std::endl;
	for (auto& f : _faces)
		os<<f;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const ConvexHull<T>& chull)
{
	chull.print(os);
	return os;
}

} // end namespace Geometry

} // end namespace SX

#endif /* NSXTOOL_CONVEXHULL_H_ */
