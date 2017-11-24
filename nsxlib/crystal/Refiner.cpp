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


#include "InstrumentState.h"
#include "Minimizer.h"
#include "Peak3D.h"
#include "Refiner.h"
#include "UnitCell.h"

namespace nsx {

RefinementBatch::RefinementBatch(const UnitCell& uc, const PeakList& peaks, double fmin, double fmax):
    _fmin(fmin), _fmax(fmax)
{
    for (auto p: peaks) {
        // skip if not selected or masked
        if (!p->isSelected() || p->isMasked()) {
            continue;
        }

        auto bb = p->getShape().aabb();
        auto lo = bb.lower();
        auto hi = bb.upper();

        // skip if peak out of range
        if (lo[2] < _fmin || hi[2] >= _fmax) {
            continue;
        }

        Eigen::RowVector3d hkl;
        
        if (uc.getMillerIndices(p->getQ(), hkl)) {
            _hkl.push_back(hkl);
            _peaks.push_back(p);
        }
    }
    UnitCell constrained = uc.applyNiggliConstraints();
    _u0 = constrained.niggliOrientation();
    _cellParameters = constrained.parameters();
}

Refiner::Refiner(sptrUnitCell cell, const PeakList& peaks, int nbatches): _batches()
{
    const double df = 1.0 / double(nbatches);

    for (int i = 0; i < nbatches; ++i) {
        RefinementBatch b(*cell, peaks, i*df, (i+1)*df);
        _batches.emplace_back(std::move(b));
    }
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

void RefinementBatch::refineDetectorState(InstrumentStateList& states, unsigned int axis)
{
    for (auto i = 0; i < states.size(); ++i) {
        if (i < _fmin || i >= _fmax) {
            continue;
        }
        _params.addParameter(&states[i].detector._offsets(axis));
    }
}

void RefinementBatch::refineSampleState(InstrumentStateList& states, unsigned int axis)
{
    for (auto i = 0; i < states.size(); ++i) {
        if (i < _fmin || i >= _fmax) {
            continue;
        }
        _params.addParameter(&states[i].sample._offsets(axis));
    }
}

void RefinementBatch::refineSourceState(InstrumentStateList& states, unsigned int axis)
{
    for (auto i = 0; i < states.size(); ++i) {
        if (i < _fmin || i >= _fmax) {
            continue;
        }
        _params.addParameter(&states[i].source._offsets(axis));
    }
}

bool RefinementBatch::refine(unsigned int max_iter)
{  
    Minimizer min;

    min.setxTol(1e-10);
    min.setfTol(1e-10);
    min.setgTol(1e-10);

    min.initialize(_params, _peaks.size()*3);
    min.set_f([&](Eigen::VectorXd& fvec) {return residuals(fvec);});
    bool success = min.fit(max_iter);
    if (success) {
        _cell = _cell.fromParameters(_u0, _uOffsets, _cellParameters);        
    } else {
        _params.reset();
    }
    return success;
}

int RefinementBatch::residuals(Eigen::VectorXd &fvec)
{
    UnitCell uc = _cell.fromParameters(_u0, _uOffsets, _cellParameters);
    Eigen::Matrix3d UB = uc.reciprocalBasis();

    //#pragma omp parallel for
    for (unsigned int i = 0; i < _peaks.size(); ++i) {
        const Eigen::RowVector3d q0 = static_cast<const Eigen::RowVector3d&>(_peaks[i]->getQ());
        const Eigen::RowVector3d q1 = _hkl[i]*UB;
        const Eigen::RowVector3d dq = q1-q0;

        fvec(3*i)   = dq[0];
        fvec(3*i+1) = dq[1];
        fvec(3*i+2) = dq[2];
    }
    return 0;
}

void Refiner::refineDetectorState(InstrumentStateList& states, unsigned int axis)
{
    for (auto&& batch: _batches) {
        batch.refineDetectorState(states, axis);
    }
}

void Refiner::refineSampleState(InstrumentStateList& states, unsigned int axis)
{
    for (auto&& batch: _batches) {
        batch.refineSampleState(states, axis);
    }
}

void Refiner::refineSourceState(InstrumentStateList& states, unsigned int axis)
{
    for (auto&& batch: _batches) {
        batch.refineSourceState(states, axis);
    }
}

bool Refiner::refine(unsigned int max_iter)
{  
    for (auto&& batch: _batches) {
        if (!batch.refine(max_iter)) {
            return false;
        }    
    }
    return true;
}

void Refiner::refineU()
{  
    for (auto&& batch: _batches) {
        batch.refineU(); 
    }
}

void Refiner::refineB()
{  
    for (auto&& batch: _batches) {
        batch.refineB();
    }
}

} // end namespace nsx
