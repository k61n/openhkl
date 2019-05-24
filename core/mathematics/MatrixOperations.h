#ifndef CORE_MATHEMATICS_MATRIXOPERATIONS_H
#define CORE_MATHEMATICS_MATRIXOPERATIONS_H

#include <Eigen/Dense>

namespace nsx {

void removeColumn(Eigen::MatrixXd& matrix, unsigned int colToRemove);

void removeRow(Eigen::MatrixXd& matrix, unsigned int rowToRemove);

Eigen::Matrix3d
interpolateRotation(const Eigen::Matrix3d& U0, const Eigen::Matrix3d& U1, const double t);

} // end namespace nsx

#endif // CORE_MATHEMATICS_MATRIXOPERATIONS_H
