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

#include "Edge.h"
#include "Vertex.h"

namespace nsx {

Edge::Edge() : _id(-1), _newFace(nullptr), _delete(false)
{
    _faces.resize(2,nullptr);
    _vertices.resize(2,nullptr);
}

Edge::Edge(int id) : _id(id), _faces(), _vertices(), _newFace(nullptr), _delete(false)
{
    _faces.resize(2,nullptr);
    _vertices.resize(2,nullptr);
}

void Edge::print(std::ostream& os) const
{
	os<<"Edge: ";
	for (auto it = _vertices.begin(); it != _vertices.end();++it)
	{
		if (*it)
			os<<"     "<<**it;
		else
			os<<" NULL ";
	}
}

std::ostream& operator<<(std::ostream& os, const Edge& edge)
{
	edge.print(os);
	return os;
}

} // end namespace nsx

