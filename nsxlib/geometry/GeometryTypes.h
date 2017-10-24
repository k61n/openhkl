#pragma once

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
class Octree;
class Triangle;
class Vertex;

using HomVector   = Eigen::Matrix<double,4,1>;
using HomMatrix   = Eigen::Matrix<double,4,4>;

using covMat = Eigen::Matrix<double,9,9>;

using sptrBasis     = std::shared_ptr<Basis>;
using sptrEllipsoid = std::shared_ptr<Ellipsoid>;

using TrianglesList = std::vector<Triangle>;

using EquivalencePair = std::pair<int,int>;
using EquivalenceList = std::vector<EquivalencePair>;

using Blob3DUMap = std::unordered_map<int,Blob3D>;

enum class Direction {CW=1,CCW=2};

enum class PointType {REGION = 1, BACKGROUND = 2, EXCLUDED = 3};

} // end namespace nsx
