//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/algo/Refiner.cpp
//! @brief     Implements class Refiner
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <algorithm>
#include <iterator>

#include "base/utils/Logger.h"
#include "core/algo/Refiner.h"
#include "core/data/DataSet.h"
#include "core/detector/DetectorEvent.h"
#include "core/experiment/UnitCellHandler.h"
#include "core/instrument/InstrumentState.h"
#include "core/peak/Peak3D.h"
#include "core/peak/Qs2Events.h"
#include "core/shape/PeakFilter.h"
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/UnitCell.h"

namespace nsx {

Refiner::Refiner(
    InstrumentStateList& states, UnitCell* cell, std::vector<nsx::Peak3D*> peaks,
    int nbatches, UnitCellHandler* cell_handler)
    : _cell_handler(cell_handler), _cell(cell), _batches()
{
    _unrefined_cell = *_cell;
    const PeakFilter peak_filter;
    std::vector<nsx::Peak3D*> filtered_peaks = peaks;
    filtered_peaks = peak_filter.filterEnabled(peaks, true);
    filtered_peaks = peak_filter.filterIndexed(filtered_peaks, *cell, cell->indexingTolerance());

    std::sort(
        filtered_peaks.begin(), filtered_peaks.end(),
        [](const Peak3D* p1, const Peak3D* p2) -> bool {
            auto&& c1 = p1->shape().center();
            auto&& c2 = p2->shape().center();
            return c1[2] < c2[2];
        });

    const double batch_size = filtered_peaks.size() / double(nbatches);
    size_t current_batch = 0;

    std::vector<const nsx::Peak3D*> peaks_subset;

    int n_batch = 0;
    for (size_t i = 0; i < filtered_peaks.size(); ++i) {
        peaks_subset.push_back(filtered_peaks[i]);

        if (i + 1.1 >= (current_batch + 1) * batch_size) {

            // Make a new unit cell for this batch
            std::ostringstream oss;
            oss << "batch" << ++n_batch;
            std::string name = oss.str();
            _cell_handler->addUnitCell(name, _unrefined_cell);
 
            RefinementBatch b(states, _cell_handler->getUnitCell(name), peaks_subset);
            _batches.emplace_back(std::move(b));
            peaks_subset.clear();
            ++current_batch;
        }
    }
}

void Refiner::refineDetectorOffset()
{
    for (auto&& batch : _batches)
        batch.refineDetectorOffset();
}

void Refiner::refineSamplePosition()
{
    for (auto&& batch : _batches)
        batch.refineSamplePosition();
}

void Refiner::refineSampleOrientation()
{
    for (auto&& batch : _batches)
        batch.refineSampleOrientation();
}

void Refiner::refineKi()
{
    for (auto&& batch : _batches)
        batch.refineKi();
}

void Refiner::refineUB()
{
    for (auto&& batch : _batches)
        batch.refineUB();
}

bool Refiner::refine(unsigned int max_iter)
{
    if (_batches.size() == 0)
        return false;

    for (auto&& batch : _batches) {
        if (!batch.refine(max_iter))
            return false;
    }
    return true;
}

const std::vector<RefinementBatch>& Refiner::batches() const
{
    return _batches;
}

int Refiner::updatePredictions(std::vector<Peak3D*> peaks) const
{
    const PeakFilter peak_filter;
    std::vector<nsx::Peak3D*> filtered_peaks = peaks;
    filtered_peaks = peak_filter.filterEnabled(peaks, true);
    filtered_peaks = peak_filter.filterIndexed(filtered_peaks, *_cell, _cell->indexingTolerance());

    int updated = 0;

    for (nsx::Peak3D* peak : filtered_peaks) {
        // find appropriate batch
        const RefinementBatch* b = nullptr;
        const double z = peak->shape().center()[2];
        for (const auto& batch : _batches) {
            if (batch.contains(z)) {
                b = &batch;
                break;
            }
        }

        // no appropriate batch
        if (b == nullptr)
            continue;

        const auto* batch_cell = b->cell();

        // update the position
        const MillerIndex hkl(peak->q(), *batch_cell);
        const ReciprocalVector q_pred(
            hkl.rowVector().cast<double>() * batch_cell->reciprocalBasis());
        const std::vector<DetectorEvent> events = algo::qs2events(
            {q_pred}, peak->dataSet()->instrumentStates(), peak->dataSet()->detector());

        // something wrong with new prediction...
        if (events.size() != 1) {
            peak->setSelected(false);
            continue;
        }

        try {
            peak->setShape(Ellipsoid(
                {events[0]._px, events[0]._py, events[0]._frame}, peak->shape().metric()));
            ++updated;
        } catch (...) {
            peak->setSelected(false);
        }
    }
    return updated;
}

void Refiner::logChange()
{
    nsxlog(Level::Info, "Original cell:", _unrefined_cell.toString());
    nsxlog(Level::Info, "Batch/Refined cell(s):");
    for (const auto& batch : _batches) {
        nsxlog(Level::Info, batch.name(), batch.cell()->toString());
    }
}

} // namespace nsx
