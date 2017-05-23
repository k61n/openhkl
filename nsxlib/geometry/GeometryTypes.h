#ifndef NSXLIB_GEOMETRYTYPES_H
#define NSXLIB_GEOMETRYTYPES_H

#include <functional>
#include <memory>
#include <vector>

#include <Eigen/Dense>

namespace nsx {

class AABB;
class Basis;
class Blob3D;
class ConvexHull;
class Ellipsoid;
class Edge;
class Face;
class Vertex;
class IShape;
class NDTree;
class OBB;
class Sphere;
class Triangle;

using HomVector   = Eigen::Matrix<double,4,1>;
using HomMatrix   = Eigen::Matrix<double,4,4>;

using covMat = Eigen::Matrix<double,9,9>;

using sptrBasis     = std::shared_ptr<Basis>;
using sptrIShape    = std::shared_ptr<IShape>;
using sptrEllipsoid = std::shared_ptr<Ellipsoid>;

using TrianglesList = std::vector<Triangle>;

enum class Direction {CW,CCW};

} // end namespace nsx

#endif // NSXLIB_GEOMETRYTYPES_H
