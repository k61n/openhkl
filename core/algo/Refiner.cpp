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
    nsxlog(level, "residual_type          = ", static_cast<int>(residual_type));
    nsxlog(level, "refine_sample_position = ", refine_sample_position);
    nsxlog(level, "refine_sample_orientation = ", refine_sample_orientation);
    nsxlog(level, "refine_detector_offset = ", refine_detector_offset);
    nsxlog(level, "refine_ki              = ", refine_ki);
    nsxlog(level, "use_batch_cells        = ", use_batch_cells);
}

Refiner::Refiner(UnitCellHandler* cell_handler) : _cell_handler(cell_handler)
{
    _params = std::make_unique<RefinerParameters>();
}

sptrUnitCell Refiner::_getUnitCell(const std::vector<Peak3D*> peaks_subset)
{
    sptrUnitCell cell_ptr;
    if (_params->use_batch_cells) { // Make a new unit cell for this batch
        // We have already refined once, and all peaks have been assigned a batch
        // with its own cell
        std::map<const UnitCell*, int> cell_count;
        for (auto* peak : peaks_subset) {
            if (peak->unitCell()) {
                auto search = cell_count.find(peak->unitCell());
                if (search == cell_count.end())
                    cell_count.insert({peak->unitCell(), 1});
                else
                    search->second += 1;
            }
        }
        // Find cell that appears the most in this batch
        int max = 0;
        const UnitCell* best_cell;
        for (const auto [key, val] : cell_count) {
            if (val > max) {
                max = val;
                best_cell = key;
            }
        }
        cell_ptr = std::make_shared<UnitCell>(*best_cell);
    } else { // The refiner has been passed a unit cell
        // Starting from scratch, use the cell obtained from autoindexing
        cell_ptr = std::make_shared<UnitCell>(_unrefined_cell);
    }

    return cell_ptr;
}

void Refiner::makeBatches(
    InstrumentStateList& states, const std::vector<nsx::Peak3D*>& peaks, sptrUnitCell cell)
{
    _peaks = peaks;
    _unrefined_states.clear();
    _batches.clear();
    _tmp_vec = _cell_handler->extractBatchCells();

    _states = &states;
    _nframes = states.size();

    for (const InstrumentState& state : states)
        _unrefined_states.push_back(state);

    if (_params->use_batch_cells) {
        _cell = nullptr;
    } else {
        _cell = cell;
        if (_cell)
            _unrefined_cell = *_cell;
    }

    PeakFilter peak_filter;
    std::vector<nsx::Peak3D*> filtered_peaks = peak_filter.filterEnabled(peaks, true);
    if (_params->use_batch_cells)
        filtered_peaks = peak_filter.filterIndexed(filtered_peaks);
    else
        filtered_peaks = peak_filter.filterIndexed(filtered_peaks, _cell.get());

    std::sort(
        filtered_peaks.begin(), filtered_peaks.end(),
        [](const Peak3D* p1, const Peak3D* p2) -> bool {
            auto&& c1 = p1->shape().center();
            auto&& c2 = p2->shape().center();
            return c1[2] < c2[2];
        });

    const double batch_size = filtered_peaks.size() / double(_params->nbatches);
    nsxlog(Level::Info, "Batch size is ", batch_size, " peaks");
    size_t current_batch = 0;

    std::vector<nsx::Peak3D*> peaks_subset;

    // batch contains peaks from frame _fmin to _fmax + 2
    for (size_t i = 0; i < filtered_peaks.size(); ++i) {
        peaks_subset.push_back(filtered_peaks[i]);

        if (i + 1.1 >= (current_batch + 1) * batch_size) {

            sptrUnitCell cell_ptr = _getUnitCell(peaks_subset);
            RefinementBatch b(states, cell_ptr, peaks_subset);
            b.setResidualType(_params->residual_type);

            std::ostringstream oss;
            oss << "frames " << b.name();
            std::string name = oss.str();

            for (auto* peak : b.peaks())
                peak->setUnitCell(cell_ptr);

            _cell_handler->addUnitCell(name, cell_ptr, true);

            _batches.emplace_back(std::move(b));
            peaks_subset.clear();
            ++current_batch;
        }
    }
}

