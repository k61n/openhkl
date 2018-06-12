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
#include <iterator>

#include "CrystalTypes.h"
#include "DataSet.h"
#include "DetectorEvent.h"
#include "InstrumentState.h"
#include "MillerIndex.h"
#include "Peak3D.h"
#include "PeakFilter.h"
#include "PeakPredictor.h"
#include "Refiner.h"
#include "UnitCell.h"

namespace nsx {

Refiner::Refiner(sptrUnitCell cell, const PeakList& peaks, int nbatches)
: _batches(), _cell(cell)
{
    PeakList sorted_peaks(peaks);

    auto sort_peaks_by_frame = [](sptrPeak3D p1, sptrPeak3D p2) -> bool {
        auto&& c1 = p1->shape().center();
        auto&& c2 = p2->shape().center();
        return c1[2] < c2[2];
    };

    std::sort(sorted_peaks.begin(),sorted_peaks.end(),sort_peaks_by_frame);

    double batch_size = sorted_peaks.size() / double(nbatches);
    size_t current_batch = 0;

    PeakList peaks_subset;

    for (size_t i = 0; i < sorted_peaks.size(); ++i) {
        peaks_subset.push_back(sorted_peaks[i]);

        if (i + 1.1 >= (current_batch+1)*batch_size) {
            RefinementBatch b(*cell, peaks_subset);
            _batches.emplace_back(std::move(b));
            peaks_subset.clear();
            ++current_batch;
        }
    }
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

void Refiner::refineSampleOrientation(InstrumentStateList& states)
{
    for (auto&& batch: _batches) {
        batch.refineSampleOrientation(states);
    }
}

bool Refiner::refine(unsigned int max_iter)
{ 
    if (_batches.size() == 0) {
        return false;
    }

    for (auto&& batch: _batches) {
        if (!batch.refine(max_iter)) {
            return false;
        }
    }
    return true;
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

int Refiner::updatePredictions(PeakList& peaks) const
{
    PeakFilter peak_filter;
    PeakList filtered_peaks;
    filtered_peaks = peak_filter.selected(peaks,true);
    filtered_peaks = peak_filter.predicted(filtered_peaks);
    filtered_peaks = peak_filter.unitCell(filtered_peaks,_cell);
    filtered_peaks = peak_filter.indexed(filtered_peaks,_cell,_cell->indexingTolerance());

    PeakList pred_peaks;
    int updated = 0;

    for (auto&& peak: filtered_peaks) {
                
        // find appropriate batch
        const RefinementBatch* b = nullptr;    
        double z = peak->shape().center()[2];
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
        MillerIndex hkl(peak->q(), *_cell);
        ReciprocalVector q_pred(hkl.rowVector().cast<double>()*_cell->reciprocalBasis());
        auto events = peak->data()->events({q_pred});

        // something wrong with new prediction...
        if (events.size() != 1) {
            continue;
        }
        
        try {
            peak->setShape(Ellipsoid({events[0]._px, events[0]._py, events[0]._frame}, peak->shape().metric()));
            ++updated;
        } catch(...) {
            peak->setSelected(false);
        }
        
    }
    return updated;
}

void Refiner::refineKi(InstrumentStateList& states)
{
    for (auto&& batch: _batches) {
        batch.refineKi(states); 
    }
}

} // end namespace nsx
