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

void RefinerParameters::log(const Level& level) const
{
    nsxlog(level, "Refiner parameters:");
    nsxlog(level, "nbatches               = ", nbatches);
    nsxlog(level, "max_iter               = ", max_iter);
    nsxlog(level, "refine_ub              = ", refine_ub);
    nsxlog(level, "refine_sample_position = ", refine_sample_position);
    nsxlog(level, "refine_sample_orientation = ", refine_sample_orientation);
    nsxlog(level, "refine_detector_offset = ", refine_detector_offset);
    nsxlog(level, "refine_ki              = ", refine_ki);
}

Refiner::Refiner(
    InstrumentStateList& states, UnitCell* cell, const std::vector<nsx::Peak3D*>& peaks,
    UnitCellHandler* cell_handler, int nbatches)
    : _cell_handler(cell_handler), _cell(cell)
{
    _params = std::make_shared<RefinerParameters>();
    _params->nbatches = nbatches;
    _states = &states;
    for (const InstrumentState& state : states)
        _unrefined_states.push_back(state);
    _unrefined_cell = *_cell;
    _nframes = states.size();
    std::vector<nsx::Peak3D*> filtered_peaks = peaks;
    PeakFilter peak_filter;
    filtered_peaks = peak_filter.filterEnabled(peaks, true);
    filtered_peaks = peak_filter.filterIndexed(filtered_peaks, *cell, cell->indexingTolerance());

    std::sort(
        filtered_peaks.begin(), filtered_peaks.end(),
        [](const Peak3D* p1, const Peak3D* p2) -> bool {
            auto&& c1 = p1->shape().center();
            auto&& c2 = p2->shape().center();
            return c1[2] < c2[2];
        });

    const double batch_size = filtered_peaks.size() / double(_params->nbatches);
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
    nsxlog(Level::Info, "Refining detector offset");
    for (auto&& batch : _batches)
        batch.refineDetectorOffset();
}

void Refiner::refineSamplePosition()
{
    nsxlog(Level::Info, "Refining sample position");
    for (auto&& batch : _batches)
        batch.refineSamplePosition();
}

void Refiner::refineSampleOrientation()
{
    nsxlog(Level::Info, "Refining sample orientation");
    for (auto&& batch : _batches)
        batch.refineSampleOrientation();
}

void Refiner::refineKi()
{
    nsxlog(Level::Info, "Refining wavevector k_i");
    for (auto&& batch : _batches)
        batch.refineKi();
}

void Refiner::refineUB()
{
    nsxlog(Level::Info, "Refining lattice vectors");
    for (auto&& batch : _batches)
        batch.refineUB();
}

bool Refiner::refine()
{
    if (_params->refine_ub)
        refineUB();
    if (_params->refine_ki)
        refineKi();
    if (_params->refine_sample_position)
        refineSamplePosition();
    if (_params->refine_sample_orientation)
        refineSampleOrientation();
    if (_params->refine_detector_offset)
        refineDetectorOffset();

    nsxlog(Level::Info, "Refiner::refine: ", _batches.size(), "batches");
    if (_batches.empty())
        return false;

    for (auto&& batch : _batches) {
        if (!batch.refine(_params->max_iter))
            return false;
    }
    logChange();
    return true;
}

const std::vector<RefinementBatch>& Refiner::batches() const
{
    return _batches;
}

int Refiner::updatePredictions(std::vector<Peak3D*>& peaks) const
{
    nsxlog(Level::Info, "Refiner::updatePredictions");
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
            if (batch.onlyContains(z)) {
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
            {q_pred}, peak->dataSet()->instrumentStates(), peak->dataSet()->detector(), _nframes);

        // something wrong with new prediction...
        if (events.size() != 1) {
            peak->setSelected(false);
            peak->setRejectionFlag(RejectionFlag::PredictionUpdateFailure);
            continue;
        }

        try {
            peak->setShape(Ellipsoid(
                {events[0]._px, events[0]._py, events[0]._frame}, peak->shape().metric()));
            ++updated;
        } catch (...) {
            peak->setSelected(false);
            peak->setRejectionFlag(RejectionFlag::InvalidShape);
        }
    }
    nsxlog(Level::Info, updated, "peaks updated");
    return updated;
}

UnitCell* Refiner::unrefinedCell()
{
    return &_unrefined_cell;
}

InstrumentStateList* Refiner::refinedStates()
{
    return _states;
}

InstrumentStateList* Refiner::unrefinedStates()
{
    return &_unrefined_states;
}

int Refiner::nframes() const
{
    return _nframes;
}

void Refiner::logChange()
{
    nsxlog(Level::Info, "Refinement succeeded");
    nsxlog(Level::Info, "Original cell: ", _unrefined_cell.toString());
    nsxlog(Level::Info, "Batch/Refined cell(s):");
    for (const auto& batch : _batches) {
        nsxlog(Level::Info, batch.name(), batch.cell()->toString());
    }
    Eigen::IOFormat vec3(6, 0, ", ", "\n", "[", "]");
    nsxlog(Level::Info, "Frame/k_i:");
    for (int i = 0; i < _states->size(); ++i) {
        Eigen::Vector3d k_i_change =
            _unrefined_states[i].ki().rowVector() - (*_states)[i].ki().rowVector();
        nsxlog(Level::Info, i + 1, k_i_change.transpose().format(vec3));
    }
    nsxlog(Level::Info, "Frame/Detector position:");
    for (int i = 0; i < _states->size(); ++i) {
        Eigen::Vector3d detector_pos_change =
            _unrefined_states[i].detectorPositionOffset - (*_states)[i].detectorPositionOffset;
        nsxlog(Level::Info, i + 1, detector_pos_change.transpose().format(vec3));
    }
    nsxlog(Level::Info, "Frame/Sample position:");
    for (int i = 0; i < _states->size(); ++i) {
        Eigen::Vector3d sample_pos_change =
            _unrefined_states[i].samplePosition - (*_states)[i].samplePosition;
        nsxlog(Level::Info, i + 1, sample_pos_change.transpose().format(vec3));
    }
    nsxlog(Level::Info, "Frame/Sample orienation:");
    for (int i = 0; i < _states->size(); ++i) {
        Eigen::Matrix3d sample_orientation_change = _unrefined_states[i].sampleOrientationMatrix()
            - (*_states)[i].sampleOrientationMatrix();
        nsxlog(Level::Info, i + 1, "\n ", sample_orientation_change.transpose().format(vec3));
    }
}

void Refiner::setParameters(std::shared_ptr<RefinerParameters> params)
{
    _params = params;
}

RefinerParameters* Refiner::parameters()
{
    return _params.get();
}

} // namespace nsx
