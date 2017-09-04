#pragma once

#include <Eigen/Dense>

namespace nsx {

class DirectVector : public Eigen::Vector3d {

public:

    DirectVector(const Eigen::Vector3d& vector);

    DirectVector(double x, double y, double z);

    virtual ~DirectVector();

    DirectVector& operator+=(const DirectVector& other);

    DirectVector& operator*=(double factor);

};

} // end namespace nsx
