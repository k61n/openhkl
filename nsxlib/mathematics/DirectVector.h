#pragma once

#include <Eigen/Dense>

#include "MathematicsTypes.h"

namespace nsx {

class DirectVector : public Eigen::Vector3d
{

public:

    using Eigen::Vector3d::Vector3d;

    ~DirectVector()=default;

    DirectVector& operator=(const Eigen::Vector3d& other);

    DirectVector& operator=(const DirectVector& other);

    DirectVector& operator+=(const Eigen::Vector3d& other)=delete;

    DirectVector& operator+=(const DirectVector& other);

    DirectVector operator+(const Eigen::Vector3d& other)=delete;

    DirectVector operator+(const DirectVector& other);
};

} // end namespace nsx
