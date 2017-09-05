#pragma once

#include <Eigen/Dense>

#include "ReciprocalVector.h"

namespace nsx {

ReciprocalVector::ReciprocalVector() : Eigen::RowVector3d()
{
}

ReciprocalVector::ReciprocalVector(const Eigen::RowVector3d& vector) : Eigen::RowVector3d(vector)
{
}

ReciprocalVector::ReciprocalVector(double x, double y, double z) : Eigen::RowVector3d(x,y,z)
{
}

ReciprocalVector::~ReciprocalVector()
{
}

ReciprocalVector& ReciprocalVector::operator=(const ReciprocalVector& other)
{
    this->Eigen::RowVector3d::operator=(other);
    return *this;
}

ReciprocalVector& ReciprocalVector::operator+=(const ReciprocalVector& other)
{
    this->Eigen::RowVector3d::operator+=(other);
    return *this;
}

ReciprocalVector ReciprocalVector::operator+(const ReciprocalVector& other)
{
    ReciprocalVector result = *this;
    result += other;
    return result;
}

} // end namespace nsx
