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

#include "IntegrationRegion.h"

#include "AABB.h"
#include "Ellipsoid.h"

#include <list>

namespace SX {
namespace Geometry {

IntegrationRegion::IntegrationRegion(
        const IntegrationRegion::Ellipsoid3D &region, double scale, double bkg_scale):
    _region(region),
    _background(region.getLower(), region.getUpper())
{
    _region.scale(scale); // todo: need erf_inv
    _background.scale(bkg_scale); // todo: need erf_inv
}

bool IntegrationRegion::inRegion(const Eigen::Vector4d &p) const
{
    if (inForbidden(p)) {
        return false;
    }
    return _region.isInside(p);
}

bool IntegrationRegion::inBackground(const Eigen::Vector4d &p) const
{
    // NOTE: we only us AABB for background!
    if(!_background.isInsideAABB(p)) {
        return false;
    }
    if (inRegion(p)) {
        return false;
    }
    return !inForbidden(p);
}

bool IntegrationRegion::inForbidden(const Eigen::Vector4d &p) const
{
    for (auto&& shape: _forbidden) {
        if (shape.isInside(p)) {
            return true;
        }
    }
    return false;
}

void IntegrationRegion::addForbidden(const IntegrationRegion& other)
{
    auto&& shape = other._region;

    if (_background.collide(shape)) {
        _forbidden.push_back(shape);
    }
}

void IntegrationRegion::resetForbidden()
{
    _forbidden.clear();
}

const IntegrationRegion::AABB3D& IntegrationRegion::getBackground() const
{
    return _background;
}

} // namespace Geometry
} // namespace SX
