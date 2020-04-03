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
#include "core/algo/FFTIndexing.h"
#include "core/data/DataSet.h" // peak->data()->interpolatedState
#include "core/shape/PeakFilter.h"
#include "tables/crystal/MillerIndex.h"

#include <iostream>
#include <string>
#include <iomanip>

namespace nsx {

AutoIndexer::AutoIndexer() : _solutions(), _handler(nullptr)
{
    _params = IndexerParameters();
}

void AutoIndexer::autoIndex(const std::vector<Peak3D*>& peaks)
{
    // Find the Q-space directions along which the projection of the the Q-vectors
    // shows the highest periodicity
    computeFFTSolutions(peaks);
    refineSolutions(peaks);
    removeBad(_params.solutionCutoff);

    // refine the constrained unit cells in order to get the uncertainties
    // refineConstraints();

    if (_handler)
        _handler->log("Done refining solutions, building solution table.");

    // finally, rank the solutions
    rankSolutions();
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

void AutoIndexer::computeFFTSolutions(const std::vector<Peak3D*>& peaks)
{
    _solutions.clear();

    // Store the q-vectors of the peaks for auto-indexing
    std::vector<ReciprocalVector> qvects;
    const std::vector<Peak3D*> filtered_peaks = PeakFilter {}.filterEnabled(peaks, true);
    for (const Peak3D* peak : filtered_peaks) {
        auto q = peak->q().rowVector();
        qvects.push_back(ReciprocalVector(q));
    }

    // Check that a minimum number of peaks have been selected for indexing
    if (qvects.size() < 10) {
        if (_handler)
            _handler->log("AutoIndexer: too few peaks to index!");
        throw std::runtime_error("Too few peaks to autoindex");
    }
    if (_handler)
        _handler->log(
            "Searching direct lattice vectors using" + std::to_string(qvects.size())
            + "peaks defined on numors:");

    // Find the best vectors via FFT
    std::vector<Eigen::RowVector3d> tvects = algo::findOnSphere(
        qvects, _params.nVertices, _params.nSolutions, _params.subdiv, _params.maxdim);

    // Need at least 3 t-vectors to form a basis
    if (tvects.size() < 3) {
        if (_handler)
            _handler->log(
                "Too few lattice planes detected to form a basis: tvects.size() = " +
                std::to_string(tvects.size()) + " (minimum 3)");
        throw std::runtime_error("Too few t-vectors to form basis");
    }

    for (int i = 0; i < _params.nSolutions; ++i) {
        for (int j = i + 1; j < _params.nSolutions; ++j) {
            for (int k = j + 1; k < _params.nSolutions; ++k) {
                // Build up a unit cell out of the current directions triplet
                Eigen::Matrix3d A;
                A.col(0) = tvects[i];
                A.col(1) = tvects[j];
                A.col(2) = tvects[k];
                // Build a unit cell with direct vectors
                auto cell = std::shared_ptr<UnitCell>(new UnitCell(A));

                // If the unit cell volume is below the user-defined minimum volume,
                // skip it
                if (cell->volume() < _params.minUnitCellVolume)
                    continue;

                bool equivalent = false;

                // check to see if the cell is equivalent to a previous one. If so, skip
                // it
                for (auto solution : _solutions) {
                    if (cell->equivalent(*solution.first, _params.unitCellEquivalenceTolerance)) {
                        equivalent = true;
                        break;
                    }
                }
                // cell is equivalent to a previous one in the list
                if (equivalent)
                    continue;
                // Add this solution to the list of solution with a scored to be defined
                // futher
                _solutions.push_back(std::make_pair(cell, -1.0));
            }
        }
    }
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
            else
                return (s1.second > s2.second);
        });
}

void AutoIndexer::refineSolutions(const std::vector<Peak3D*>& peaks)
{
    // TODO: candidate for easy parallelization
    for (auto&& soln : _solutions) {

        auto cell = soln.first;
        cell->setIndexingTolerance(_params.indexingTolerance);
        Eigen::Matrix3d B = cell->reciprocalBasis();
        std::vector<Eigen::RowVector3d> hkls;
        std::vector<Eigen::RowVector3d> qs;
        std::vector<Eigen::Matrix3d> wt;

        PeakFilter peak_filter;
        std::vector<Peak3D*> enabled_peaks = peak_filter.filterEnabled(peaks, true);

        std::vector<Peak3D*> filtered_peaks =
            peak_filter.filterIndexed(enabled_peaks, *cell, cell->indexingTolerance());

        int success = filtered_peaks.size();
        for (auto peak : filtered_peaks) {
            MillerIndex hkld(peak->q(), *cell);
            hkls.emplace_back(hkld.rowVector().cast<double>());
            qs.emplace_back(peak->q().rowVector());

            Eigen::Vector3d c = peak->shape().center();
            Eigen::Matrix3d M = peak->shape().metric();
            auto state = peak->dataSet()->instrumentStates().interpolate(c[2]);
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

        // Sets the Minimizer with the parameters store and the size of the residual
        // vector
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
            cell->setIndexingTolerance(_params.indexingTolerance);
            cell->reduce(_params.niggliReduction, _params.niggliTolerance, _params.gruberTolerance);
            *cell = cell->applyNiggliConstraints();
        } catch (std::exception& e) {
            if (_handler)
                _handler->log("exception: " + std::string(e.what()));
            continue;
        }

        // Define the final score of this solution by computing the percentage of
        // the selected peaks which have been successfully indexed

        std::vector<Peak3D*> refiltered_peaks =
            peak_filter.filterIndexed(filtered_peaks, *cell, cell->indexingTolerance());

        double score = static_cast<double>(refiltered_peaks.size());
        double maxscore = static_cast<double>(filtered_peaks.size());

        // Percentage of indexing
        score /= 0.01 * maxscore;
        soln.second = score;
    }
}

void AutoIndexer::printSolutions()
{
    std::cout << std::setw(10) << "Quality"
              << std::setw(10) << "a"
              << std::setw(10) << "b"
              << std::setw(10) << "c"
              << std::setw(10) << "alpha"
              << std::setw(10) << "beta"
              << std::setw(10) << "gamma" << std::endl;
    for (auto solution : _solutions) {
        std::cout << std::fixed << std::setw(10) << std::setprecision(3)
                  << solution.second << solution.first->toString() << std::endl;
    }
}

} // namespace nsx
