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

#include "Vertex.h"

namespace nsx {

Vertex::Vertex()
    : _id(-1), _coords(), _duplicate(nullptr), _onHull(false), _mark(false) {}

Vertex::Vertex(int id)
    : _id(id), _coords(), _duplicate(nullptr), _onHull(false), _mark(false) {}

Vertex::Vertex(int id, const Eigen::Vector3d &coords)
    : _id(id), _coords(coords), _duplicate(nullptr), _onHull(false),
      _mark(false) {}

void Vertex::print(std::ostream &os) const {
  os << "Vertex @ " << _coords.transpose();
}

std::ostream &operator<<(std::ostream &os, const Vertex &vertex) {
  vertex.print(os);
  return os;
}

} // end namespace nsx
