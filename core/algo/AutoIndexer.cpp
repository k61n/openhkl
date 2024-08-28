//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/algo/AutoIndexer.cpp
//! @brief     Implements class AutoIndexer
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/algo/AutoIndexer.h"
#include "base/fit/FitParameters.h"
#include "base/fit/Minimizer.h"
#include "base/geometry/ReciprocalVector.h"
#include "base/utils/Logger.h"
#include "core/algo/FFTIndexing.h"
#include "core/data/DataSet.h" // peak->data()->interpolatedState
#include "core/instrument/InstrumentState.h"
#include "core/instrument/InterpolatedState.h" // interpolate
#include "core/peak/Peak3D.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/PeakFilter.h"
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/UnitCell.h"

#include <iomanip>
#include <string>

namespace ohkl {

void IndexerParameters::log(const Level& level) const
{
    ohklLog(level, "Autoindexer parameters:");
    ohklLog(level, "maxdim             = ", maxdim);
    ohklLog(level, "nSolutions         = ", nSolutions);
    ohklLog(level, "nVertices          = ", nVertices);
    ohklLog(level, "subdiv             = ", subdiv);
    ohklLog(level, "indexingTolerance  = ", indexingTolerance);
    ohklLog(level, "niggliTolerance    = ", niggliTolerance);
    ohklLog(level, "gruberTolerance    = ", gruberTolerance);
    ohklLog(level, "niggliReduction    = ", niggliReduction);
    ohklLog(level, "minUnitCellVolume  = ", minUnitCellVolume);
    ohklLog(level, "unitCellEquivalenceTolerance = ", unitCellEquivalenceTolerance);
    ohklLog(level, "solutionCutoff     = ", solutionCutoff);
    ohklLog(level, "frequencyTolerance = ", frequencyTolerance);
    ohklLog(level, "first_frame        = ", first_frame);
    ohklLog(level, "last_frame         = ", last_frame);
    ohklLog(level, "d_min              = ", d_min);
    ohklLog(level, "d_max              = ", d_max);
    ohklLog(level, "strength_min       = ", strength_min);
    ohklLog(level, "strength_max       = ", strength_max);
    ohklLog(level, "peaks_integrated   = ", peaks_integrated);
}

AutoIndexer::AutoIndexer() : _solutions(), _handler(nullptr)
{
    _params = std::make_unique<IndexerParameters>();
}

IndexerParameters* AutoIndexer::parameters()
{
    return _params.get();
}

bool AutoIndexer::autoIndex(
    const std::vector<Peak3D*>& peaks, sptrDataSet data, const InstrumentState* state, bool filter)
{
    _params->log(Level::Info);
    ohklLog(Level::Info, "AutoIndexer::autoIndex: indexing using ", peaks.size(), " peaks");
    _filtered_peaks.clear();
    _data = data;
    if (state)
        ohklLog(
            Level::Info, "AutoIndexer::autoIndex: indexing using InstrumentState\n",
            state->toString());
    if (filter)
        filterPeaks(peaks, state);
    else
        for (auto peak : peaks)
            _filtered_peaks.push_back(peak);
    // Find the Q-space directions along which the projection of the the Q-vectors
    // shows the highest periodicity
    bool success = computeFFTSolutions(_filtered_peaks, state);
    if (!success)
        return success;
    refineSolutions(_filtered_peaks, state);
    removeBad(_params->solutionCutoff);

    // refine the constrained unit cells in order to get the uncertainties
    // refineConstraints();

    // finally, rank the solutions
    rankSolutions();

    ohklLog(Level::Info, "AutoIndexer::autoindex: ", _solutions.size(), " unit cells found");
    ohklLog(Level::Info, solutionsToString());
    return success;
}

bool AutoIndexer::autoIndex(
    PeakCollection* peaks, sptrDataSet data, const InstrumentState* state, bool filter)
{
    ohklLog(Level::Info, "AutoIndexer::autoindex: indexing PeakCollection '", peaks->name(), "'");
    _params->peaks_integrated = peaks->isIntegrated();
    std::vector<Peak3D*> peak_list = peaks->getPeakList();

    if (autoIndex(peak_list, data, state, filter)) {
        peaks->setIndexed(true);
        return true;
    }
    peaks->setIndexed(false);
    return false;
}

void AutoIndexer::removeBad(double quality)
{
    // remove the bad solutions
    auto remove =
        std::remove_if(_solutions.begin(), _solutions.end(), [=](const RankedSolution& s) {
            return s.second < quality;
        });
    _solutions.erase(remove, _solutions.end());
    ohklLog(Level::Info, "AutoIndexer::removeBad: ", _solutions.size(), " solutions remaining");
}

const std::vector<std::pair<sptrUnitCell, double>>& AutoIndexer::solutions() const
{
    return _solutions;
}

void AutoIndexer::filterPeaks(const std::vector<Peak3D*>& peaks, const InstrumentState* state)
{
    std::vector<Peak3D*> tmp_peaks;
    int last = _params->last_frame == -1 ? _data->nFrames() : _params->last_frame;
    const std::vector<Peak3D*> frame_range_peaks =
        PeakFilter{}.filterFrameRange(peaks, _params->first_frame, last);
    ohklLog(
        Level::Info, "AutoIndexer::filterPeaks: ", frame_range_peaks.size(),
        " peaks in frame range");
    const std::vector<Peak3D*> enabled_peaks = PeakFilter{}.filterEnabled(frame_range_peaks, true);
    ohklLog(Level::Info, "AutoIndexer::filterPeaks: ", enabled_peaks.size(), " enabled peaks");
    if (!_params->peaks_integrated) {
        ohklLog(Level::Info, "AutoIndexer::filterPeaks: filtering by resolution");
        _filtered_peaks =
            PeakFilter{}.filterDRange(enabled_peaks, _params->d_min, _params->d_max, state);
    } else {
        ohklLog(Level::Info, "AutoIndexer::filterPeaks: filtering by resolution and strength");
        tmp_peaks = PeakFilter{}.filterStrength(
            enabled_peaks, _params->strength_min, _params->strength_max);
        _filtered_peaks =
            PeakFilter{}.filterDRange(tmp_peaks, _params->d_min, _params->d_max, state);
    }
    ohklLog(
        Level::Info, "AutoIndexer::filterPeaks: ", _filtered_peaks.size(),
        " peaks used in indexing");
}

bool AutoIndexer::computeFFTSolutions(
    const std::vector<Peak3D*>& peaks, const InstrumentState* state)
{
    _solutions.clear();

    // Store the q-vectors of the peaks for auto-indexing
    std::vector<ReciprocalVector> qvects;
    for (const Peak3D* peak : peaks) {
        ReciprocalVector qvec;
        if (state)
            qvec = peak->q(*state);
        else
            qvec = peak->q();
        qvects.emplace_back(qvec);
    }
    ohklLog(Level::Info, "AutoIndexer::computeFFTSolutions: ", qvects.size(), " q vectors");

    // Check that a minimum number of peaks have been selected for indexing
    if (qvects.size() < 10) {
        ohklLog(Level::Info, "AutoIndexer::computeFFTSolutions: Too few peaks to autoindex");
        return false;
    }

    // Find the best vectors via FFT
    std::vector<Eigen::RowVector3d> tvects = algo::findOnSphere(
        qvects, _params->nVertices, _params->nSolutions, _params->subdiv, _params->maxdim,
        _params->frequencyTolerance);
    ohklLog(Level::Info, "AutoIndexer::computeFFTSolutions: ", tvects.size(), " t vectors");

    // Need at least 3 t-vectors to form a basis
    if (tvects.size() < 3) {
        ohklLog(Level::Info, "AutoIndexer::computeFFTSolutions: Too few t-vectors to form basis");
        return false;
    }

    for (int i = 0; i < _params->nSolutions; ++i) {
        for (int j = i + 1; j < _params->nSolutions; ++j) {
            for (int k = j + 1; k < _params->nSolutions; ++k) {
                // Build up a unit cell out of the current directions triplet
                Eigen::Matrix3d A;
                A.col(0) = tvects[i];
                A.col(1) = tvects[j];
                A.col(2) = tvects[k];
                // Build a unit cell with direct vectors
                std::shared_ptr<UnitCell> cell{new UnitCell{A, _data}};

                // Skip this unit cell if its volume is below a user-defined minimum.
                if (cell->volume() < _params->minUnitCellVolume)
                    continue;

                // Skip this unit cell if there is already an equivalent one.
                bool equivalent = false;
                for (const auto& solution : _solutions) {
                    if (cell->equivalent(*solution.first, _params->unitCellEquivalenceTolerance)) {
                        equivalent = true;
                        break;
                    }
                }
                if (equivalent)
                    continue;

                // Add this unit cell to the list of solution. Score will be computed later.
                _solutions.push_back({cell, -1.0});
            }
        }
    }
    ohklLog(
        Level::Info, "AutoIndexer::computeFFTSolutions: ", _solutions.size(),
        " unrefined solutions");
    return true;
}

void AutoIndexer::rankSolutions()
{
    // Sort solutions by decreasing quality.
    // For equal quality, smallest volume is first
    std::sort(
        _solutions.begin(), _solutions.end(),
        [](const RankedSolution& s1, const RankedSolution& s2) -> bool {
            if (s1.second == s2.second)
                return (s1.first->volume() < s2.first->volume());
            return (s1.second > s2.second);
        });
}

void AutoIndexer::refineSolutions(const std::vector<Peak3D*>& peaks, const InstrumentState* state)
{
    // TODO: candidate for easy parallelization
    for (auto&& soln : _solutions) {
        UnitCell* cell = soln.first.get();
        cell->setIndexingTolerance(_params->indexingTolerance);
        Eigen::Matrix3d B = cell->reciprocalBasis();
        std::vector<Eigen::RowVector3d> hkls;
        std::vector<Eigen::RowVector3d> qs;
        std::vector<Eigen::Matrix3d> wt;

        PeakFilter peak_filter;
        std::vector<Peak3D*> filtered_peaks = peak_filter.filterIndexed(peaks, cell, state);

        int success = filtered_peaks.size();
        for (const auto* peak : filtered_peaks) {
            ReciprocalVector q;
            if (state)
                q = peak->q(*state);
            else
                q = peak->q();
            MillerIndex hkld(q, *cell);
            hkls.emplace_back(hkld.rowVector().cast<double>());
            qs.emplace_back(q.rowVector());

            Eigen::Vector3d c = peak->shape().center();
            Eigen::Matrix3d M = peak->shape().metric();
            InterpolatedState state =
                InterpolatedState::interpolate(peak->dataSet()->instrumentStates(), c[2]);
            if (!state.isValid())
                continue;

            Eigen::Matrix3d J = state.jacobianQ(c[0], c[1]);
            Eigen::Matrix3d JI = J.inverse();

            Eigen::Matrix3d A = JI.transpose() * M * JI;

            Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(A);
            Eigen::Matrix3d U = solver.eigenvectors();

            Eigen::Matrix3d D;
            D.setZero();

            for (auto i = 0; i < 3; ++i)
                D(i, i) = std::sqrt(solver.eigenvalues()[i]);

            wt.emplace_back(U.transpose() * D * U);
        }

        // The number of peaks must be at least for a proper minimization
        if (success < 10)
            continue;

        // Lambda to compute residuals
        auto residuals = [&B, &hkls, &qs](Eigen::VectorXd& f) -> int {
            int n = f.size() / 3;

            for (int i = 0; i < n; ++i) {
                // auto dq = wt[i]*(qs[i]-hkls[i]*B).transpose();
                auto dq = qs[i] - hkls[i] * B;
                f(3 * i + 0) = dq(0);
                f(3 * i + 1) = dq(1);
                f(3 * i + 2) = dq(2);
            }
            return 0;
        };

        // Pass by address the parameters to be fitted to the parameter store
        FitParameters params;
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c)
                params.addParameter(&B(r, c));
        }

