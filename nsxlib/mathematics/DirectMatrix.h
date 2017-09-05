#pragma once

#include <Eigen/Dense>

#include "DirectVector.h"

namespace nsx {

class DirectMatrix : public Eigen::Matrix3d
{

public:

    using Eigen::Matrix3d::Matrix3d;

    DirectMatrix(const Eigen::Vector3d& col1, const Eigen::Vector3d& col2, const Eigen::Vector3d& col3);

    virtual ~DirectMatrix();

    DirectMatrix& operator=(const DirectMatrix& other);

    DirectMatrix& operator+=(const Eigen::Matrix3d& other)=delete;

    DirectMatrix& operator+=(const DirectMatrix& other);

    DirectMatrix operator+(const Eigen::Matrix3d& other)=delete;

    DirectMatrix operator+(const DirectMatrix& other);

    DirectVector operator*(const DirectVector& vector);
};

} // end namespace nsx
