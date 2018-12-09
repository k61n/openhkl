#pragma once

#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
#include <Eigen/Dense>

namespace nsx {

class AABB;
class Blob3D;
class ConvexHull;

struct Edge;
struct Face;
struct Triangle;
struct Vertex;

class Ellipsoid;
using sptrEllipsoid = std::shared_ptr<Ellipsoid>;

class Profile3D;
using sptrProfile3D = std::shared_ptr<Profile3D>;

class ShapeLibrary;
using sptrShapeLibrary = std::shared_ptr<ShapeLibrary>;

using TrianglesList = std::vector<Triangle>;

} // end namespace nsx
