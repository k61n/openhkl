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

#include "InstrumentState.h"
#include "MillerIndex.h"
#include "Minimizer.h"
#include "PeakFilter.h"
#include "Peak3D.h"
#include "Refiner.h"
#include "UnitCell.h"

const static double g_eps = 1e-5;

namespace nsx {

RefinementBatch::RefinementBatch(const UnitCell& uc, const PeakList& peaks)
: _fmin(std::numeric_limits<double>().max()),
  _fmax(std::numeric_limits<double>().lowest()),
  _cell(new UnitCell(uc)),
  _peaks(peaks)
{
    for (auto peak : peaks) {
        const double z = peak->getShape().center()[2];
        _fmin = std::min(z, std::floor(_fmin));
        _fmax = std::max(z, std::ceil(_fmax));
    }

    // take care of floating point error
    _fmin -= g_eps;
    _fmax += g_eps;

    PeakFilter peak_filter;
    PeakList filtered_peaks;
    filtered_peaks = peak_filter.selected(peaks,true);
    filtered_peaks = peak_filter.indexed(filtered_peaks,_cell,_cell->indexingTolerance(),true);

    _hkls.reserve(filtered_peaks.size());
    for (auto peak : peaks) {
        MillerIndex hkl(peak->q(), *_cell);
        _hkls.push_back(hkl.rowVector().cast<double>());
    }

    UnitCell constrained = _cell->applyNiggliConstraints();
    _u0 = constrained.niggliOrientation();
    _cellParameters = constrained.parameters();
}

void RefinementBatch::refineU()
{
    for (int i = 0; i < _uOffsets.size(); ++i) {
        _params.addParameter(&_uOffsets(i));
    }
}

void RefinementBatch::refineB()
{
    for (int i = 0; i < _cellParameters.size(); ++i) {
        _params.addParameter(&_cellParameters(i));
    }
}

void RefinementBatch::refineDetectorOffset(InstrumentStateList& states)
{
    for (int axis = 0; axis < 3; ++axis) {
        std::vector<int> ids;
        for (size_t i = 0; i < states.size(); ++i) {
            if (!contains(i)) {
                continue;
            }
            int id = _params.addParameter(&states[i].detectorOffset(axis));
            ids.push_back(id);
        }
        // record the constraints
        _constraints.push_back(ids);
    }
}

void RefinementBatch::refineSamplePosition(InstrumentStateList& states)
{
    for (int axis = 0; axis < 3; ++axis) {
        std::vector<int> ids;
        for (size_t i = 0; i < states.size(); ++i) {
            if (!contains(i)) {
                continue;
            }
            int id = _params.addParameter(&states[i].samplePosition(axis));
            ids.push_back(id);
        }
        // record the constraints
        _constraints.push_back(ids);
    }
}

void RefinementBatch::refineSampleOrientation(InstrumentStateList& states)
{
    for (int axis = 0; axis < 3; ++axis) {
        std::vector<int> ids;
        for (size_t i = 0; i < states.size(); ++i) {
            if (!contains(i)) {
                continue;
            }
            int id = _params.addParameter(&states[i].sampleOrientationOffset(axis));
            ids.push_back(id);
        }
        // record the constraints
        _constraints.push_back(ids);
    }
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

    min.initialize(_params, _peaks.size()*3);
    min.set_f([&](Eigen::VectorXd& fvec) {return residuals(fvec);});
    bool success = min.fit(max_iter);   
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
        const Eigen::RowVector3d dq = q1-q0;

        fvec(3*i)   = dq[0];
        fvec(3*i+1) = dq[1];
        fvec(3*i+2) = dq[2];
    }
    return 0;
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

    // debugging
    std::cout << "kernel:\n" << K << std::endl;

    return K;
}


bool RefinementBatch::contains(double f) const
{
    return (f > _fmin) && (f < _fmax);
}

void RefinementBatch::refineKi(InstrumentStateList& states)
{
    std::vector<int> x_ids;
    std::vector<int> z_ids;

    for (size_t i = 0; i < states.size(); ++i) {
        if (!contains(i)) {
            continue;
        }
        // note: do _not_ refine y component since it is not functionally dependent
        x_ids.push_back(_params.addParameter(&states[i].ni(0)));
        z_ids.push_back(_params.addParameter(&states[i].ni(2)));    
    }

    // record the constraints
    _constraints.push_back(x_ids);
    _constraints.push_back(z_ids);
}

} // end namespace nsx
