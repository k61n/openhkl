//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/algo/Refiner.cpp
//! @brief     Implements class Refiner
//!
//! @homepage  https://openhkl.org
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
#include "core/data/DataTypes.h"
#include "core/detector/DetectorEvent.h"
#include "core/experiment/UnitCellHandler.h"
#include "core/instrument/InstrumentState.h"
#include "core/peak/Peak3D.h"
#include "core/peak/Qs2Events.h"
#include "core/shape/PeakFilter.h"
#include "tables/crystal/MillerIndex.h"


namespace ohkl {

void RefinerParameters::log(const Level& level) const
{
    ohklLog(level, "Refiner parameters:");
    ohklLog(level, "nbatches               = ", nbatches);
    ohklLog(level, "max_iter               = ", max_iter);
    ohklLog(level, "refine_ub              = ", refine_ub);
    ohklLog(level, "residual_type          = ", static_cast<int>(residual_type));
    ohklLog(level, "refine_sample_position = ", refine_sample_position);
    ohklLog(level, "refine_sample_orientation = ", refine_sample_orientation);
    ohklLog(level, "refine_detector_offset = ", refine_detector_offset);
    ohklLog(level, "refine_ki              = ", refine_ki);
    ohklLog(level, "use_batch_cells        = ", use_batch_cells);
    ohklLog(level, "set_unit_cell          = ", set_unit_cell);
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
        const UnitCell* best_cell = cell_ptr.get();
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
    InstrumentStateList& states, const std::vector<ohkl::Peak3D*>& peaks, sptrUnitCell cell)
{
    ohklLog(Level::Info, "Refiner::makeBatches: making ", _params->nbatches, " batches");
    _peaks = peaks;
    _unrefined_states.clear();
    _batches.clear();

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
    std::vector<ohkl::Peak3D*> filtered_peaks = peak_filter.filterEnabled(peaks, true);
    if (_params->use_batch_cells)
        filtered_peaks = peak_filter.filterIndexed(filtered_peaks);
    else
        filtered_peaks = peak_filter.filterIndexed(filtered_peaks, _cell.get());

    ohklLog(Level::Info, filtered_peaks.size(), " / ", peaks.size(), " peaks used by refiner");

    std::sort(
        filtered_peaks.begin(), filtered_peaks.end(),
        [](const Peak3D* p1, const Peak3D* p2) -> bool {
            auto&& c1 = p1->shape().center();
            auto&& c2 = p2->shape().center();
            return c1[2] < c2[2];
        });

    const double batch_size = filtered_peaks.size() / double(_params->nbatches);
    ohklLog(Level::Info, "Batch size is ", batch_size, " peaks");
    size_t current_batch = 0;

    std::vector<ohkl::Peak3D*> peaks_subset;

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

            if (_params->set_unit_cell) {
                for (auto* peak : b.peaks())
                    peak->setUnitCell(cell_ptr);
            }

            _cell_handler->addUnitCell(name, cell_ptr);

            _batches.emplace_back(std::move(b));
            peaks_subset.clear();
            ++current_batch;
        }
    }
}

void Refiner::assignPredictedCells(std::vector<Peak3D*> predicted_peaks)
{
    for (const auto& batch : _batches) {
        for (auto* peak : predicted_peaks) {
            if (batch.onlyContains(peak->shape().center()[2]))
                peak->setUnitCell(batch.sptrCell());
        }
    }
}

void Refiner::reconstructBatches(std::vector<Peak3D*> peaks)
{
    _batches.clear();

    std::vector<ohkl::Peak3D*> filtered_peaks = peaks;
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
    ohklLog(Level::Info, "Batch size is ", batch_size, " peaks");
    size_t current_batch = 0;

    std::vector<ohkl::Peak3D*> peaks_subset;

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

            _cell_handler->addUnitCell(name, cell_ptr);

            _batches.emplace_back(std::move(b));
            peaks_subset.clear();
            ++current_batch;
        }
    }
}

void Refiner::refineDetectorOffset()
{
    ohklLog(Level::Info, "Refining detector offset");
    for (auto&& batch : _batches)
        batch.refineDetectorOffset();
}

void Refiner::refineSamplePosition()
{
    ohklLog(Level::Info, "Refining sample position");
    for (auto&& batch : _batches)
        batch.refineSamplePosition();
}

void Refiner::refineSampleOrientation()
{
    ohklLog(Level::Info, "Refining sample orientation");
    for (auto&& batch : _batches)
        batch.refineSampleOrientation();
}

void Refiner::refineKi()
{
    ohklLog(Level::Info, "Refining wavevector k_i");
    for (auto&& batch : _batches)
        batch.refineKi();
}

