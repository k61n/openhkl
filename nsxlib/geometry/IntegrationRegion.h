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

#ifndef NSXTOOL_PEAKSHAPE_H_
#define NSXTOOL_PEAKSHAPE_H_

#include "Ellipsoid.h"
#include "AABB.h"
#include <list>

namespace SX {
namespace Geometry {
class IntegrationRegion {
public:
    using Ellipsoid3D = Ellipsoid<double, 3>;
    using AABB3D = AABB<double, 3>;

    IntegrationRegion() = default;
    IntegrationRegion(const Ellipsoid3D& region, double scale = 1.0, double bkg_scale = 3.0);

    const Ellipsoid3D& getRegion() const;
    bool inRegion(const Eigen::Vector4d& p) const;
    bool inBackground(const Eigen::Vector4d& p) const;
    bool inForbidden(const Eigen::Vector4d& p) const;

    void addForbidden(const IntegrationRegion& other);
    void resetForbidden();

    const AABB3D& getBackground() const;

private:
    Ellipsoid3D _region;
    AABB3D _background;
    std::list<Ellipsoid3D> _forbidden;
};

} // namespace Geometry
} // namespace SX

#endif // NSXTOOL_SHAPEUNION_H_
