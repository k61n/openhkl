#include <Eigen/Dense>

#include "ReciprocalMatrix.h"
#include "ReciprocalVector.h"

namespace nsx {

ReciprocalVector& ReciprocalVector::operator=(const Eigen::RowVector3d& other)
{
    this->Eigen::RowVector3d::operator=(other);
    return *this;
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

ReciprocalVector& ReciprocalVector::operator*=(double factor)
{
    this->Eigen::RowVector3d::operator*=(factor);
    return *this;
}

ReciprocalVector& ReciprocalVector::operator*=(const ReciprocalMatrix& B)
{
    this->Eigen::RowVector3d::operator*=(B);
    return *this;
}

} // end namespace nsx
