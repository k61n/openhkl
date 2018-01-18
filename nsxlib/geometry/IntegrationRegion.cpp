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

IntegrationRegion::IntegrationRegion(Ellipsoid shape, double bkg_begin, double bkg_end, int nslices):
    _shape(shape),
    _bkgBegin(bkg_begin),
    _bkgEnd(bkg_end),
    _nslices(nslices),
    _bestSlice(0)
{
}

AABB IntegrationRegion::aabb() const
{
    Ellipsoid bkg(_shape);
    bkg.scale(_bkgEnd);
    return bkg.aabb();
}

void IntegrationRegion::updateMask(Eigen::MatrixXi& mask, double z) const
{
    const auto& bounding_box = aabb();
    auto lower = bounding_box.lower();
    auto upper = bounding_box.upper();

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
            auto s = classifySlice(p);
            if (s >= 0) {
                mask(y,x) += s;
            }
        }
    }
}

int IntegrationRegion::classifySlice(const Eigen::Vector3d& p) const
{
    const auto& x = p-_shape.center();
    const double r2 = x.transpose()*_shape.metric()*x;

    // point falls outside of background region
    if (r2 > _bkgEnd*_bkgEnd) {
        return -1;
    }
    // point is outside the integration shell
    if (r2 > _bkgBegin*_bkgBegin) {
        return 0;
    }

    // determine which integration shell it lies in
    const int selected_slice  = 1 + static_cast<int>(std::sqrt(r2)*_nslices/_bkgBegin);

    return selected_slice;
}

size_t IntegrationRegion::nslices() const
{
    // note: we add one here to include the background case
    return _nslices+1;
}

//! Best integration slice
int IntegrationRegion::bestSlice() const
{
    return _bestSlice;
}
//! Set the best integration slice.
void IntegrationRegion::setBestSlice(int n)
{
    assert(n > 0 && n <= _nslices);
    _bestSlice = n;
}

} // end namespace nsx
