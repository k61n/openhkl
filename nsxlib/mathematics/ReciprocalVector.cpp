#include <Eigen/Dense>

#include "ReciprocalVector.h"

namespace nsx {

ReciprocalVector::ReciprocalVector(const Eigen::RowVector3d& rvector) : _rvector(rvector)
{
}

ReciprocalVector::operator const Eigen::RowVector3d& () const
{
    return _rvector;
}

void ReciprocalVector::print(std::ostream& os) const
{
    os << _rvector;
}

std::ostream& operator<<(std::ostream& os, const ReciprocalVector& rvector)
{
    rvector.print(os);
    return os;
}

} // end namespace nsx
