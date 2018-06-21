/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2017- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

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

#include <string>

#include "AutoIndexer.h"
#include "DataSet.h"
#include "Detector.h"
#include "Diffractometer.h"
#include "Experiment.h"
#include "FFTIndexing.h"
#include "FitParameters.h"
#include "Gonio.h"
#include "GruberReduction.h"
#include "MillerIndex.h"
#include "Minimizer.h"
#include "NiggliReduction.h"
#include "Peak3D.h"
#include "PeakFilter.h"
#include "ProgressHandler.h"
#include "ReciprocalVector.h"
#include "Sample.h"
#include "UnitCell.h"

namespace nsx {

 
AutoIndexer::AutoIndexer(const std::shared_ptr<ProgressHandler>& handler):
    _peaks(),
    _solutions(),
    _handler(handler)
{
}

void AutoIndexer::autoIndex(const IndexerParameters& params)
{
    _params = params;
        
    // Find the Q-space directions along which the projection of the the Q-vectors shows the highest periodicity
    computeFFTSolutions();

    refineSolutions();

    // Remove the solution whose percentage of successfully indexed peaks is under a given cutoff
    removeBad(_params.solutionCutoff);

    // refine the constrained unit cells in order to get the uncertainties
    // refineConstraints();

    if (_handler) {
        _handler->log("Done refining solutions, building solution table.");
    }

    // finally, rank the solutions
    rankSolutions();
}

void AutoIndexer::removeBad(double quality)
{
    // remove the bad solutions
    auto remove = std::remove_if(_solutions.begin(), _solutions.end(), [=] (const RankedSolution& s) { return s.second < quality; });
    _solutions.erase(remove, _solutions.end());
}

const std::vector<std::pair<sptrUnitCell, double> > &AutoIndexer::solutions() const
{
    return _solutions;
}

void AutoIndexer::computeFFTSolutions()
{
    _solutions.clear();
        
    // Store the q-vectors of the peaks for auto-indexing
    std::vector<ReciprocalVector> qvects;

    PeakFilter peak_filter;
    auto filtered_peaks = peak_filter.selected(_peaks,true);

    for (size_t i = 0; i < filtered_peaks.size(); ++i) {
        auto& peak = filtered_peaks[i];
        auto q = peak->q().rowVector();
        qvects.push_back(ReciprocalVector(q));
    }

    // Check that a minimum number of peaks have been selected for indexing
    if (qvects.size() < 10) {
        if (_handler) {
            _handler->log("AutoIndexer: too few peaks to index!");
        }
        throw std::runtime_error("Too few peaks to autoindex");
    }

    if (_handler) {
        _handler->log("Searching direct lattice vectors using" + std::to_string(qvects.size()) + "peaks defined on numors:");
    }
    
    // Set up a FFT indexer object
    FFTIndexing indexing(_params.subdiv, _params.maxdim);
    
    // Find the best vectors via FFT
    auto&& tvects = indexing.findOnSphere(qvects, _params.nVertices, _params.nSolutions);
    qvects.clear();
    
    for (int i = 0; i < _params.nSolutions; ++i) {
        for (int j = i+1; j < _params.nSolutions; ++j) {
            for (int k = j+1; k < _params.nSolutions; ++k) {
                // Build up a unit cell out of the current directions triplet
                Eigen::Matrix3d A;
                A.col(0) = tvects[i].first;
                A.col(1) = tvects[j].first;
                A.col(2) = tvects[k].first;
                // Build a unit cell with direct vectors
                auto cell = std::shared_ptr<UnitCell>(new UnitCell(A));

                // If the unit cell volume is below the user-defined minimum volume, skip it
                if (cell->volume() < _params.minUnitCellVolume) {
                    continue;
                }

                bool equivalent = false;
    
                // check to see if the cell is equivalent to a previous one. If so, skip it
                for (auto solution: _solutions) {
                    if (cell->equivalent(*solution.first, _params.unitCellEquivalenceTolerance)) {
                        equivalent = true;
                        break;
                    }
                }
                // cell is equivalent to a previous one in the list
                if (equivalent) {
                    continue;
                }
                // Add this solution to the list of solution with a scored to be defined futher
                _solutions.push_back(std::make_pair(cell, -1.0));
            }
        }
    }
}

void AutoIndexer::rankSolutions()
{
    // Sort solutions by decreasing quality.
    // For equal quality, smallest volume is first
    std::sort(_solutions.begin(),_solutions.end(),[](const RankedSolution& s1, const RankedSolution& s2) -> bool
    {
        if (s1.second == s2.second) {
            return (s1.first->volume() < s2.first->volume());
        } else {
            return (s1.second > s2.second);
        }
    });
}

void AutoIndexer::refineSolutions()
{
    //#pragma omp parallel for
    for (auto&& soln: _solutions) {
        auto cell = soln.first;
        cell->setIndexingTolerance(_params.indexingTolerance);
        Eigen::Matrix3d B = cell->reciprocalBasis();
        std::vector<Eigen::RowVector3d> hkls;
        std::vector<Eigen::RowVector3d> qs;
        std::vector<Eigen::Matrix3d> wt;

        PeakFilter peak_filter;
        PeakList filtered_peaks;
        filtered_peaks = peak_filter.selected(_peaks,true);
        filtered_peaks = peak_filter.indexed(filtered_peaks, cell, cell->indexingTolerance());

        int success = filtered_peaks.size();
        for (auto peak: filtered_peaks) {
            MillerIndex hkld(peak->q(), *cell);
            hkls.emplace_back(hkld.rowVector().cast<double>());
            qs.emplace_back(peak->q().rowVector());

            Eigen::Vector3d c = peak->shape().center();
            Eigen::Matrix3d M = peak->shape().metric();
            auto state = peak->data()->interpolatedState(c[2]);
            Eigen::Matrix3d J = state.jacobianQ(c[0], c[1]);
            Eigen::Matrix3d JI = J.inverse();

            Eigen::Matrix3d A = JI.transpose() * M * JI;

            Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(A);
            Eigen::Matrix3d U = solver.eigenvectors();

            Eigen::Matrix3d D;
            D.setZero();

            for (auto i = 0; i < 3; ++i) {
                D(i,i) = std::sqrt(solver.eigenvalues()[i]);
            }

            wt.emplace_back(U.transpose() * D * U);
        }

        // The number of peaks must be at least for a proper minimization
        if (success < 10) {      
            continue;
        }

        // Lambda to compute residuals
        auto residuals = [&B, &hkls, &qs, &wt] (Eigen::VectorXd& f) -> int
        {
            int n = f.size() / 3;

            for (int i = 0; i < n; ++i) {
                //auto dq = wt[i]*(qs[i]-hkls[i]*B).transpose();
                auto dq = qs[i] - hkls[i]*B;
                f(3*i+0) = dq(0);
                f(3*i+1) = dq(1);
                f(3*i+2) = dq(2);
            }
            return 0;
        };

        // Pass by address the parameters to be fitted to the parameter store
        FitParameters params;
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                params.addParameter(&B(r,c));
            }
        }

