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

#ifndef NSXTOOL_FACE_H_
#define NSXTOOL_FACE_H_

#include <array>
#include <ostream>

#include <Eigen/Dense>

#include "Edge.h"

namespace nsx {

// Forward declarations
template <typename T>
class Vertex;

template <typename T>
class Edge;

/* !
 * \brief Class Face.
 * This class implements the Face object used in the incremental convex hull algorithm.
 */
template <typename T>
class Face {

public:

	//! Default constructor
	Face();

	//! Copy constructor
	Face(const Face& other)=delete;

	//! Destructor
	~Face();

	//! Assignment operator
	Face& operator=(const Face& other)=delete;

	//! Returns the int signed volume of the tetrahedron formed by this Face and a vertex.
	//! It is equal respectively to 1 and -1  if the vertex is on the negative or positive side of this Face
	//! with the positive side determined by the right hand rule. It is equal to 0 if the vertice is within the face
	int volumeSign(Vertex<T>* v) const;

	//! Returns the volume of the tetrahedron formed by this Face and any point.
	double volume(const Eigen::Vector3d& pos) const;

	//! Send some informations about this Face on an output stream
	void print(std::ostream& os) const;

public:
	//! The three pointer to the edges of this Face
	std::array<Edge<T>*,3> _edges;
	//! The three pointers to the vertices of this Face
	std::array<Vertex<T>*,3> _vertices;
	//! Indicates whether or not this Face is visible
	bool _visible;
};

template <typename T>
Face<T>::Face() : _edges(), _vertices(), _visible(false)
{
	_edges.fill(nullptr);
	_vertices.fill(nullptr);
}

template <typename T>
Face<T>::~Face()
{
}

template <typename T>
double Face<T>::volume(const Eigen::Vector3d& pos) const
{
	Eigen::Matrix<T,3,3> mat;

	mat.row(0)=_vertices[0]->_coords - pos;
	mat.row(1)=_vertices[1]->_coords - pos;
	mat.row(2)=_vertices[2]->_coords - pos;

	T det=mat.determinant();

	return std::abs(det)/6.0;
}

template <typename T>
int Face<T>::volumeSign(Vertex<T>* v) const
{
	Eigen::Matrix<T,3,3> mat;

	mat.row(0)=_vertices[0]->_coords - v->_coords;
	mat.row(1)=_vertices[1]->_coords - v->_coords;
	mat.row(2)=_vertices[2]->_coords - v->_coords;

	T det=mat.determinant();

	if (std::abs(det)<1.0e-9)
		return 0;
	else
		return (det > 0) ? 1 : -1;
}

template<typename T>
void Face<T>::print(std::ostream& os) const
{
	os<<"Face:"<<std::endl;
	for (auto it=_edges.begin();it!=_edges.end();++it)
		if (*it)
			os<<**it;
		else
			os<<" NULL ";
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Face<T>& face)
{
	face.print(os);
	return os;
}

} // end namespace nsx

#endif /* NSXTOOL_FACE_H_ */
