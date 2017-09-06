#pragma once

#include <Eigen/Dense>

namespace nsx {

class DirectVector;

class DirectMatrix : public Eigen::Matrix3d
{

public:

    DirectMatrix();

    DirectMatrix(const DirectMatrix& other);

    DirectMatrix(const Eigen::Matrix3d& matrix)=delete;

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
