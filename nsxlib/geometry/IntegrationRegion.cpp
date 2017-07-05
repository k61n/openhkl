/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2017- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
          Jonathan Fisher, Forschungszentrum Juelich GmbH
    BP 156
    6, rue Jules Horowitz
    38042 Grenoble Cedex 9
    France
    chapon[at]ill.fr
    pellegrini[at]ill.fr
    j.fisher[at]fz-juelich.de

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "Ellipsoid.h"
#include "IntegrationRegion.h"

namespace nsx {

IntegrationRegion::IntegrationRegion(
        const Ellipsoid &region, double scale, double bkg_scale):
    _region(region),
    _background(region)
{
    _region.scale(scale); // todo: need erf_inv
    _background.scale(bkg_scale); // todo: need erf_inv
}

const Ellipsoid &IntegrationRegion::getRegion() const
{
    return _region;
}

bool IntegrationRegion::inRegion(const Eigen::Vector3d &p) const
{
    return _region.isInside(p);
}

bool IntegrationRegion::inBackground(const Eigen::Vector3d &p) const
{
    if(!_background.aabb().isInside(p)) {
        return false;
    }
    // exclude if in peak
    return !inRegion(p);
}

PointType IntegrationRegion::classifyPoint(const Eigen::Vector3d &p) const
{
    if (!_background.aabb().isInside(p)) {
        return PointType::EXCLUDED;
    }
    if (_region.isInside(p)) {
        return PointType::REGION;
    }
    return PointType::BACKGROUND;
}

const Ellipsoid& IntegrationRegion::getBackground() const
{
    return _background;
}

void IntegrationRegion::updateMask(Eigen::MatrixXi& mask, double z) const
{
    auto aabb = _background.aabb();
    auto lower = aabb.lower();
    auto upper = aabb.upper();

    if (z < lower[2] || z > upper[2]) {
        return;
    }

    long xmin = std::lround(std::floor(lower[0]));
    long ymin = std::lround(std::floor(lower[1]));
    long xmax = std::lround(std::ceil(upper[0])+1);
    long ymax = std::lround(std::ceil(upper[1])+1);

    xmin = std::max(0l, xmin);
    ymin = std::max(0l, ymin);

    xmax = std::min(xmax, long(mask.cols()));
    ymax = std::min(ymax, long(mask.rows()));

    for (auto x = xmin; x < xmax; ++x) {
        for (auto y = ymin; y < ymax; ++y) {
            Eigen::Vector3d p(x, y, z);
            if (inRegion(p)) {
                mask(y, x) = 1;
            }
        }
    }
}

} // end namespace nsx
