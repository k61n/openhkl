#pragma once

#include <iostream>

#include <Eigen/Dense>

namespace nsx {

class ReciprocalVector
{
public:

    ReciprocalVector()=default;

    ReciprocalVector(const ReciprocalVector& other)=default;

    explicit ReciprocalVector(const Eigen::RowVector3d& rvector);

    ReciprocalVector& operator=(const ReciprocalVector& other)=default;

    ~ReciprocalVector()=default;

    explicit operator const Eigen::RowVector3d& () const;

    void print(std::ostream& os) const;

private:
    Eigen::RowVector3d _rvector;
};

std::ostream& operator<<(std::ostream& os, const ReciprocalVector& rvector);

} // end namespace nsx
