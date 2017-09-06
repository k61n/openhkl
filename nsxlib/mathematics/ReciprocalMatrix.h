#pragma once

#include <Eigen/Dense>

namespace nsx {

class ReciprocalMatrix : public Eigen::Matrix3d
{

public:

    ReciprocalMatrix();

    ReciprocalMatrix(const ReciprocalMatrix& other);

    ReciprocalMatrix(const Eigen::Matrix3d& matrix);

    ReciprocalMatrix(const Eigen::RowVector3d& row1, const Eigen::RowVector3d& row2, const Eigen::RowVector3d& row3);

    virtual ~ReciprocalMatrix();

    ReciprocalMatrix& operator=(const Eigen::Matrix3d& matrix);

    ReciprocalMatrix& operator=(const ReciprocalMatrix& other);

    ReciprocalMatrix& operator+=(const Eigen::Matrix3d& other)=delete;

    ReciprocalMatrix& operator+=(const ReciprocalMatrix& other);

    ReciprocalMatrix operator+(const Eigen::Matrix3d& other)=delete;

    ReciprocalMatrix operator+(const ReciprocalMatrix& other);
};

} // end namespace nsx
