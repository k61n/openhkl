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

namespace SX {

namespace Geometry
{

template <typename T>
class Edge;

template<typename T>
class Vertex
{
	typedef Eigen::Matrix<T,3,1> vector3;

public:

	Vertex();

	Vertex(const Vertex& other);

	Vertex(const vector3& coords);

	~Vertex();

	Vertex& operator=(const Vertex& other);

	void print(std::ostream& os) const;

public:

	vector3 _coords;
	Edge<T>* _duplicate;
	bool _onHull;
	bool _mark;
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
Vertex<T>::Vertex(const Vertex<T>& other) : _coords(other._coords), _duplicate(other._duplicate), _onHull(other._onHull), _mark(other._mark)
{
}

template <typename T>
Vertex<T>& Vertex<T>::operator=(const Vertex<T>& other)
{
	if (this!=&other)
	{
		_coords=other._coords;
		_duplicate=other._duplicate;
		_onHull=other._onHull;
		_mark=other._mark;

	}
	return *this;
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

} // end namespace SX

#endif /* NSXTOOL_VERTEX_H_ */
