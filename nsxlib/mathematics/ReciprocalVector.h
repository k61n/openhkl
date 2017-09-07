#pragma once

#include <Eigen/Dense>

namespace nsx {

class ReciprocalMatrix;

class ReciprocalVector : public Eigen::RowVector3d
{

public:

    using Eigen::RowVector3d::RowVector3d;

    ~ReciprocalVector()=default;

    ReciprocalVector& operator=(const Eigen::RowVector3d& rvector);

    ReciprocalVector& operator=(const ReciprocalVector& other);

    ReciprocalVector& operator+=(const Eigen::RowVector3d& other)=delete;

    ReciprocalVector& operator+=(const ReciprocalVector& other);

    ReciprocalVector operator+(const Eigen::RowVector3d& other)=delete;

    ReciprocalVector operator+(const ReciprocalVector& other);

    ReciprocalVector& operator*=(double factor);

    ReciprocalVector& operator*=(const ReciprocalMatrix& B);

};

} // end namespace nsx
