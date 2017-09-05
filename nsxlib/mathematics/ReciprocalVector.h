#pragma once

#include <Eigen/Dense>

namespace nsx {

class ReciprocalVector : public Eigen::RowVector3d
{

public:

    using Eigen::RowVector3d::RowVector3d;

    virtual ~ReciprocalVector();

    ReciprocalVector& operator=(const ReciprocalVector& other);

    ReciprocalVector& operator+=(const Eigen::RowVector3d& other)=delete;

    ReciprocalVector& operator+=(const ReciprocalVector& other);

    ReciprocalVector operator+(const Eigen::RowVector3d& other)=delete;

    ReciprocalVector operator+(const ReciprocalVector& other);

};

} // end namespace nsx
