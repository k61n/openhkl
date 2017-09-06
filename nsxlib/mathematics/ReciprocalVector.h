#pragma once

#include <Eigen/Dense>

namespace nsx {

class ReciprocalMatrix;

class ReciprocalVector : public Eigen::RowVector3d
{

public:

    ReciprocalVector();

    ReciprocalVector(const ReciprocalVector& other);

    ReciprocalVector(const Eigen::RowVector3d& rvector);

    ReciprocalVector(double rx, double  ry, double rz);

    virtual ~ReciprocalVector();

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
