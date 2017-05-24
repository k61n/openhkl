#ifndef NSXLIB_GEOMETRYTYPES_H
#define NSXLIB_GEOMETRYTYPES_H

#include <functional>
#include <memory>
#include <set>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Eigen/Dense>

namespace nsx {

class AABB;
class Basis;
class Blob3D;
class ConvexHull;
class Edge;
class Ellipsoid;
class Face;
class IShape;
class Octree;
class OBB;
class Sphere;
class Triangle;
class Vertex;

using HomVector   = Eigen::Matrix<double,4,1>;
using HomMatrix   = Eigen::Matrix<double,4,4>;

using covMat = Eigen::Matrix<double,9,9>;

using sptrBasis     = std::shared_ptr<Basis>;
using sptrIShape    = std::shared_ptr<IShape>;
using sptrEllipsoid = std::shared_ptr<Ellipsoid>;

using TrianglesList = std::vector<Triangle>;

using EquivalencePair = std::pair<int,int>;
using EquivalenceList = std::vector<EquivalencePair>;

using Blob3DUMap = std::unordered_map<int,Blob3D>;

using Shape3DMap = std::unordered_map<const IShape*,int>;

using CollisionSet = std::set<const IShape*>;

enum class Direction {CW,CCW};

} // end namespace nsx

#endif // NSXLIB_GEOMETRYTYPES_H
