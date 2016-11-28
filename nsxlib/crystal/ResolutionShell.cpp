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

#include "ResolutionShell.h"
#include "gcd.h"

namespace SX
{

namespace Crystal
{

ResolutionShell::ResolutionShell(double dmin, double dmax, double wavelength, SX::Crystal::UnitCell cell):
    _dmin(dmin), _dmax(dmax), _wavelength(wavelength), _cell(cell), _hkls()
{
    computeIndices();
}

const std::vector<Eigen::Vector3i>& ResolutionShell::getIndices()
{
    return _hkls;
}

void ResolutionShell::computeIndices()
{
    const Eigen::Vector3d b1(_cell.getReciprocalAVector());
    const Eigen::Vector3d b2(_cell.getReciprocalBVector());
    const Eigen::Vector3d b3(_cell.getReciprocalCVector());
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eigen_solver;
    const Eigen::Matrix3d BTB(_cell.getReciprocalMetricTensor());

    _hkls.clear();

    eigen_solver.compute(BTB);
    double b_min = std::sqrt(eigen_solver.eigenvalues().minCoeff());
    const int hkl_max = std::ceil(2.0 / (_wavelength * b_min));

    for (int h = -hkl_max; h <= hkl_max; ++h) {
        for (int k = -hkl_max; k <= hkl_max; ++k) {
            for (int l = -hkl_max; l <= hkl_max; ++l) {
                double gcd = (double)SX::Utils::gcd(h, k, l);
                Eigen::Vector3d q = h*b1 + k*b2 + l*b3;
                double d = gcd / q.norm();

                if (d >= _dmin && d <= _dmax)
                    _hkls.push_back(Eigen::Vector3i(h, k, l));
            }
        }
    }
}


} // Namespace Crystal
} /* namespace SX */
