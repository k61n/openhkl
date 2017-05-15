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

#ifndef NSXTOOL_INTEGRATIONREGION_H_
#define NSXTOOL_INTEGRATIONREGION_H_

#include "../geometry/Ellipsoid.h"
#include "../geometry/AABB.h"
#include <list>
#include <Eigen/Core>

namespace nsx {

class IntegrationRegion {
public:
    enum class point_type {
        REGION = 1,
        BACKGROUND = 2,
        EXCLUDED = 3
    };

    using Ellipsoid3D = Ellipsoid<double, 3>;

    IntegrationRegion() = default;
    IntegrationRegion(const Ellipsoid3D& region, double scale = 1.0, double bkg_scale = 3.0);

    const Ellipsoid3D& getRegion() const;
    bool inRegion(const Eigen::Vector4d& p) const;
    bool inBackground(const Eigen::Vector4d& p) const;
    point_type classifyPoint(const Eigen::Vector4d& p) const;

    const Ellipsoid3D& getBackground() const;
    void updateMask(Eigen::MatrixXi& mask, double z) const;

private:
    Ellipsoid3D _region, _background;
};

} // end namespace nsx

#endif // NSXTOOL_INTEGRATIONREGION_H_