void Refiner::refineUB()
{
    ohklLog(Level::Info, "Refining lattice vectors");
    for (auto&& batch : _batches)
        batch.refineUB();
}

bool Refiner::refine(sptrDataSet data, const std::vector<Peak3D*> peaks, sptrUnitCell cell)
{
    _params->log(Level::Info);
    auto& states = data->instrumentStates();
    makeBatches(states, peaks, cell);

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

    ohklLog(Level::Info, "Refiner::refine: ", _batches.size(), " batches");
    if (_batches.empty())
        return false;

    unsigned int failed_batches = 0;
    for (auto&& batch : _batches) {
        if (!batch.refine(_params->max_iter)) {
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

int Refiner::updatePredictions(std::vector<Peak3D*> peaks)
{
    ohklLog(Level::Info, "Refiner::updatePredictions");
    assignPredictedCells(peaks); // Set the batch cells to the predicted peaks
    int updated = 0;

    for (ohkl::Peak3D* peak : peaks) {
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

        const sptrUnitCell batch_cell = b->sptrCell();

        // update the position
        const ReciprocalVector q_pred(
            peak->hkl().rowVector().cast<double>() * batch_cell->reciprocalBasis());
        const std::vector<DetectorEvent> events = algo::qVector2Events(
            q_pred, peak->dataSet()->instrumentStates(), peak->dataSet()->detector(), _nframes);

        if (events.size() == 0) { // No event found
            peak->setRejectionFlag(RejectionFlag::PredictionUpdateFailure);
            continue;
        } else if (events.size() > 1) { // Some peaks cross the Ewald sphere more than once
            for (auto event : events) {
                Eigen::Vector3d other = {event.px, event.py, event.frame};
                Eigen::Vector3d vec = peak->shape().center() - other;
                if (vec.norm() < _eps_norm) {
                    peak->setShape(
                        Ellipsoid({event.px, event.py, event.frame}, peak->shape().metric()));
                    peak->setUnitCell(batch_cell);
                    ++updated;
                }
            }
        } else {
            peak->setShape(
                Ellipsoid({events[0].px, events[0].py, events[0].frame}, peak->shape().metric()));
            peak->setUnitCell(batch_cell);
            ++updated;
        }
    }
    ohklLog(Level::Info, updated, " peaks updated");
    return updated;
}

const UnitCell* Refiner::unrefinedCell() const
{
    return &_unrefined_cell;
}

const InstrumentStateList* Refiner::refinedStates() const
{
    return _states;
}

const InstrumentStateList* Refiner::unrefinedStates() const
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
    ohklLog(Level::Debug, "Refinement succeeded");
    if (!_params->use_batch_cells)
        ohklLog(Level::Debug, "Original cell: ", _unrefined_cell.toString());
    ohklLog(Level::Debug, "Batch/Refined cell(s):");
    for (const auto& batch : _batches) {
        ohklLog(Level::Debug, batch.name(), ": ", batch.cell()->toString());
    }
    Eigen::IOFormat vec3(6, 0, ", ", "\n", "[", "]");
    ohklLog(Level::Debug, "Frame/k_i:");
    for (int i = 0; i < _states->size(); ++i) {
        Eigen::Vector3d k_i_change =
            _unrefined_states[i].ki().rowVector() - (*_states)[i].ki().rowVector();
        ohklLog(Level::Debug, i + 1, ": ", k_i_change.transpose().format(vec3));
    }
    ohklLog(Level::Debug, "Frame/Detector position:");
    for (int i = 0; i < _states->size(); ++i) {
        Eigen::Vector3d detector_pos_change =
            _unrefined_states[i].detectorPositionOffset - (*_states)[i].detectorPositionOffset;
        ohklLog(Level::Debug, i + 1, ": ", detector_pos_change.transpose().format(vec3));
    }
    ohklLog(Level::Debug, "Frame/Sample position:");
    for (int i = 0; i < _states->size(); ++i) {
        Eigen::Vector3d sample_pos_change =
            _unrefined_states[i].samplePosition - (*_states)[i].samplePosition;
        ohklLog(Level::Debug, i + 1, ": ", sample_pos_change.transpose().format(vec3));
    }
    ohklLog(Level::Debug, "Frame/Sample orientation:");
    for (int i = 0; i < _states->size(); ++i) {
        Eigen::Matrix3d sample_orientation_change = _unrefined_states[i].sampleOrientationMatrix()
            - (*_states)[i].sampleOrientationMatrix();
        ohklLog(Level::Debug, i + 1, ":\n ", sample_orientation_change.transpose().format(vec3));
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

} // namespace ohkl
