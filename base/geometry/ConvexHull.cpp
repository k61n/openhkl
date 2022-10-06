//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/geometry/ConvexHull.cpp
//! @brief     Implements class ConvexHull
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "base/geometry/ConvexHull.h"

#include <memory>

#include <libqhullcpp/Qhull.h>
#include <libqhullcpp/QhullFacetList.h>
#include <libqhullcpp/QhullVertexSet.h>

namespace ohkl {

using namespace orgQhull;


ConvexHull::ConvexHull(const ConvexHull& other)
{
    *this = other;
}

ConvexHull& ConvexHull::operator=(const ConvexHull& other) = default;

void ConvexHull::reset()
{
    _vertices.clear();
    _faces.clear();

    _normals.clear();
    _dists.clear();

    _center = Eigen::Vector3d(0., 0., 0.);

    _volume = 0.;
    _outerR2 = -1.;

    const double inf = std::numeric_limits<double>::infinity();
    Eigen::Vector3d aabblower(inf, inf, inf);
    Eigen::Vector3d aabbupper = -aabblower;
    _aabb = AABB(aabblower, aabbupper);
}

bool ConvexHull::addVertex(const Eigen::Vector3d& coords, double tolerance)
{
    // test if the vertex is duplicate and ignore it in that case
    for (const auto& v : _vertices) {
        if (v.isApprox(coords, tolerance)) {
            return false;
        }
    }

    _vertices.push_back(coords);
    return true;
}

bool ConvexHull::removeVertex(const Eigen::Vector3d& coords, double tolerance)
{
    for (auto it = _vertices.begin(); it != _vertices.end(); ++it) {
        const auto& v = *it;
        if (v.isApprox(coords, tolerance)) {
            _vertices.erase(it);
            return true;
        }
    }
    return false;
}

// Convex hull calculation using qhull,
// code from Takin2/tlibs2 (doi: 10.5281/zenodo.4117437).
bool ConvexHull::updateHull(double tolerance)
{
    if (_vertices.size() < 4)
        return false;

    if (_vertices.size() == 4) {
        Eigen::Matrix3d mat;
        mat << _vertices[1] - _vertices[0], _vertices[2] - _vertices[0],
            _vertices[3] - _vertices[0];
        // do the vectors all lie on a plane?
        if (std::abs(mat.determinant()) < tolerance)
            return false;
    }

    try {
        std::unique_ptr<double[]> mem{new double[_vertices.size() * 3]};

        std::size_t i = 0;
        for (const Eigen::Vector3d& vert : _vertices) {
            mem[i++] = vert[0];
            mem[i++] = vert[1];
            mem[i++] = vert[2];
        }

        // calculate convex hull
        Qhull qhull;
        qhull.setFactorEpsilon(tolerance);
        qhull.setErrorStream(&std::cerr);
        qhull.runQhull("tlibs2", 3, int(_vertices.size()), mem.get(), "");
        QhullVertexList vertices = qhull.vertexList();
        QhullFacetList facets = qhull.facetList();

        reset();

        _volume = qhull.volume();

        // get hull vertices and bounding sphere center
        for (auto iter = vertices.begin(); iter != vertices.end(); ++iter) {
            QhullPoint point = (*iter).point();
            Eigen::Vector3d vert(point[0], point[1], point[2]);
            _center += vert;
            _vertices.emplace_back(std::move(vert));
        }
        _center /= double(_vertices.size());

        // get bounding sphere and aabb
        Eigen::Vector3d aabblower = _aabb.lower();
        Eigen::Vector3d aabbupper = _aabb.upper();
        for (const auto& vert : _vertices) {
            double r2 = (vert - _center).squaredNorm();
            _outerR2 = std::max(_outerR2, r2);

            aabblower(0) = std::min(aabblower(0), vert(0));
            aabblower(1) = std::min(aabblower(1), vert(1));
            aabblower(2) = std::min(aabblower(2), vert(2));

            aabbupper(0) = std::max(aabbupper(0), vert(0));
            aabbupper(1) = std::max(aabbupper(1), vert(1));
            aabbupper(2) = std::max(aabbupper(2), vert(2));
        }
        _aabb.setLower(aabblower);
        _aabb.setUpper(aabbupper);

        // get polygons for the hull faces
        for (auto iter = facets.begin(); iter != facets.end(); ++iter) {
            if (iter->isUpperDelaunay())
                continue;

            std::vector<Eigen::Vector3d> face;
            QhullVertexSet vertices = iter->vertices();
            for (auto iterVertex = vertices.begin(); iterVertex != vertices.end(); ++iterVertex) {
                QhullPoint point = (*iterVertex).point();
                face.emplace_back(Eigen::Vector3d(point[0], point[1], point[2]));
            }

            _faces.emplace_back(std::move(face));

            // polygon plane
            QhullHyperplane plane = iter->hyperplane();
            const double* norm = plane.coordinates();
            _normals.emplace_back(Eigen::Vector3d(norm[0], norm[1], norm[2]));
            _dists.push_back(plane.offset());
        }

        return true;
    } catch (const std::exception& ex) {
        return false;
    }
}

// Note: this function is absolutely performance critical.
// Make changes with caution, and remember to profile!
bool ConvexHull::contains(const Eigen::Vector3d& v) const
{
    const double r2 = (v - _center).squaredNorm();

    // point outside bounding sphere?
    if (r2 > _outerR2)
        return false;

    // point outside bounding box?
    if (!_aabb.isInside(v))
        return false;

    // directly check against face planes of the hull
    for (std::size_t normidx = 0; normidx < _normals.size(); ++normidx) {
        if (v.dot(_normals[normidx]) <= _dists[normidx])
            return false;
    }

    return true;
}

} // namespace ohkl
