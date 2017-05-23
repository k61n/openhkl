#ifndef NSXLIB_GEOMETRYTYPES_H
#define NSXLIB_GEOMETRYTYPES_H

#include <functional>
#include <memory>

#include <Eigen/Dense>

namespace nsx {

class Basis;
class Blob3D;

template <typename T, unsigned int D>
class AABB;

template <typename T>
class ConvexHull;

template <typename T, unsigned int D>
class IShape;

template <typename T, unsigned int D>
class Ellipsoid;

template <typename T, unsigned int>
class NDTree;

using AABB3D      = AABB<double,3>;
using Ellipsoid3D = Ellipsoid<double,3>;
using IShape3D    = IShape<double,3>;
using Octree      = NDTree<double,3>;

using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using FilterCallback = std::function<RealMatrix(const RealMatrix&)>;

using covMat = Eigen::Matrix<double,9,9>;

using sptrBasis  = std::shared_ptr<Basis>;
using sptrIShape3D = std::shared_ptr<IShape3D>;
using sptrEllipsoid3D = std::shared_ptr<Ellipsoid3D>;

} // end namespace nsx

#endif // NSXLIB_GEOMETRYTYPES_H
