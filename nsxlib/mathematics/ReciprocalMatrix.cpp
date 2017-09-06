#include <Eigen/Dense>

#include "ReciprocalMatrix.h"

namespace nsx {

ReciprocalMatrix::ReciprocalMatrix() : Eigen::Matrix3d()
{
}

ReciprocalMatrix::ReciprocalMatrix(const ReciprocalMatrix& other) : Eigen::Matrix3d(other)
{
}

ReciprocalMatrix::ReciprocalMatrix(const Eigen::Matrix3d& matrix) : Eigen::Matrix3d(matrix)
{
}

ReciprocalMatrix::ReciprocalMatrix(const Eigen::RowVector3d& row1, const Eigen::RowVector3d& row2, const Eigen::RowVector3d& row3)
{
    this->row(0) = row1;
    this->row(1) = row2;
    this->row(2) = row3;
}

ReciprocalMatrix::~ReciprocalMatrix()
{
}

ReciprocalMatrix& ReciprocalMatrix::operator=(const ReciprocalMatrix& other)
{
    this->Eigen::Matrix3d::operator=(other);
    return *this;
}

ReciprocalMatrix& ReciprocalMatrix::operator=(const Eigen::Matrix3d& matrix)
{
    this->Eigen::Matrix3d::operator=(matrix);
    return *this;
}

ReciprocalMatrix& ReciprocalMatrix::operator+=(const ReciprocalMatrix& other)
{
    this->Eigen::Matrix3d::operator+=(other);
    return *this;
}

ReciprocalMatrix ReciprocalMatrix::operator+(const ReciprocalMatrix& other)
{
    ReciprocalMatrix result = *this;
    result += other;
    return result;
}

} // end namespace nsx
