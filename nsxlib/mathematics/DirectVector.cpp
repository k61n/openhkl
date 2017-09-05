#pragma once

#include <Eigen/Dense>

#include "DirectVector.h"

namespace nsx {

DirectVector::DirectVector() : Eigen::Vector3d()
{
}

DirectVector::DirectVector(const Eigen::Vector3d& vector) : Eigen::Vector3d(vector)
{
}

DirectVector::DirectVector(double x, double y, double z) : Eigen::Vector3d(x,y,z)
{
}

DirectVector::~DirectVector()
{
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
