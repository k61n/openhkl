#pragma once

#include <Eigen/Dense>

#include "MathematicsTypes.h"

namespace nsx {

class DirectVector : public Eigen::Vector3d
{

public:

    DirectVector();

    DirectVector(const DirectVector& other);

    DirectVector(const Eigen::Vector3d& other);

    DirectVector(double x, double  y, double z);

    virtual ~DirectVector();

    DirectVector& operator=(const Eigen::Vector3d& other);

    DirectVector& operator=(const DirectVector& other);

    DirectVector& operator+=(const Eigen::Vector3d& other)=delete;

    DirectVector& operator+=(const DirectVector& other);

    DirectVector operator+(const Eigen::Vector3d& other)=delete;

    DirectVector operator+(const DirectVector& other);
};

} // end namespace nsx
