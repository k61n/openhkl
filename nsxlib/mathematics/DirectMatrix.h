#pragma once

#include <Eigen/Dense>

namespace nsx {

class DirectVector;

class DirectMatrix : public Eigen::Matrix3d
{

public:

    using Eigen::Matrix3d::Matrix3d;

    DirectMatrix(const Eigen::Vector3d& col1, const Eigen::Vector3d& col2, const Eigen::Vector3d& col3);

    ~DirectMatrix()=default;

    DirectMatrix& operator=(const DirectMatrix& other);

    DirectMatrix& operator+=(const Eigen::Matrix3d& other)=delete;

    DirectMatrix& operator+=(const DirectMatrix& other);

    DirectMatrix operator+(const Eigen::Matrix3d& other)=delete;

    DirectMatrix operator+(const DirectMatrix& other);

    DirectVector operator*(const DirectVector& vector);
};

} // end namespace nsx
