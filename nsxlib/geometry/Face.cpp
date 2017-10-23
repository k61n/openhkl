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

#include <array>

#include <Eigen/Dense>

#include "Edge.h"
#include "Face.h"
#include "Vertex.h"

namespace nsx {

Face::Face() : _edges(), _vertices(), _visible(false)
{
	_edges.fill(nullptr);
	_vertices.fill(nullptr);
}

double Face::volume(const Eigen::Vector3d& pos) const
{
	Eigen::Matrix3d mat;

	mat.row(0)=_vertices[0]->_coords - pos;
	mat.row(1)=_vertices[1]->_coords - pos;
	mat.row(2)=_vertices[2]->_coords - pos;

	double det=mat.determinant();

	return std::abs(det)/6.0;
}

int Face::volumeSign(Vertex* v) const
{
	Eigen::Matrix3d mat;

	mat.row(0)=_vertices[0]->_coords - v->_coords;
	mat.row(1)=_vertices[1]->_coords - v->_coords;
	mat.row(2)=_vertices[2]->_coords - v->_coords;

	double det=mat.determinant();

	if (std::abs(det)<1.0e-9)
		return 0;
	else
		return (det > 0) ? 1 : -1;
}

void Face::print(std::ostream& os) const
{
	os<<"Face:"<<std::endl;
	for (auto it=_edges.begin();it!=_edges.end();++it)
		if (*it)
			os<<**it;
		else
			os<<" NULL ";
}

std::ostream& operator<<(std::ostream& os, const Face& face)
{
	face.print(os);
	return os;
}

} // end namespace nsx

