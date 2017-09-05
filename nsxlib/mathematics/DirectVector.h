#pragma once

#include <Eigen/Dense>

namespace nsx {

class DirectVector : public Eigen::Vector3d
{

public:

    using Eigen::Vector3d::Vector3d;

    virtual ~DirectVector();

    DirectVector& operator=(const DirectVector& other);

    DirectVector& operator+=(const Eigen::Vector3d& other)=delete;

    DirectVector& operator+=(const DirectVector& other);

    DirectVector operator+(const Eigen::Vector3d& other)=delete;

    DirectVector operator+(const DirectVector& other);
};

} // end namespace nsx
