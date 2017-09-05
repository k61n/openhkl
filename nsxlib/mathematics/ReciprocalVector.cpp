#pragma once

#include <Eigen/Dense>

#include "ReciprocalVector.h"

namespace nsx {

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
