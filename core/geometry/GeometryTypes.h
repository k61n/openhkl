//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/geometry/GeometryTypes.h
//! @brief     Defines things
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

namespace nsx {

class AABB;
class ConvexHull;
class DirectVector;
class Ellipsoid;
class ShapeLibrary;
struct Edge;
struct Face;
struct Triangle;
struct Vertex;

using sptrShapeLibrary = std::shared_ptr<ShapeLibrary>;

} // namespace nsx

#endif // CORE_GEOMETRY_GEOMETRYTYPES_H
