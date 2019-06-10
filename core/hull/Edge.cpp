//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/hull/Edge.cpp
//! @brief     Implements struct Edge
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/hull/Edge.h"
#include "core/hull/Vertex.h"

namespace nsx {

Edge::Edge() : _id(-1), _newFace(nullptr), _delete(false)
{
    _faces.resize(2, nullptr);
    _vertices.resize(2, nullptr);
}

Edge::Edge(int id) : _id(id), _faces(), _vertices(), _newFace(nullptr), _delete(false)
{
    _faces.resize(2, nullptr);
    _vertices.resize(2, nullptr);
}

void Edge::print(std::ostream& os) const
{
    os << "Edge: ";
    for (auto it = _vertices.begin(); it != _vertices.end(); ++it) {
        if (*it)
            os << "     " << **it;
        else
            os << " NULL ";
    }
}

std::ostream& operator<<(std::ostream& os, const Edge& edge)
{
    edge.print(os);
    return os;
}

} // namespace nsx
