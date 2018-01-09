#include <Eigen/Dense>

#include "ReciprocalVector.h"

namespace nsx {

ReciprocalVector::ReciprocalVector(const Eigen::RowVector3d& rvector) : _rvector(rvector)
{
}

const Eigen::RowVector3d& ReciprocalVector::rowVector() const
{
    return _rvector;
}

double ReciprocalVector::operator[](int index) const
{
    if (index < 0 || index > 2) {
        throw std::runtime_error("Invalid index for a 3D vector");
    }
    return _rvector[index];
}

double& ReciprocalVector::operator[](int index)
{
    if (index < 0 || index > 2) {
        throw std::runtime_error("Invalid index for a 3D vector");
    }
    return _rvector[index];
}

double ReciprocalVector::operator()(int index) const
{
    if (index < 0 || index > 2) {
        throw std::runtime_error("Invalid index for a 3D vector");
    }
    return _rvector[index];
}

double& ReciprocalVector::operator()(int index)
{
    if (index < 0 || index > 2) {
        throw std::runtime_error("Invalid index for a 3D vector");
    }
    return _rvector[index];
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
