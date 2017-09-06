#include <Eigen/Dense>

#include "DirectVector.h"

namespace nsx {

DirectVector::DirectVector() : Eigen::Vector3d()
{
}

DirectVector::DirectVector(const DirectVector& other) : Eigen::Vector3d(other)
{
}

DirectVector::DirectVector(const Eigen::Vector3d& other) : Eigen::Vector3d(other)
{
}

DirectVector::DirectVector(double x, double y, double z) : Eigen::Vector3d(x,y,z)
{
}

DirectVector::~DirectVector()
{
}

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