void Refiner::reconstructBatches(std::vector<Peak3D*> peaks)
{
    auto tmp = _cell_handler->extractBatchCells(); // we're discarding this
    _batches.clear();

    std::vector<nsx::Peak3D*> filtered_peaks = peaks;
    PeakFilter peak_filter;
    filtered_peaks = peak_filter.filterEnabled(peaks, true);
    filtered_peaks = peak_filter.filterIndexed(filtered_peaks);

    std::sort(
        filtered_peaks.begin(), filtered_peaks.end(),
        [](const Peak3D* p1, const Peak3D* p2) -> bool {
            auto&& c1 = p1->shape().center();
            auto&& c2 = p2->shape().center();
            return c1[2] < c2[2];
        });

    const double batch_size = filtered_peaks.size() / double(_tmp_vec.size());
    nsxlog(Level::Info, "Batch size is ", batch_size, " peaks");
    size_t current_batch = 0;

    std::vector<nsx::Peak3D*> peaks_subset;

    // batch contains peaks from frame _fmin to _fmax + 2
    for (size_t i = 0; i < filtered_peaks.size(); ++i) {
        peaks_subset.push_back(filtered_peaks[i]);

        if (i + 1.1 >= (current_batch + 1) * batch_size) {

            sptrUnitCell cell_ptr = _tmp_vec.at(current_batch);
            RefinementBatch b(*_states, cell_ptr, peaks_subset);
            std::ostringstream oss;
            oss << "frames " << b.name();
            std::string name = oss.str();

            b.setResidualType(_params->residual_type);

            for (auto* peak : b.peaks())
                peak->setUnitCell(cell_ptr);

            _cell_handler->addUnitCell(name, cell_ptr, true);

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
    _params->log(Level::Info);

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

    nsxlog(Level::Info, "Refiner::refine: ", _batches.size(), " batches");
    if (_batches.empty())
        return false;

    unsigned int failed_batches = 0;
    // #pragma omp parallel for
    for (auto&& batch : _batches) {
        if (!batch.refine(_params->max_iter)) {
            // #pragma omp atomic
            ++failed_batches;
        }
    }
    _first_refine = false;
    logChange();
    if (failed_batches > 0) { // Failed, replace the original batch cells
        reconstructBatches(_peaks);
        return false;
    } else {
        return true;
    }
}

const std::vector<RefinementBatch>& Refiner::batches() const
{
    return _batches;
}

int Refiner::updatePredictions(std::vector<Peak3D*> peaks) const
{
    nsxlog(Level::Info, "Refiner::updatePredictions");
    const PeakFilter peak_filter;
    std::vector<nsx::Peak3D*> filtered_peaks = peaks;
    filtered_peaks = peak_filter.filterEnabled(peaks, true);
    if (_cell)
        filtered_peaks = peak_filter.filterIndexed(filtered_peaks, _cell.get());
    else
        filtered_peaks = peak_filter.filterIndexed(filtered_peaks);

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
        const std::vector<DetectorEvent> events = algo::qVector2Events(
            q_pred, peak->dataSet()->instrumentStates(), peak->dataSet()->detector(), _nframes);

        if (events.size() == 0) { // No event found
            peak->setSelected(false);
            peak->setRejectionFlag(RejectionFlag::PredictionUpdateFailure);
            continue;
        } else if (events.size() > 1) { // Some peaks cross the Ewald sphere more than once
            for (auto event : events) {
                auto vec = peak->shape().center() - Eigen::Vector3d(event.px, event.py, event.frame);
                if (vec.norm() < _eps_norm){
                    peak->setShape(
                        Ellipsoid({event.px, event.py, event.frame}, peak->shape().metric()));
                    ++updated;
                }
            }
        } else {
            peak->setShape(
                Ellipsoid({events[0].px, events[0].py, events[0].frame},
                          peak->shape().metric()));
            ++updated;
        }
    }
    nsxlog(Level::Info, updated, " peaks updated");
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

bool Refiner::firstRefine() const
{
    return _first_refine;
}

void Refiner::logChange()
{
    nsxlog(Level::Info, "Refinement succeeded");
    if (!_params->use_batch_cells)
        nsxlog(Level::Info, "Original cell: ", _unrefined_cell.toString());
    nsxlog(Level::Info, "Batch/Refined cell(s):");
    for (const auto& batch : _batches) {
        nsxlog(Level::Info, batch.name(), ": ", batch.cell()->toString());
    }
    Eigen::IOFormat vec3(6, 0, ", ", "\n", "[", "]");
    nsxlog(Level::Info, "Frame/k_i:");
    for (int i = 0; i < _states->size(); ++i) {
        Eigen::Vector3d k_i_change =
            _unrefined_states[i].ki().rowVector() - (*_states)[i].ki().rowVector();
        nsxlog(Level::Info, i + 1, ": ", k_i_change.transpose().format(vec3));
    }
    nsxlog(Level::Info, "Frame/Detector position:");
    for (int i = 0; i < _states->size(); ++i) {
        Eigen::Vector3d detector_pos_change =
            _unrefined_states[i].detectorPositionOffset - (*_states)[i].detectorPositionOffset;
        nsxlog(Level::Info, i + 1, ": ", detector_pos_change.transpose().format(vec3));
    }
    nsxlog(Level::Info, "Frame/Sample position:");
    for (int i = 0; i < _states->size(); ++i) {
        Eigen::Vector3d sample_pos_change =
            _unrefined_states[i].samplePosition - (*_states)[i].samplePosition;
        nsxlog(Level::Info, i + 1, ": ", sample_pos_change.transpose().format(vec3));
    }
    nsxlog(Level::Info, "Frame/Sample orienation:");
    for (int i = 0; i < _states->size(); ++i) {
        Eigen::Matrix3d sample_orientation_change = _unrefined_states[i].sampleOrientationMatrix()
            - (*_states)[i].sampleOrientationMatrix();
        nsxlog(Level::Info, i + 1, ":\n ", sample_orientation_change.transpose().format(vec3));
    }
}

RefinerParameters* Refiner::parameters()
{
    return _params.get();
}

void Refiner::setParameters(const RefinerParameters& params)
{
    _params->nbatches = params.nbatches;
    _params->max_iter = params.max_iter;
    _params->refine_ub = params.refine_ub;
    _params->residual_type = params.residual_type;
    _params->refine_sample_orientation = params.refine_sample_orientation;
    _params->refine_sample_position = params.refine_sample_position;
    _params->refine_detector_offset = params.refine_detector_offset;
    _params->refine_ki = params.refine_ki;
}

} // namespace nsx
