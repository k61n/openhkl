//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/monte-carlo/MCAbsorption.cpp
//! @brief     Implements class MCAbsorption
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <ctime>
#include <stdexcept>

#include "base/hull/ConvexHull.h"
#include "base/hull/Triangle.h"
#include "core/monte-carlo/MCAbsorption.h"

namespace nsx {

using TrianglesList = std::vector<Triangle>;

MCAbsorption::MCAbsorption(
    const ConvexHull& convex_hull, double source_width, double source_height, double source_y_pos)
    : _convex_hull(convex_hull)
    , _mu_scattering(1.0)
    , _mu_absorption(1.0)
    , _source_width(source_width)
    , _source_height(source_height)
    , _source_y_pos(source_y_pos)
{
}

MCAbsorption::~MCAbsorption() {}

void MCAbsorption::setConvexHull(const ConvexHull& convex_hull)
{
    _convex_hull = convex_hull;
}

void MCAbsorption::setMuAbsorption(double mu_absorption)
{
    _mu_absorption = mu_absorption;
}

void MCAbsorption::setMuScattering(double mu_scattering)
{
    _mu_scattering = mu_scattering;
}

double MCAbsorption::run(
    unsigned int nIterations, const Eigen::Vector3d& outV,
    const Eigen::Matrix3d& sampleOrientation) const
{
    std::function<double(void)> random =
        std::bind(std::uniform_real_distribution<double>(-0.5, 0.5), std::mt19937(std::time(0)));

    TrianglesList faces = _convex_hull.createFaceCache(sampleOrientation);

    if (faces.empty())
        throw std::runtime_error("No sample defined.");

    Eigen::Vector3d dir(0, 1, 0);

    double transmission(0.0);

    unsigned int nHits(0);

    for (unsigned int i = 0; i < nIterations; ++i) {
        double w = random() * _source_width;
        double h = random() * _source_height;
        Eigen::Vector3d point(w, _source_y_pos, h);

        unsigned int nIntersections(0);

        double times[2];

        for (const auto& triangle : faces) {
            if (triangle.rayIntersect(point, dir, times[nIntersections])) {
                if (++nIntersections == 2)
                    break;
            }
        }

        if (nIntersections != 2)
            continue;

        if (times[0] > times[1])
            std::swap(times[0], times[1]);

        double lpm = (0.5 + random()) * (times[1] - times[0]);
        point += lpm * dir + times[0] * dir;

        double t2;
        for (const auto& triangle : faces) {
            if (triangle.rayIntersect(point, outV, t2)) {
                lpm += outV.norm() * t2;
                nHits++;
                break;
            } else {
                continue;
            }
        }

        transmission += exp(-(_mu_scattering + _mu_absorption) * lpm);
    }

    if (nHits == 0)
        transmission = 1.0;
    else
        transmission /= nHits;

    return transmission;
}

} // namespace nsx
