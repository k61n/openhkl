#pragma once

#include <Eigen/Dense>

namespace nsx {

void removeColumn(Eigen::MatrixXd& matrix, unsigned int colToRemove);

void removeRow(Eigen::MatrixXd& matrix, unsigned int rowToRemove);

Eigen::Matrix3d
interpolateRotation(const Eigen::Matrix3d& U0, const Eigen::Matrix3d& U1, const double t);

} // end namespace nsx
