#pragma once

#include <Eigen/Dense>

namespace nsx {

class ReciprocalMatrix : public Eigen::Matrix3d
{

public:

    using Eigen::Matrix3d::Matrix3d;

    ReciprocalMatrix(const Eigen::RowVector3d& row1, const Eigen::RowVector3d& row2, const Eigen::RowVector3d& row3);

    ~ReciprocalMatrix()=default;

    ReciprocalMatrix& operator=(const Eigen::Matrix3d& matrix);

    ReciprocalMatrix& operator=(const ReciprocalMatrix& other);

    ReciprocalMatrix& operator+=(const Eigen::Matrix3d& other)=delete;

    ReciprocalMatrix& operator+=(const ReciprocalMatrix& other);

    ReciprocalMatrix operator+(const Eigen::Matrix3d& other)=delete;

    ReciprocalMatrix operator+(const ReciprocalMatrix& other);
};

} // end namespace nsx
