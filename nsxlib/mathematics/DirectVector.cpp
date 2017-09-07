#include <Eigen/Dense>

#include "DirectVector.h"

namespace nsx {

DirectVector& DirectVector::operator=(const Eigen::Vector3d& other)
{
    this->Eigen::Vector3d::operator=(other);
    return *this;
}

DirectVector& DirectVector::operator=(const DirectVector& other)
{
    this->Eigen::Vector3d::operator=(other);
    return *this;
}

DirectVector& DirectVector::operator+=(const DirectVector& other)
{
    this->Eigen::Vector3d::operator+=(other);
    return *this;
}

DirectVector DirectVector::operator+(const DirectVector& other)
{
    DirectVector result = *this;
    result += other;
    return result;
}

} // end namespace nsx
