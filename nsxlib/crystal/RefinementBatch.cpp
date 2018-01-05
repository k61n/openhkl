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

RefinementBatch::RefinementBatch(const UnitCell& uc, const PeakList& peaks, double fmin, double fmax)
: _fmin(fmin),
  _fmax(fmax),
  _peaks(peaks),
  _cell(uc)
{
    _hkls.reserve(peaks.size());
    for (auto p : peaks) {
        Eigen::RowVector3d hkl;
        uc.getMillerIndices(p->getQ(),hkl);
        _hkls.push_back(hkl);

    }

    UnitCell constrained = _cell.applyNiggliConstraints();
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
            if (i < _fmin || i >= _fmax) {
                continue;
            }
            int id = _params.addParameter(&states[i].detectorOffset(axis));
            ids.push_back(id);
        }
        // record the constraints
        for (size_t i = 1; i < ids.size(); ++i) {
            _constraints.push_back(std::make_pair(ids[i], ids[i-1]));
        }
    }
}

void RefinementBatch::refineSamplePosition(InstrumentStateList& states)
{
    for (int axis = 0; axis < 3; ++axis) {
        std::vector<int> ids;
        for (size_t i = 0; i < states.size(); ++i) {
            if (i < _fmin || i >= _fmax) {
                continue;
            }
            int id = _params.addParameter(&states[i].samplePosition(axis));
            ids.push_back(id);
        }
        // record the constraints
        for (size_t i = 1; i < ids.size(); ++i) {
            _constraints.push_back(std::make_pair(ids[i], ids[i-1]));
        }
    }
}

bool RefinementBatch::refine(unsigned int max_iter)
{  
    Minimizer min;

    min.setxTol(1e-10);
    min.setfTol(1e-10);
    min.setgTol(1e-10);

    if (_constraints.size() > 0) {
        auto C = constraints();
        _params.setConstraint(constraints());
    }

    min.initialize(_params, _peaks.size()*3);
    min.set_f([&](Eigen::VectorXd& fvec) {return residuals(fvec);});
    bool success = min.fit(max_iter);   
    _cell = _cell.fromParameters(_u0, _uOffsets, _cellParameters);        
    return success;
}

int RefinementBatch::residuals(Eigen::VectorXd &fvec)
{
    UnitCell uc = _cell.fromParameters(_u0, _uOffsets, _cellParameters);
    Eigen::Matrix3d UB = uc.reciprocalBasis();

    //#pragma omp parallel for
    for (unsigned int i = 0; i < _peaks.size(); ++i) {
        const Eigen::RowVector3d q0 = _peaks[i]->getQ().rowVector();
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

const UnitCell& RefinementBatch::cell() const
{
    return _cell;
}
    
UnitCell& RefinementBatch::cell()
{
    return _cell;
}

Eigen::MatrixXd RefinementBatch::constraints() const
{
    Eigen::MatrixXd C(_constraints.size(), _params.nparams());
    C.setZero();

    for (size_t i = 0; i < _constraints.size(); ++i) {
        auto id1 = _constraints[i].first;
        auto id2 = _constraints[i].second;
        C(i, id1) = 1.0;
        C(i, id2) = -1.0;
    }
    return C;   
}

bool RefinementBatch::contains(double f) const
{
    return f >= _fmin && f < _fmax;
}

} // end namespace nsx
