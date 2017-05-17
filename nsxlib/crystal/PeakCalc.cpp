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

#include "PeakCalc.h"
#include "Peak3D.h"
#include "../data/IData.h"
#include "../geometry/Ellipsoid.h"
#include "../geometry/NDTree.h"
#include "../instrument/DetectorEvent.h"

using SX::Geometry::Ellipsoid;
using SX::Geometry::Matrix3d;
using std::shared_ptr;
using SX::Data::DataSet;
using Ellipsoid3D = SX::Geometry::Ellipsoid<double, 3>;


namespace SX {
namespace Crystal {

using sptrPeak3D = PeakCalc::sptrPeak3D;
using PeakList = PeakCalc::PeakList;

PeakCalc::PeakCalc(double h,double k,double l, double x,double y, double frame):
    _h(h),_k(k),_l(l),_x(x),_y(y),_frame(frame)
{
}

//PeakCalc::~PeakCalc()
//{
//}

sptrPeak3D PeakCalc::averagePeaks(const Octree& tree, double distance, double min_axis)
{
    Eigen::Matrix3d peak_shape;
    sptrPeak3D peak = std::make_shared<Peak3D>();

    // An averaged peak is by definition not an observed peak but a calculated peak
    peak->setObserved(false);

    Eigen::Vector3d center(_x, _y, _frame);
    Eigen::SelfAdjointEigenSolver<Matrix3d> solver;
    // todo: should the z component have a scaling factor?

    const double val = distance;
    Eigen::Vector3d vals = {val, val, val};
    Eigen::Matrix3d vects = Eigen::Matrix3d::Identity();

    Ellipsoid3D search_shape = {center, vals, vects};
    auto&& neighbors = tree.getCollisions(search_shape);

    unsigned int num_neighbors = 0;
    peak_shape.setZero();

    for(auto&& p: neighbors) {
        const Ellipsoid3D* ell_peak = dynamic_cast<const Ellipsoid3D*>(p);

        if (ell_peak == nullptr) {
            continue;
        }
        const Matrix3d peak_rs = ell_peak->getRSinv();
        peak_shape += (peak_rs.transpose() * peak_rs).inverse();
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

    peak->setShape(Ellipsoid3D(center, vals, solver.eigenvectors()));
    return peak;
}

} // Namespace Crystal
} // namespace SX
