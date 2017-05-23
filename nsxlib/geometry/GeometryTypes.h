#ifndef NSXLIB_GEOMETRYTYPES_H
#define NSXLIB_GEOMETRYTYPES_H

#include <functional>
#include <memory>

#include <Eigen/Dense>

namespace nsx {

class Basis;
class Blob3D;

template <typename T>
class ConvexHull;

class AABB;
class Ellipsoid;
class IShape;
class OBB;
class Sphere;

template <typename T, unsigned int>
class NDTree;

using HomVector   = Eigen::Matrix<double,4,1>;
using HomMatrix   = Eigen::Matrix<double,4,4>;

using Octree      = NDTree<double,3>;

using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using FilterCallback = std::function<RealMatrix(const RealMatrix&)>;

using covMat = Eigen::Matrix<double,9,9>;

using sptrBasis     = std::shared_ptr<Basis>;
using sptrIShape    = std::shared_ptr<IShape>;
using sptrEllipsoid = std::shared_ptr<Ellipsoid>;

enum class Direction {CW,CCW};

} // end namespace nsx

#endif // NSXLIB_GEOMETRYTYPES_H
