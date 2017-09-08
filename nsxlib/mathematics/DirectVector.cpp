#include <Eigen/Dense>

#include "DirectVector.h"

namespace nsx {

DirectVector::DirectVector(const Eigen::Vector3d& dvector) : _dvector(dvector)
{
}

DirectVector::operator const Eigen::Vector3d& () const
{
    return _dvector;
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
