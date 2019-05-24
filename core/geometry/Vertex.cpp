#include "Vertex.h"

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

} // end namespace nsx
