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

Refiner::Refiner(sptrUnitCell cell, PeakList peaks):
    _params(),
    _cell(cell), _peaks(peaks), _hkl()
{
    for (auto p: _peaks) {
        if (p->getActiveUnitCell() != _cell ||  !p->isSelected() || p->isMasked()) {
            continue;
        }

        Eigen::RowVector3d hkl;
        
        if (_cell->getMillerIndices(p->getQ(), hkl)) {
            _hkl.push_back(hkl);
        }
    }

    UnitCell constrained = cell->applyNiggliConstraints();
    _u0 = constrained.niggliOrientation();
    _cellParameters = constrained.parameters();
}

void Refiner::refineU()
{
    for (int i = 0; i < _uOffsets.size(); ++i) {
        _params.addParameter(&_uOffsets(i));
    }
}

void Refiner::refineB()
{
    for (int i = 0; i < _cellParameters.size(); ++i) {
        _params.addParameter(&_cellParameters(i));
    }
}

void Refiner::refineDetectorStates(InstrumentStateList& states, unsigned int axis, unsigned int nbatches)
{
    for (auto i = 0; i < states.size(); ++i) {
        _params.addParameter(&states[i].detector._offsets(i));
    }
}

void Refiner::refineSampleState(InstrumentStateList& states, unsigned int axis, unsigned int nbatches)
{
    for (auto i = 0; i < states.size(); ++i) {
        _params.addParameter(&states[i].sample._offsets(i));
    }
}

void Refiner::refineSourceState(InstrumentStateList& states, unsigned int axis, unsigned int nbatches)
{
    for (auto i = 0; i < states.size(); ++i) {
        _params.addParameter(&states[i].source._offsets(i));
    }
}

bool Refiner::refine(unsigned int max_iter)
{  
    Minimizer min;

    min.setxTol(1e-10);
    min.setfTol(1e-10);
    min.setgTol(1e-10);

    min.initialize(_params, _peaks.size());
    min.set_f([&](Eigen::VectorXd& fvec) {return residuals(fvec);});
    bool success = min.fit(max_iter);
    if (success) {
        *_cell = _cell->fromParameters(_u0, _uOffsets, _cellParameters);        
    } else {
        _params.reset();
    }
    return success;
}

int Refiner::residuals(Eigen::VectorXd &fvec)
{
    UnitCell uc = _cell->fromParameters(_u0, _uOffsets, _cellParameters);
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

} // end namespace nsx
