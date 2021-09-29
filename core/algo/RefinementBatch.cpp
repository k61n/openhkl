//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/algo/RefinementBatch.cpp
//! @brief     Implements class RefinementBatch
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <limits>

#include "base/fit/Minimizer.h"
#include "core/algo/Refiner.h"
#include "core/data/DataSet.h"
#include "core/instrument/InstrumentState.h"
#include "core/instrument/InterpolatedState.h"
#include "core/peak/Peak3D.h"
#include "core/peak/Qs2Events.h"
#include "core/shape/PeakFilter.h"
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/UnitCell.h"

namespace {

static const double g_eps = 1e-5;

//! Returns the matrix of parameter constraints.
Eigen::MatrixXd constraintKernel(int nparams, const std::vector<std::vector<int>>& constraints)
{
    std::vector<bool> is_free(nparams, true);
    std::vector<std::vector<double>> columns;

    // columns corresponding to the constrained parameters
    for (const std::vector<int>& constraint : constraints) {
        std::vector<double> column(nparams, 0.0);
        for (const int idx : constraint) {
            column[idx] = 1.0;
            is_free[idx] = false;
        }
        columns.push_back(column);
    }

    // columns corresponding to the free parameters
    for (int idx = 0; idx < nparams; ++idx) {
        if (!is_free[idx])
            continue;
        std::vector<double> column(nparams, 0.0);
        column[idx] = 1.0;
        columns.push_back(column);
    }

    // pack columns into a matrix
    Eigen::MatrixXd K(nparams, columns.size());
    for (size_t j = 0; j < columns.size(); ++j) {
        for (auto i = 0; i < nparams; ++i)
            K(i, j) = columns[j][i];
    }

    return K;
}

} // namespace

