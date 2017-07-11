/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2016- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 Forschungszentrum Juelich GmbH
 52425 Juelich
 Germany
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

#include <set>

#include <Eigen/Eigenvalues>

#include "../crystal/Peak3D.h"
#include "../crystal/PeakCalc.h"
#include "../data/DataSet.h"
#include "../geometry/Ellipsoid.h"
#include "../geometry/Octree.h"
#include "../instrument/ComponentState.h"
#include "../instrument/DetectorEvent.h"

namespace nsx {

PeakCalc::PeakCalc(int h, int k, int l, double x,double y, double frame):
    _h(h),_k(k),_l(l),_x(x),_y(y),_frame(frame), _intensity()
{
}

PeakCalc::PeakCalc(): _h(0), _k(0), _l(0), _x(0), _y(0), _frame(0), _intensity()
{

}

PeakCalc::PeakCalc(const Peak3D& peak): _intensity()
{
    auto hkl = peak.getIntegerMillerIndices();
    auto center = peak.getShape().aabb().center();

    _h = hkl(0);
    _k = hkl(1);
    _l = hkl(2);

    _x = center(0);
    _y = center(1);
    _frame = center(2);

    _intensity = peak.getCorrectedIntensity();
}

sptrPeak3D PeakCalc::averagePeaks(const Octree& tree, double distance, double min_axis)
{
    Eigen::Matrix3d peak_shape;

    Eigen::Vector3d center(_x, _y, _frame);
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver;
    // todo: should the z component have a scaling factor?

    const double val = distance;
    Eigen::Vector3d vals = {val, val, val};
    Eigen::Matrix3d vects = Eigen::Matrix3d::Identity();

    Ellipsoid search_shape = {center, vals, vects};
    auto&& neighbors = tree.getCollisions(search_shape);

    unsigned int num_neighbors = 0;
    peak_shape.setZero();

    for(auto&& p: neighbors) {
        const Ellipsoid* ell_peak = dynamic_cast<const Ellipsoid*>(p);

        if (ell_peak == nullptr) {
            continue;
        }
        peak_shape += ell_peak->inverseMetric();
        ++num_neighbors;
    }

    // too few neighbors to get average shape
    if (num_neighbors < 1) {
        return nullptr;
    }

    peak_shape /= num_neighbors;
    solver.compute(peak_shape);
    vals = solver.eigenvalues();

    for (auto i = 0; i < vals.size(); ++i) {
        vals(i) = std::sqrt(vals(i));

        if (vals(i) < min_axis) {
            vals(i) = min_axis;
        }
    }

    sptrPeak3D peak = std::make_shared<Peak3D>(Ellipsoid(center, vals, solver.eigenvectors()));

    // An averaged peak is by definition not an observed peak but a calculated peak
    peak->setObserved(false);

    return peak;
}

} // end namespace nsx
