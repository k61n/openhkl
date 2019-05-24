#ifndef CORE_MATHEMATICS_MATHEMATICSTYPES_H
#define CORE_MATHEMATICS_MATHEMATICSTYPES_H

#include <Eigen/Dense>

namespace nsx {

using IntMatrix = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

} // end namespace nsx

#endif // CORE_MATHEMATICS_MATHEMATICSTYPES_H
