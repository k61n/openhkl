#include <stdexcept>

#include <Eigen/Dense>

#include "DirectVector.h"

namespace nsx {

DirectVector::DirectVector(double x, double y, double z) : _dvector(x, y, z) {}

DirectVector::DirectVector(const Eigen::Vector3d& dvector) : _dvector(dvector) {}

const Eigen::Vector3d& DirectVector::vector() const
{
    return _dvector;
}

double DirectVector::operator[](int index) const
{
    if (index < 0 || index > 2) {
        throw std::runtime_error("Invalid index for a 3D vector");
    }
    return _dvector[index];
}

double& DirectVector::operator[](int index)
{
    if (index < 0 || index > 2) {
        throw std::runtime_error("Invalid index for a 3D vector");
    }
    return _dvector[index];
}

double DirectVector::operator()(int index) const
{
    if (index < 0 || index > 2) {
        throw std::runtime_error("Invalid index for a 3D vector");
    }
    return _dvector[index];
}

double& DirectVector::operator()(int index)
{
    if (index < 0 || index > 2) {
        throw std::runtime_error("Invalid index for a 3D vector");
    }
    return _dvector[index];
}

void DirectVector::print(std::ostream& os) const
{
    os << _dvector;
}

std::ostream& operator<<(std::ostream& os, const DirectVector& dvector)
{
    dvector.print(os);
    return os;
}

} // end namespace nsx
