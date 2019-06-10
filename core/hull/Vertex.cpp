//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/hull/Vertex.cpp
//! @brief     Implements ###THINGS###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/hull/Vertex.h"

namespace nsx {

Vertex::Vertex() : _id(-1), _coords(), _duplicate(nullptr), _onHull(false), _mark(false) {}

Vertex::Vertex(int id) : _id(id), _coords(), _duplicate(nullptr), _onHull(false), _mark(false) {}

Vertex::Vertex(int id, const Eigen::Vector3d& coords)
    : _id(id), _coords(coords), _duplicate(nullptr), _onHull(false), _mark(false)
{
}

void Vertex::print(std::ostream& os) const
{
    os << "Vertex @ " << _coords.transpose();
}

std::ostream& operator<<(std::ostream& os, const Vertex& vertex)
{
    vertex.print(os);
    return os;
}

} // namespace nsx
