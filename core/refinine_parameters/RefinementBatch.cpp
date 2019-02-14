/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2017- Laurent C. Chapon,  Eric Pellegrini, Jonathan Fisher

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

#include <algorithm>
#include <limits>

#include "DataSet.h"
#include "InstrumentState.h"
#include "MillerIndex.h"
#include "Minimizer.h"
#include "PeakFilter.h"
#include "Peak3D.h"
#include "Refiner.h"
#include "UnitCell.h"

const static double g_eps = 1e-5;

namespace nsx {

RefinementBatch::RefinementBatch(InstrumentStateList& states, const UnitCell& uc, const PeakList& peaks)
: _fmin(std::numeric_limits<double>().max()),
  _fmax(std::numeric_limits<double>().lowest()),
  _cell(new UnitCell(uc)),
  _peaks(peaks)
{
    for (auto peak : peaks) {
        const double z = peak->shape().center()[2];
        _fmin = std::min(z, std::floor(_fmin));
        _fmax = std::max(z, std::ceil(_fmax));
    }

    // take care of floating point error
    _fmin -= g_eps;
    _fmax += g_eps;

    _hkls.reserve(_peaks.size());
    for (auto peak : _peaks) {
        MillerIndex hkl(peak->q(), *_cell);
        _hkls.push_back(hkl.rowVector().cast<double>());

        Eigen::Vector3d c = peak->shape().center();
        Eigen::Matrix3d M = peak->shape().metric();
        auto state = peak->data()->interpolatedState(c[2]);
        Eigen::Matrix3d J = state.jacobianQ(c[0], c[1]);
        Eigen::Matrix3d JI = J.inverse();
        Eigen::Matrix3d A = JI.transpose() * M * JI;
        Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(A);
        Eigen::Matrix3d U = solver.eigenvectors();
        Eigen::Matrix3d D;
        D.setZero();
        for (auto i = 0; i < 3; ++i) {
            D(i,i) = std::sqrt(solver.eigenvalues()[i]);
        }
        _wts.emplace_back(U.transpose() * D * U);
    }

    UnitCell constrained = _cell->applyNiggliConstraints();
    _u0 = constrained.niggliOrientation();
    _uOffsets = Eigen::Vector3d::Zero();
    _cellParameters = constrained.parameters();

    _states.reserve(states.size());
    for (size_t i = 0; i < states.size(); ++i) {
        if (!contains(i)) {
            continue;
        }
        _states.push_back(states[i]);
    }

}

void RefinementBatch::refineUB()
{
    for (int i = 0; i < _cellParameters.size(); ++i) {
        _params.addParameter(&_cellParameters(i));
    }
}

void RefinementBatch::refineDetectorOffset()
{
    for (int axis = 0; axis < 3; ++axis) {
        std::vector<int> ids;
        for (size_t i = 0; i < _states.size(); ++i) {
            int id = _params.addParameter(&(_states[i].get().detectorPositionOffset(axis)));
            ids.push_back(id);
        }
        // record the constraints
        _constraints.push_back(ids);
    }
}

void RefinementBatch::refineSamplePosition()
{
    for (int axis = 0; axis < 3; ++axis) {
        std::vector<int> ids;
        for (size_t i = 0; i < _states.size(); ++i) {
            int id = _params.addParameter(&(_states[i].get().samplePosition(axis)));
            ids.push_back(id);
        }
        // record the constraints
        _constraints.push_back(ids);
    }
}

void RefinementBatch::refineSampleOrientation()
{
    // refine the imaginary parts of the quaternion
    for (int axis = 0; axis < 3; ++axis) {
        std::vector<int> ids;
        for (size_t i = 0; i < _states.size(); ++i) {
            int id = _params.addParameter(&(_states[i].get().sampleOrientationOffset.coeffs()[axis]));
            ids.push_back(id);
        }
        // record the constraints
        _constraints.push_back(ids);
    }
}

void RefinementBatch::refineKi()
{
    std::vector<int> x_ids;
    std::vector<int> z_ids;

    for (size_t i = 0; i < _states.size(); ++i) {
        // note: do _not_ refine y component since it is not functionally dependent
        x_ids.push_back(_params.addParameter(&(_states[i].get().ni(0))));
        z_ids.push_back(_params.addParameter(&(_states[i].get().ni(2))));
    }

    // record the constraints
    _constraints.push_back(x_ids);
    _constraints.push_back(z_ids);
}

bool RefinementBatch::refine(unsigned int max_iter)
{  
    Minimizer min;

    min.setxTol(1e-10);
    min.setfTol(1e-10);
    min.setgTol(1e-10);

    if (_constraints.size() > 0) {
        _params.setKernel(constraintKernel());
    }

    _cost_function.clear();
    _cost_function.shrink_to_fit();

    min.initialize(_params, _peaks.size()*3);
    min.set_f([&](Eigen::VectorXd& fvec) {return residuals(fvec);});
    bool success = min.fit(max_iter);
    for (auto state : _states) {
        state.get().refined = success;
    }

    *_cell = _cell->fromParameters(_u0, _uOffsets, _cellParameters);

    return success;
}

int RefinementBatch::residuals(Eigen::VectorXd &fvec)
{
    UnitCell uc = _cell->fromParameters(_u0, _uOffsets, _cellParameters);
    Eigen::Matrix3d UB = uc.reciprocalBasis();

    //#pragma omp parallel for
    for (unsigned int i = 0; i < _peaks.size(); ++i) {
        const Eigen::RowVector3d q0 = _peaks[i]->q().rowVector();
        const Eigen::RowVector3d q1 = _hkls[i]*UB;
        const Eigen::RowVector3d dq = _wts[i]*(q1-q0).transpose();

        if (dq.squaredNorm() < 10.0) {
            fvec(3*i)   = dq[0];
            fvec(3*i+1) = dq[1];
            fvec(3*i+2) = dq[2];
        }
        else {
            fvec(3*i) = 0.0;
            fvec(3*i+1) = 0.0;
            fvec(3*i+2) = 0.0;
        }
    }

    _cost_function.push_back(0.5*fvec.norm());

    return 0;
}

const std::vector<double>& RefinementBatch::costFunction() const
{
    return _cost_function;
}

const PeakList& RefinementBatch::peaks() const
{
    return _peaks;
}

sptrUnitCell RefinementBatch::cell() const
{
    return _cell;
}
    
Eigen::MatrixXd RefinementBatch::constraintKernel() const
{
    const int nparams = _params.nparams();
    std::vector<bool> is_free(nparams, true);
    std::vector<std::vector<double>> columns;

    // columns corresponding to the constrained parameters
    for (auto&& constraint: _constraints) {
        std::vector<double> column(nparams, 0.0);

        for (auto idx: constraint) {
            column[idx] = 1.0;
            is_free[idx] = false;
        }

        columns.push_back(column);
    }

    // columns corresponding to the free parameters
    for (auto idx = 0; idx < nparams; ++idx) {
        if (!is_free[idx]) {
            continue;
        }
        std::vector<double> column(nparams, 0.0);
        column[idx] = 1.0;
        columns.push_back(column);
    }

    // pack columns into a matrix
    Eigen::MatrixXd K(nparams, columns.size());

    for (size_t j = 0;  j < columns.size(); ++j) {
        for (auto i = 0; i < nparams; ++i) {
            K(i, j) = columns[j][i];
        }
    }

    return K;
}


bool RefinementBatch::contains(double f) const
{
    return (f > _fmin) && (f < _fmax);
}

} // end namespace nsx