        // Sets the Minimizer with the parameters store and the size of the residual vector
        Minimizer minimizer;
        minimizer.initialize(params, 3 * success);
        minimizer.set_f(residuals);
        minimizer.setxTol(1e-15);
        minimizer.setfTol(1e-15);
        minimizer.setgTol(1e-15);

        // fails to fit
        if (!minimizer.fit(500))
            continue;

        // Update the cell with the fitter one and reduce it
        try {
            cell->setReciprocalBasis(B);
            cell->setIndexingTolerance(_params->indexingTolerance);
            cell->reduce(
                _params->niggliReduction, _params->niggliTolerance, _params->gruberTolerance);
            *cell = cell->applyNiggliConstraints();
        } catch (std::exception& e) {
            continue;
        }

        // Define the final score of this solution by computing the percentage of
        // the selected peaks which have been successfully indexed

        std::vector<Peak3D*> refiltered_peaks =
            peak_filter.filterIndexed(filtered_peaks, cell, state);

        double score = static_cast<double>(refiltered_peaks.size());
        double maxscore = static_cast<double>(filtered_peaks.size());

        // Percentage of indexing
        score /= 0.01 * maxscore;
        soln.second = score;
    }
}

std::string AutoIndexer::solutionsToString() const
{
    std::ostringstream oss;
    oss << std::endl
        << std::setw(10) << "quality" << std::setw(10) << "a" << std::setw(10) << "b"
        << std::setw(10) << "c" << std::setw(10) << "alpha" << std::setw(10) << "beta"
        << std::setw(10) << "gamma" << std::setw(10) << "bravais";
    for (const auto& solution : _solutions) {
        oss << std::endl
            << std::fixed << std::setw(10) << std::setprecision(3) << solution.second
            << solution.first->toString();
    }
    return oss.str();
}

void AutoIndexer::acceptSolution(
    const sptrUnitCell solution, const std::vector<ohkl::Peak3D*>& peaks)
{
    for (auto* peak : peaks)
        peak->setUnitCell(solution);
}

void AutoIndexer::acceptSolution(const sptrUnitCell solution, PeakCollection* peaks)
{
    for (auto* peak : peaks->getPeakList())
        peak->setUnitCell(solution);
    peaks->setIndexed(true);
}

sptrUnitCell AutoIndexer::goodSolution(
    const UnitCell* reference_cell, double length_tol, double angle_tol)
{
    for (const auto& solution : _solutions) {
        if (solution.first->isSimilar(reference_cell, length_tol, angle_tol)) {
            return solution.first;
        }
    }
    return nullptr;
}

std::vector<RankedSolution> AutoIndexer::filterSolutions() const
{
    std::vector<RankedSolution> filtered;
    for (const auto& solution : _solutions) {
        if (solution.first->character().a < _params->maxdim
            && solution.first->character().b < _params->maxdim
            && solution.first->character().c < _params->maxdim) {
            if (solution.first->volume() > _params->minUnitCellVolume)
                filtered.emplace_back(solution);
        }
    }
    return filtered;
}

} // namespace ohkl
