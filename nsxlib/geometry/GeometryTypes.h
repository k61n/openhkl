#ifndef NSXLIB_GEOMETRYTYPES_H
#define NSXLIB_GEOMETRYTYPES_H

#include <functional>

#include <Eigen/Dense>

namespace nsx {

using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using FilterCallback = std::function<RealMatrix(const RealMatrix&)>;

template <typename T>
class ConvexHull;

} // end namespace nsx

#endif // NSXLIB_GEOMETRYTYPES_H
