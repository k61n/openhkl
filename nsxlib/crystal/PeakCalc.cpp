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
#include "../instrument/DetectorEvent.h"

using SX::Geometry::Ellipsoid;
using SX::Geometry::Matrix3d;
using std::shared_ptr;
using SX::Data::IData;


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

sptrPeak3D PeakCalc::averagePeaks(const std::shared_ptr<IData> data, double distance)
{
    Eigen::Matrix3d peak_shape, bkg_shape;

    sptrPeak3D peak = std::make_shared<Peak3D>(Peak3D(data));
    PeakList neighbors;
    const double original_distance = distance;

    // An averaged peak is by definition not an observed peak but a calculated peak
    peak->setObserved(false);

    do {
        neighbors = findNeighbors(data->getPeaks(), distance);
        distance += original_distance;
    } while(neighbors.empty());

    if (neighbors.empty()) {
        return nullptr;
    }

    double weight = 1.0 / double(neighbors.size());
    peak_shape.setZero();
    bkg_shape.setZero();

    using ellipsoid = Ellipsoid<double, 3>;

    for(auto&& p: neighbors) {
        // in current implementation these casts should always work
        const ellipsoid& ell_peak = p->getPeak();
        const ellipsoid& ell_bkg = p->getBackground();

        const Matrix3d& peak_rs = ell_peak.getRSinv();
        const Matrix3d& bkg_rs = ell_bkg.getRSinv();

        peak_shape += weight * peak_rs.transpose() * peak_rs;
        bkg_shape += weight * bkg_rs.transpose() * bkg_rs;
    }

    Eigen::Vector3d center(_x, _y, _frame);
    Eigen::SelfAdjointEigenSolver<Matrix3d> solver;
    Eigen::Vector3d eigenvalues;

    solver.compute(peak_shape);
    eigenvalues = solver.eigenvalues();

    for (int i = 0; i < 3; ++i) {
        eigenvalues(i) = 1.0 / std::sqrt(eigenvalues(i));
    }

    peak->setPeakShape(ellipsoid(center, eigenvalues, solver.eigenvectors()));
    solver.compute(bkg_shape);
    eigenvalues = solver.eigenvalues();

    for (int i = 0; i < 3; ++i) {
        eigenvalues(i) = 1.0 / std::sqrt(eigenvalues(i));
    }
    peak->setBackgroundShape(ellipsoid(center, eigenvalues, solver.eigenvectors()));
    return peak;
}

PeakCalc::sptrPeak3D PeakCalc::averagePeaksQ(const std::shared_ptr<PeakCalc::IData> data)
{
//    Eigen::Matrix3d peak_shape, bkg_shape;

//    sptrPeak3D peak = std::make_shared<Peak3D>(Peak3D(data));

//    // An averaged peak is by definition not an observed peak but a calculated peak
//    peak->setObserved(false);

//    peak_shape.setZero();
//    bkg_shape.setZero();

//    using ellipsoid = Ellipsoid<double, 3>;

//    for(auto&& p: data->getPeaks()) {
//        // in current implementation these casts should always work
//        const ellipsoid& ell_peak = p->getPeak();
//        const ellipsoid& ell_bkg = p->getBackground();

//        const Matrix3d& peak_rs = ell_peak.getRSinv();
//        const Matrix3d& bkg_rs = ell_bkg.getRSinv();

//        peak_shape += weight * peak_rs.transpose() * peak_rs;
//        bkg_shape += weight * bkg_rs.transpose() * bkg_rs;
//    }

//    Eigen::Vector3d center(_x, _y, _frame);
//    Eigen::SelfAdjointEigenSolver<Matrix3d> solver;
//    Eigen::Vector3d eigenvalues;

//    solver.compute(peak_shape);
//    eigenvalues = solver.eigenvalues();

//    for (int i = 0; i < 3; ++i) {
//        eigenvalues(i) = 1.0 / std::sqrt(eigenvalues(i));
//    }

//    peak->setPeakShape(ellipsoid(center, eigenvalues, solver.eigenvectors()));
//    solver.compute(bkg_shape);
//    eigenvalues = solver.eigenvalues();

//    for (int i = 0; i < 3; ++i) {
//        eigenvalues(i) = 1.0 / std::sqrt(eigenvalues(i));
//    }
//    peak->setBackgroundShape(ellipsoid(center, eigenvalues, solver.eigenvectors()));
//    return peak;
    return nullptr;
}

std::vector<sptrPeak3D> PeakCalc::findNeighbors(const std::set<sptrPeak3D>& peak_list, double distance)
{
    const double max_squared_dist = distance*distance;
    std::vector<sptrPeak3D> neighbors;
    neighbors.reserve(100);
    Eigen::Vector3d center(_x, _y, _frame);

    for (auto&& peak: peak_list) {
        // ignore peak
        if (peak->isMasked() || !peak->isSelected()) {
            continue;
        }
        const double squared_dist = (center-peak->getPeak().getAABBCenter()).squaredNorm();

        // not close enough
        if ( squared_dist > max_squared_dist) {
            continue;
        }
        neighbors.push_back(peak);
    }

    neighbors.shrink_to_fit();
    return neighbors;
}

} // Namespace Crystal
} /* namespace SX */
