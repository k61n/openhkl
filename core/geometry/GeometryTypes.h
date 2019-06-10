//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/geometry/GeometryTypes.h
//! @brief     Defines classes Direction, PointType
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_GEOMETRY_GEOMETRYTYPES_H
#define CORE_GEOMETRY_GEOMETRYTYPES_H

#include <memory>
#include <vector>

#include <Eigen/Dense>

namespace nsx {

class AABB;
class ConvexHull;
class DirectVector;
class Ellipsoid;
class ReciprocalVector;
class ShapeLibrary;
struct Edge;
struct Face;
struct Triangle;
struct Vertex;


using sptrShapeLibrary = std::shared_ptr<ShapeLibrary>;

using EquivalencePair = std::pair<int, int>;
using EquivalenceList = std::vector<EquivalencePair>;


enum class Direction { CW = 1, CCW = 2 };

enum class PointType { REGION = 1, BACKGROUND = 2, EXCLUDED = 3 };

} // namespace nsx

#endif // CORE_GEOMETRY_GEOMETRYTYPES_H
