#ifndef NSXLIB_GEOMETRYTYPES_H
#define NSXLIB_GEOMETRYTYPES_H

#include <functional>
#include <memory>

#include <Eigen/Dense>

namespace nsx {

template <typename T>
class ConvexHull;

class Basis;

using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using FilterCallback = std::function<RealMatrix(const RealMatrix&)>;

using covMat = Eigen::Matrix<double,9,9>;

using sptrBasis = std::shared_ptr<Basis>;

} // end namespace nsx

#endif // NSXLIB_GEOMETRYTYPES_H
