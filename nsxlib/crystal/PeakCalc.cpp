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

 Forshungszentrum Juelich GmbH
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
#include "IData.h"
#include "Ellipsoid.h"

using namespace SX::Geometry;

namespace SX
{

namespace Crystal
{

PeakCalc::PeakCalc(double h,double k,double l, double x,double y, double frame):
    _h(h),_k(k),_l(l),_x(x),_y(y),_frame(frame)
{

}

PeakCalc::~PeakCalc()
{
}

sptrPeak3D PeakCalc::averagePeaks(std::shared_ptr<Data::IData> data, double distance)
{
    Eigen::Matrix3d peak_shape, bkg_shape;
    sptrPeak3D peak = sptrPeak3D(new Peak3D(data));

    std::vector<sptrPeak3D> neighbors;

    const double original_distance = distance;

    // An averaged peak is by definition not an observed peak but a calculated peak
    peak->setObserved(false);

    do {
        neighbors = findNeighbors(data->getPeaks(), distance);
        distance += original_distance;
    } while(neighbors.size() <= 0);

    if (neighbors.size() <= 0)
        return nullptr;

//    peak->setMillerIndices(_h, _k, _l);
    double weight = 1.0 / (double)neighbors.size();
    peak_shape.setZero();
    bkg_shape.setZero();

    for(sptrPeak3D p: neighbors)
    {
        std::shared_ptr<Ellipsoid<double, 3>> ell_peak = std::dynamic_pointer_cast<Ellipsoid3D>(p->getPeak());
        std::shared_ptr<Ellipsoid<double, 3>> ell_bkg = std::dynamic_pointer_cast<Ellipsoid3D>(p->getBackground());

        // in current implementation these casts should always work
        assert(ell_peak != nullptr);
        assert(ell_bkg != nullptr);

        const Matrix3d& peak_rs = ell_peak->getRSinv();
        const Matrix3d& bkg_rs = ell_bkg->getRSinv();

        peak_shape += weight * peak_rs.transpose() * peak_rs;
        bkg_shape += weight * bkg_rs.transpose() * bkg_rs;
    }

    Eigen::Vector3d center(_x, _y, _frame);
    Eigen::SelfAdjointEigenSolver<Matrix3d> solver;
    Eigen::Vector3d eigenvalues;

    solver.compute(peak_shape);
    eigenvalues = solver.eigenvalues();

    for (int i = 0; i < 3; ++i)
        eigenvalues(i) = 1.0 / std::sqrt(eigenvalues(i));

    peak->setPeakShape(sptrEllipsoid3D(new Ellipsoid<double, 3>(center, eigenvalues, solver.eigenvectors())));

    solver.compute(bkg_shape);
    eigenvalues = solver.eigenvalues();

    for (int i = 0; i < 3; ++i)
        eigenvalues(i) = 1.0 / std::sqrt(eigenvalues(i));

    peak->setBackgroundShape(sptrEllipsoid3D(new Ellipsoid<double, 3>(center, eigenvalues, solver.eigenvectors())));

    return peak;
}

std::vector<sptrPeak3D> PeakCalc::findNeighbors(const std::set<sptrPeak3D>& peak_list, double distance)
{
    const double max_squared_dist = distance*distance;
    std::vector<sptrPeak3D> neighbors;
    neighbors.reserve(100);
    Eigen::Vector3d center(_x, _y, _frame);

    for (sptrPeak3D peak: peak_list) {
        // ignore peak
        if (peak->isMasked() || !peak->isSelected())
            continue;

        const double squared_dist = (center-peak->getPeak()->getAABBCenter()).squaredNorm();

        // not close enough
        if ( squared_dist > max_squared_dist)
            continue;

        neighbors.push_back(peak);
    }

    neighbors.shrink_to_fit();
    return neighbors;
}


} // Namespace Crystal
} /* namespace SX */
