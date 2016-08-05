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

#ifndef NSXTOOL_EDGE_H_
#define NSXTOOL_EDGE_H_

#include <array>

#include "Vertex.h"

namespace SX
{

namespace Geometry
{

// Forward declaration
template <typename T>
class Face;

/* !
 * \brief Class Edge.
 * This class implements the Edge object used in the incremental convex hull algorithm.
 */
template <typename T>
class Edge
{
public:

	//! Default constructor
	Edge();

	//! Copy constructor
	Edge(const Edge& other)=delete;

	//! Destructor
	~Edge();

	//! Assignment operator
	Edge& operator=(const Edge& other)=delete;

	//! Send some information about this Edge on an output stream
	void print(std::ostream& os) const;

public:
	//! The two pointers to the faces adjacent to this Edge
	std::array<Face<T>*,2> _adjFace;
	//! The two pointers to the vertices that makes this Edge
	std::array<Vertex<T>*,2> _endPts;
	//! When not null indicates the new face formed by this Edge and a new vertex of the hull
	Face<T>* _newFace;
	//! If true this Edge is marked to be deleted at the next clean up step
	bool _delete;
};

template <typename T>
Edge<T>::Edge() : _adjFace(), _endPts(), _newFace(nullptr), _delete(false)
{
	_adjFace.fill(nullptr);
	_endPts.fill(nullptr);
}

template <typename T>
Edge<T>::~Edge()
{
}

template <typename T>
void Edge<T>::print(std::ostream& os) const
{
	os<<"Edge: ";
	for (auto it=_endPts.begin();it!=_endPts.end();++it)
	{
		if (*it)
			os<<"     "<<**it;
		else
			os<<" NULL ";
	}
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Edge<T>& edge)
{
	edge.print(os);
	return os;
}

} // end namespace Geometry

} // end namespace SX

#endif /* NSXTOOL_EDGE_H_ */