namespace nsx {

RefinementBatch::RefinementBatch(
    InstrumentStateList& states, sptrUnitCell uc, std::vector<nsx::Peak3D*> peaks)
    : _fmin(std::numeric_limits<double>::max())
    , _fmax(std::numeric_limits<double>::lowest())
    , _residual_type(ResidualType::QSpace)
    , _cell(uc)
    , _peaks(peaks)
{
    for (auto* peak : peaks) {
        const double z = peak->shape().center()[2];
        _fmin = std::min(z, std::floor(_fmin));
        _fmax = std::max(z, std::ceil(_fmax));
    }

    // take care of floating point error
    _fmin -= g_eps;
    _fmax += g_eps;

    _hkls.reserve(_peaks.size());
    for (const auto* peak : _peaks) {
        MillerIndex hkl(peak->q(), *_cell);
        _hkls.emplace_back(hkl.rowVector().cast<double>());

        Eigen::Vector3d c = peak->shape().center();
        Eigen::Matrix3d M = peak->shape().metric();
        Eigen::Matrix3d J =
            InterpolatedState::interpolate(peak->dataSet()->instrumentStates(), c[2]).jacobianQ(c[0], c[1]);
        Eigen::Matrix3d JI = J.inverse();
        Eigen::Matrix3d A = JI.transpose() * M * JI;
        Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(A);
        Eigen::Matrix3d U = solver.eigenvectors();
        Eigen::Matrix3d D;
        D.setZero();
        for (auto i = 0; i < 3; ++i)
            D(i, i) = std::sqrt(solver.eigenvalues()[i]);
        _wts.emplace_back(U.transpose() * D * U);
    }

    UnitCell constrained = _cell->applyNiggliConstraints();
    _u0 = constrained.niggliOrientation();
    _uOffsets = Eigen::Vector3d::Zero();
    _cellParameters = constrained.parameters();

    _states.reserve(states.size());
    for (size_t i = 0; i < states.size(); ++i) {
        if (!contains(i))
            continue;
        _states.emplace_back(states[i]);
    }
}

void RefinementBatch::refineUB()
{
    for (int i = 0; i < _cellParameters.size(); ++i)
        _params.addParameter(&_cellParameters(i));
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
            int id =
                _params.addParameter(&(_states[i].get().sampleOrientationOffset.coeffs()[axis]));
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

    if (!_constraints.empty())
        _params.setKernel(constraintKernel(_params.nparams(), _constraints));

    _cost_function.clear();
    _cost_function.shrink_to_fit();

    min.initialize(_params, _peaks.size() * 3);
    min.set_f([&](Eigen::VectorXd& fvec) { return residuals(fvec); });
    bool success = min.fit(max_iter);

    for (const auto& state : _states)
        state.get().refined = success;

    _cell->updateParameters(_u0, _uOffsets, _cellParameters);

    return success;
}

int RefinementBatch::residuals(Eigen::VectorXd& fvec)
{
    if (_residual_type == ResidualType::QSpace) {
        return qSpaceResiduals(fvec);
    }
    else if (_residual_type == ResidualType::RealSpace) {
        return realSpaceResiduals(fvec);
    }
    else {
        return 0;
    }
}

int RefinementBatch::qSpaceResiduals(Eigen::VectorXd& fvec)
{
    UnitCell uc = _cell->fromParameters(_u0, _uOffsets, _cellParameters);
    const Eigen::Matrix3d& UB = uc.reciprocalBasis();

    //#pragma omp parallel for
    for (unsigned int i = 0; i < _peaks.size(); ++i) {
        const Eigen::RowVector3d q0 = _peaks[i]->q().rowVector();
        const Eigen::RowVector3d q1 = _hkls[i] * UB;
        const Eigen::RowVector3d dq = _wts[i] * (q1 - q0).transpose();

        if (dq.squaredNorm() < 10.0) {
            fvec(3 * i) = dq[0];
            fvec(3 * i + 1) = dq[1];
            fvec(3 * i + 2) = dq[2];
        } else {
            fvec(3 * i) = 0.0;
            fvec(3 * i + 1) = 0.0;
            fvec(3 * i + 2) = 0.0;
        }
    }

    _cost_function.push_back(0.5 * fvec.norm());

    return 0;
}

int RefinementBatch::realSpaceResiduals(Eigen::VectorXd& fvec)
{
    UnitCell uc = _cell->fromParameters(_u0, _uOffsets, _cellParameters);
    const Eigen::Matrix3d& UB = uc.reciprocalBasis();

    //#pragma omp parallel for
    for (unsigned int i = 0; i < _peaks.size(); ++i) {
        const Eigen::RowVector3d x0 = _peaks[i]->shape().center();
        auto data = _peaks[i]->dataSet();
        ReciprocalVector q1{_hkls[i] * UB};

        std::vector<DetectorEvent> events =
            algo::qVector2Events(q1, data->instrumentStates(), data->detector(), data->nFrames());
        std::vector<Eigen::RowVector3d> differences;
        for (auto&& event : events) {
            const Eigen::RowVector3d x1{event.px, event.py, event.frame};
            const Eigen::RowVector3d dx = x1 - x0;
            differences.push_back(dx);
        }
        double minNorm = 10000;
        int minInd = 0;
        if (!differences.empty()) {
            for (int i = 0; i < differences.size(); ++i) {
                double norm = differences[i].squaredNorm();
                if (norm < minNorm) {
                    minNorm = norm;
                    minInd = i;
                }
            }
        } else
            continue;

        fvec(3 * i) = 0.0;
        fvec(3 * i + 1) = 0.0;
        fvec(3 * i + 2) = 0.0;
        if (differences[minInd].squaredNorm() < 10) {
            fvec(3 * i) = differences[minInd][0];
            fvec(3 * i + 1) = differences[minInd][1];
            fvec(3 * i + 2) = differences[minInd][2];
        }
    }

    _cost_function.push_back(0.5 * fvec.norm());

    return 0;
}

std::vector<nsx::Peak3D*> RefinementBatch::peaks() const
{
    return _peaks;
}

UnitCell* RefinementBatch::cell() const
{
    return _cell.get();
}

int RefinementBatch::first_frame() const
{
    return _fmin;
}

int RefinementBatch::last_frame() const
{
    return _fmax;
}

bool RefinementBatch::contains(double f) const
{
    return (f > _fmin) && (f < _fmax);
}

bool RefinementBatch::onlyContains(double f) const
{
    return (f >= _fmin) && (f < _fmax - 2.0);
}

std::string RefinementBatch::name() const
{
    std::ostringstream oss;
    oss << std::setw(3) << std::setfill('0') << std::lround(_fmin) << " - "
        << std::setw(3) << std::setfill('0') << std::lround(_fmax);
    return oss.str();
}

void RefinementBatch::setResidualType(const ResidualType& residual)
{
    _residual_type = residual;
}

} // namespace nsx
