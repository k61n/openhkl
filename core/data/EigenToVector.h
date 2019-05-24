#ifndef CORE_DATA_EIGENTOVECTOR_H
#define CORE_DATA_EIGENTOVECTOR_H

#include <vector>

#include <Eigen/Dense>

namespace nsx {

std::vector<double> eigenToVector(const Eigen::VectorXd& ev);

} // end namespace nsx

#endif // CORE_DATA_EIGENTOVECTOR_H
