//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/algo/AutoIndexer.cpp
//! @brief     Implements class AutoIndexer
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/algo/AutoIndexer.h"
#include "base/fit/FitParameters.h"
#include "base/fit/Minimizer.h"
#include "base/utils/Logger.h"
#include "core/algo/FFTIndexing.h"
#include "core/data/DataSet.h" // peak->data()->interpolatedState
#include "core/shape/PeakFilter.h"
#include "tables/crystal/MillerIndex.h"
#include "core/instrument/InterpolatedState.h" // interpolate


#include <iomanip>
#include <iostream>
#include <string>

namespace nsx {

void IndexerParameters::log(const Level& level) const
{
    nsxlog(level, "Autoindexer parameters:");
    nsxlog(level, "maxdim             = ", maxdim);
    nsxlog(level, "nSolutions         = ", nSolutions);
    nsxlog(level, "nVertices          = ", nVertices);
    nsxlog(level, "subdiv             = ", subdiv);
    nsxlog(level, "indexingTolerance  = ", indexingTolerance);
    nsxlog(level, "niggliTolerance    = ", niggliTolerance);
    nsxlog(level, "gruberTolerance    = ", gruberTolerance);
    nsxlog(level, "niggliReduction    = ", niggliReduction);
    nsxlog(level, "minUnitCellVolume  = ", minUnitCellVolume);
    nsxlog(level, "unitCellEquivalenceTolerance = ", unitCellEquivalenceTolerance);
    nsxlog(level, "solutionCutoff     = ", solutionCutoff);
    nsxlog(level, "frequencyTolerance = ", frequencyTolerance);
    nsxlog(level, "first_frame        = ", first_frame);
    nsxlog(level, "last_frame         = ", last_frame);
    nsxlog(level, "d_min              = ", d_min);
    nsxlog(level, "d_max              = ", d_max);
    nsxlog(level, "strength_min       = ", strength_min);
    nsxlog(level, "strength_max       = ", strength_max);
}

AutoIndexer::AutoIndexer() : _solutions(), _handler(nullptr)
{
    _params = std::make_unique<IndexerParameters>();
}

IndexerParameters* AutoIndexer::parameters()
{
    return _params.get();
}

bool AutoIndexer::autoIndex(const std::vector<Peak3D*>& peaks)
{
    _params->log(Level::Info);
    nsxlog(Level::Info, "AutoIndexer::autoindex: indexing using ", peaks.size(), " peaks");
    // Find the Q-space directions along which the projection of the the Q-vectors
    // shows the highest periodicity
    bool success = computeFFTSolutions(peaks);
    if (!success)
        return success;
    refineSolutions(peaks);
    removeBad(_params->solutionCutoff);

    // refine the constrained unit cells in order to get the uncertainties
    // refineConstraints();

    // finally, rank the solutions
    rankSolutions();
    nsxlog(Level::Info, "AutoIndexer::autoindex: ", _solutions.size(), " unit cells found");
    nsxlog(Level::Info, solutionsToString());
    return success;
}

bool AutoIndexer::autoIndex(PeakCollection* peaks)
{
    nsxlog(Level::Info, "AutoIndexer::autoindex: indexing PeakCollection '", peaks->name(), "'");
    std::vector<Peak3D*> peak_list = peaks->getPeakList();
    return autoIndex(peak_list);
}

void AutoIndexer::removeBad(double quality)
{
    // remove the bad solutions
    auto remove =
        std::remove_if(_solutions.begin(), _solutions.end(), [=](const RankedSolution& s) {
            return s.second < quality;
        });
    _solutions.erase(remove, _solutions.end());
}

const std::vector<std::pair<sptrUnitCell, double>>& AutoIndexer::solutions() const
{
    return _solutions;
}

bool AutoIndexer::computeFFTSolutions(const std::vector<Peak3D*>& peaks)
{
    _solutions.clear();

    // Store the q-vectors of the peaks for auto-indexing
    std::vector<ReciprocalVector> qvects;
    const std::vector<Peak3D*> filtered_peaks = PeakFilter{}.filterEnabled(peaks, true);
    for (const Peak3D* peak : filtered_peaks) {
        auto q = peak->q().rowVector();
        qvects.emplace_back(ReciprocalVector(q));
    }

    // Check that a minimum number of peaks have been selected for indexing
    if (qvects.size() < 10) {
        nsxlog(Level::Info, "AutoIndexer::computeFFTSolutions: Too few peaks to autoindex");
        return false;
    }

    // Find the best vectors via FFT
    std::vector<Eigen::RowVector3d> tvects = algo::findOnSphere(
        qvects, _params->nVertices, _params->nSolutions, _params->subdiv, _params->maxdim,
        _params->frequencyTolerance);

    // Need at least 3 t-vectors to form a basis
    if (tvects.size() < 3) {
        nsxlog(Level::Info, "AutoIndexer::computeFFTSolutions: Too few t-vectors to form basis");
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
                std::shared_ptr<UnitCell> cell{new UnitCell{A}};

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

void AutoIndexer::refineSolutions(const std::vector<Peak3D*>& peaks)
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
        std::vector<Peak3D*> enabled_peaks = peak_filter.filterEnabled(peaks, true);

        std::vector<Peak3D*> filtered_peaks =
            peak_filter.filterIndexed(enabled_peaks, cell);

        int success = filtered_peaks.size();
        for (const auto* peak : filtered_peaks) {
            MillerIndex hkld(peak->q(), *cell);
            hkls.emplace_back(hkld.rowVector().cast<double>());
            qs.emplace_back(peak->q().rowVector());

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
            peak_filter.filterIndexed(filtered_peaks, cell);

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
        << std::setw(10) << "gamma";
    for (const auto& solution : _solutions) {
        oss << std::endl
            << std::fixed << std::setw(10) << std::setprecision(3) << solution.second
            << solution.first->toString();
    }
    return oss.str();
}

void AutoIndexer::acceptSolution(sptrUnitCell solution, const std::vector<nsx::Peak3D*>& peaks)
{
    for (auto* peak : peaks)
        peak->setUnitCell(solution);
}

sptrUnitCell AutoIndexer::goodSolution(const UnitCell* reference_cell, double length_tol, double angle_tol)
{
    for (const auto& solution : _solutions) {
        if (solution.first->isSimilar(reference_cell, length_tol, angle_tol)) {
            return solution.first;
        }
    }
    return nullptr;
}

} // namespace nsx
