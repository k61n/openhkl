#ifndef NSXLIB_MATHEMATICSTYPES_H
#define NSXLIB_MATHEMATICSTYPES_H

#include <functional>

#include <Eigen/Dense>

namespace nsx {

using IntMatrix = Eigen::Matrix<int,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>;
using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

using FilterCallback = std::function<RealMatrix(const RealMatrix&)>;
} // end namespace nsx

#endif // NSXLIB_MATHEMATICSTYPES_H
