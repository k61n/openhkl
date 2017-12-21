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
#include "PeakPredictor.h"
#include "Refiner.h"
#include "UnitCell.h"

#include <limits>

namespace nsx {

RefinementBatch::RefinementBatch(const UnitCell& uc, const PeakList& peaks, double fmin, double fmax):
    _fmin(fmin), _fmax(fmax), _cell(uc)
{
    for (auto p: peaks) {
        // skip if not selected or masked
        if (!p->isSelected() || p->isMasked() || !p->isObserved()) {
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
    UnitCell constrained = _cell.applyNiggliConstraints();
    _u0 = constrained.niggliOrientation();
    _cellParameters = constrained.parameters();
}

Refiner::Refiner(sptrUnitCell cell, const PeakList& peaks, int nbatches): _batches(),  _cell(cell)
{   
    double fmin = std::numeric_limits<double>().max();
    double fmax = std::numeric_limits<double>().lowest();

    for (auto peak: peaks) {
        const double z = peak->getShape().center()[2];
        fmin = std::min(z, fmin);
        fmax = std::max(z, fmax);
    }

    fmin = std::floor(fmin);
    fmax = std::ceil(fmax);

    const double df = (fmax-fmin) / double(nbatches);

    for (int i = 0; i < nbatches; ++i) {
        RefinementBatch b(*cell, peaks, fmin + i*df, fmin + (i+1)*df);
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

void RefinementBatch::refineDetectorOffset(InstrumentStateList& states)
{
    for (int axis = 0; axis < 3; ++axis) {
        std::vector<int> ids;
        for (auto i = 0; i < states.size(); ++i) {
            if (i < _fmin || i >= _fmax) {
                continue;
            }
            int id = _params.addParameter(&states[i].detectorOffset(axis));
            ids.push_back(id);
        }
        // record the constraints
        for (int i = 1; i < ids.size(); ++i) {
            _constraints.push_back(std::make_pair(ids[i], ids[i-1]));
        }
    }
}

void RefinementBatch::refineSamplePosition(InstrumentStateList& states)
{
    for (int axis = 0; axis < 3; ++axis) {
        std::vector<int> ids;
        for (auto i = 0; i < states.size(); ++i) {
            if (i < _fmin || i >= _fmax) {
                continue;
            }
            int id = _params.addParameter(&states[i].samplePosition(axis));
            ids.push_back(id);
        }
        // record the constraints
        for (int i = 1; i < ids.size(); ++i) {
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
        // DEBUGGING
        std::cout << "constraint matrix for (" << _fmin << ", " << _fmax << ")" << std::endl;
        std::cout << "size: " << C.rows() << " x " << C.cols() << std::endl;
        std::cout << C << "\n--------------------------------------------------" << std::endl;
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
        const Eigen::RowVector3d q1 = _hkl[i]*UB;
        const Eigen::RowVector3d dq = q1-q0;

        fvec(3*i)   = dq[0];
        fvec(3*i+1) = dq[1];
        fvec(3*i+2) = dq[2];
    }
    return 0;
}

void Refiner::refineDetectorOffset(InstrumentStateList& states)
{
    for (auto&& batch: _batches) {
        batch.refineDetectorOffset(states);
    }
}

void Refiner::refineSamplePosition(InstrumentStateList& states)
{
    for (auto&& batch: _batches) {
        batch.refineSamplePosition(states);
    }
}

bool Refiner::refine(unsigned int max_iter)
{ 
    if (_batches.size() == 0) {
        return false;
    }

    UnitCell uc = _batches[0].cell();

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

const std::vector<RefinementBatch>& Refiner::batches() const
{
    return _batches;
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

    for (auto i = 0; i < _constraints.size(); ++i) {
        auto id1 = _constraints[i].first;
        auto id2 = _constraints[i].second;
        C(i, id1) = 1.0;
        C(i, id2) = -1.0;
    }
    return C;   
}

int Refiner::updatePredictions(PeakList& peaks) const
{
    PeakList pred_peaks;
    int updated = 0;

    for (auto&& peak: peaks) {
        if (peak->isObserved()) {
            continue;
        }
        if (!peak->isSelected() || peak->isMasked()) {
            continue;
        }
        if (peak->getActiveUnitCell() != _cell) {
            continue;
        }
                
        // find appropriate batch
        const RefinementBatch* b = nullptr;    
        double z = peak->getShape().center()[2];
        for (auto&& batch: _batches) {
            

            if (batch.contains(z)) {
                b = &batch;
                break;
            }
        }

        // no appropriate batch
        if (b == nullptr) {
            continue;
        }

        // update the position
        Eigen::RowVector3d hkl = b->cell().getIntegerMillerIndices(peak->getQ()).cast<double>();
        PeakPredictor predictor(peak->data());
        auto pred = predictor.predictPeaks({hkl}, b->cell().reciprocalBasis());

        // something wrong with new prediction...
        if (pred.size() != 1) {
            continue;
        }

        peak->setShape(Ellipsoid(pred[0]->getShape().center(), peak->getShape().metric()));
        ++updated;
    }
    return updated;
}

bool RefinementBatch::contains(double f) const
{
    return f >= _fmin && f < _fmax;
}

} // end namespace nsx
