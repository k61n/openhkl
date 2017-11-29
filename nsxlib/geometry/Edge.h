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

#pragma once

#include <array>

#include "GeometryTypes.h"

namespace nsx {

/* !
 * \brief Class Edge.
 * This class implements the Edge object used in the incremental convex hull algorithm.
 */
class Edge {
public:

	//! Default constructor
	Edge();

	//! Copy constructor
	Edge(const Edge& other)=delete;

	//! Destructor
	~Edge()=default;

	//! Assignment operator
	Edge& operator=(const Edge& other)=delete;

	//! Send some information about this Edge on an output stream
	void print(std::ostream& os) const;

public:
	//! The two pointers to the faces adjacent to this Edge
	std::array<Face*,2> _adjFace;
	//! The two pointers to the vertices that makes this Edge
	std::array<Vertex*,2> _endPts;
	//! When not null indicates the new face formed by this Edge and a new vertex of the hull
	Face* _newFace;
	//! If true this Edge is marked to be deleted at the next clean up step
	bool _delete;
};

std::ostream& operator<<(std::ostream& os, const Edge& edge);

} // end namespace nsx
