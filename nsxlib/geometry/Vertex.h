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

#ifndef NSXTOOL_VERTEX_H_
#define NSXTOOL_VERTEX_H_

#include <Eigen/Dense>

#include <ostream>

namespace nsx
{

namespace Geometry
{

// Forward declaration
template <typename T>
class Edge;

/* !
 * \brief Class Vertex.
 * This class implements the Vertex object used in the incremental convex hull algorithm.
 */
template<typename T>
class Vertex
{

public:

	// Typedefs
	typedef Eigen::Matrix<T,3,1> vector3;

public:

	//! Default constructor
	Vertex();

	//! Copy constructor
	Vertex(const Vertex& other)=delete;

	//! Constructs a Vertex object from a vector of coordinates
	Vertex(const vector3& coords);

	//! Destructor
	~Vertex();

	//! Assignment operator
	Vertex& operator=(const Vertex& other)=delete;

	//! Send some informations about this Vertex on an output stream
	void print(std::ostream& os) const;

public:

	//! The coordinates of this Vertex
	vector3 _coords;
	//! A pointer to the incident cone edge (or nullptr)
	Edge<T>* _duplicate;
	//! True if this Vertex is on the hull
	bool _onHull;
	//! True if the vertex has been processed
	bool _mark;

private:
};

template <typename T>
Vertex<T>::Vertex() : _coords(), _duplicate(nullptr), _onHull(false), _mark(false)
{
}

template <typename T>
Vertex<T>::Vertex(const vector3& coords) : _coords(coords), _duplicate(nullptr), _onHull(false), _mark(false)
{
}

template <typename T>
Vertex<T>::~Vertex()
{
}

template <typename T>
void Vertex<T>::print(std::ostream& os) const
{
	os<<"Vertex @ "<<_coords.transpose();
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Vertex<T>& vertex)
{
	vertex.print(os);
	return os;
}

} // end namespace Geometry

} // end namespace nsx

#endif /* NSXTOOL_VERTEX_H_ */
