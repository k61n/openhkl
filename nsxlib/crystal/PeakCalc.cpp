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

#include "PeakCalc.h"
#include "Peak3D.h"
#include "IData.h"
#include "Ellipsoid.h"

#include <Eigen/Eigenvalues>

#include <set>

using namespace SX::Geometry;

namespace SX {
namespace Crystal {


PeakCalc::PeakCalc(double h,double k,double l, double x,double y, double frame):
    _h(h),_k(k),_l(l),_x(x),_y(y),_frame(frame)
{

}

PeakCalc::~PeakCalc()
{
}

Peak3D* PeakCalc::averagePeaks(std::shared_ptr<Data::IData> data, double distance)
{
    Peak3D* peak = new Peak3D(data);

    std::vector<Peak3D*> neighbors(findNeighbors(data->getPeaks(), distance));

    if (neighbors.size() <= 0) {
        delete peak;
        return nullptr;
    }

    peak->setMillerIndices(_h, _k, _l);

    double weight = 1.0 / (double)neighbors.size();

    Eigen::Matrix3d peak_shape, bkg_shape;
    peak_shape.setZero();
    bkg_shape.setZero();

    for(Peak3D* p: neighbors) {
        const Ellipsoid<double, 3>* ell_peak = dynamic_cast<const Ellipsoid<double, 3>*>(p->getPeak());
        const Ellipsoid<double, 3>* ell_bkg = dynamic_cast<const Ellipsoid<double, 3>*>(p->getBackground());

        // in current implementation these casts should always work
        assert(ell_peak != nullptr);
        assert(ell_bkg != nullptr);

        // debugging
        if (ell_peak->getAABBExtents().maxCoeff() > 1e3)
            std::cout << "something strange" << std::endl;
        if (ell_peak->getAABBExtents().minCoeff() < 1e-2)
            std::cout << "something strange" << std::endl;

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

    peak->setPeakShape(new Ellipsoid<double, 3>(center, eigenvalues, solver.eigenvectors()));

    solver.compute(bkg_shape);
    eigenvalues = solver.eigenvalues();

    for (int i = 0; i < 3; ++i)
        eigenvalues(i) = 1.0 / std::sqrt(eigenvalues(i));

    peak->setBackgroundShape(new Ellipsoid<double, 3>(center, eigenvalues, solver.eigenvectors()));

    return peak;
}

std::vector<Peak3D *> PeakCalc::findNeighbors(const std::set<Peak3D *> &peak_list, double distance)
{
    const double max_squared_dist = distance*distance;

    std::vector<Peak3D*> neighbors;
    neighbors.reserve(100);

    Eigen::Vector3d center(_x, _y, _frame);

    for (Peak3D* peak: peak_list) {
        if (peak->isMasked() || !peak->isSelected())
            continue;

        double squared_dist = (center-peak->getPeak()->getAABBCenter()).squaredNorm();

        if ( squared_dist < max_squared_dist)
            neighbors.push_back(peak);
    }

    neighbors.shrink_to_fit();
    return neighbors;
}


} // Namespace Crystal
} /* namespace SX */
