#include <Eigen/Dense>

#include "DirectMatrix.h"
#include "DirectVector.h"

namespace nsx {

DirectMatrix::DirectMatrix(const Eigen::Vector3d& col1, const Eigen::Vector3d& col2, const Eigen::Vector3d& col3)
{
    this->col(0) = col1;
    this->col(1) = col2;
    this->col(2) = col3;
}

DirectMatrix& DirectMatrix::operator=(const DirectMatrix& other)
{
    this->Eigen::Matrix3d::operator=(other);
    return *this;
}

DirectMatrix& DirectMatrix::operator+=(const DirectMatrix& other)
{
    this->Eigen::Matrix3d::operator+=(other);
    return *this;
}

DirectMatrix DirectMatrix::operator+(const DirectMatrix& other)
{
    DirectMatrix result = *this;
    result += other;
    return result;
}

DirectVector DirectMatrix::operator*(const DirectVector& vector)
{
    return (DirectVector)this->Eigen::Matrix3d::operator*(vector);
}

} // end namespace nsx
