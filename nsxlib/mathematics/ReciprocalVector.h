#pragma once

#include <Eigen/Dense>

namespace nsx {

class ReciprocalVector : public Eigen::RowVector3d
{

public:

    ReciprocalVector();

    ReciprocalVector(const Eigen::RowVector3d& vector);

    ReciprocalVector(double x, double y, double z);

    virtual ~ReciprocalVector();

    ReciprocalVector& operator=(const ReciprocalVector& other);

    ReciprocalVector& operator+=(const Eigen::RowVector3d& other)=delete;

    ReciprocalVector& operator+=(const ReciprocalVector& other);

    ReciprocalVector operator+(const Eigen::RowVector3d& other)=delete;

    ReciprocalVector operator+(const ReciprocalVector& other);

};

} // end namespace nsx