        // Set the Minimizer with the parameters store and the size of the residual vector
        Minimizer minimizer;
        minimizer.initialize(params, 3*success);
        minimizer.set_f(residuals);  
        minimizer.setxTol(1e-15);
        minimizer.setfTol(1e-15);
        minimizer.setgTol(1e-15);

        // fails to fit
        if (!minimizer.fit(500)) {
            continue;
        }

        // Update the cell with the fitter one and reduce it
        try {
            cell->setReciprocalBasis(B);
            cell->setIndexingTolerance(_params.indexingTolerance);
            cell->reduce(_params.niggliReduction, _params.niggliTolerance, _params.gruberTolerance);
            *cell = cell->applyNiggliConstraints();
        } catch(std::exception& e) {
            if (_handler) {
                _handler->log("exception: " +std::string(e.what()));
            }
            continue;
        }

        // Define the final score of this solution by computing the percentage of the selected peaks which have been successfully indexed

        PeakList refiltered_peaks;
        refiltered_peaks = peak_filter.indexed(filtered_peaks, cell, cell->indexingTolerance());

        double score = static_cast<double>(refiltered_peaks.size());
        double maxscore = static_cast<double>(filtered_peaks.size());

        // Percentage of indexing
        score /= 0.01*maxscore;
        soln.second = score;
    }
}

void AutoIndexer::addPeak(sptrPeak3D peak)
{
    _peaks.push_back(peak);
}

} // end namespace nsx
