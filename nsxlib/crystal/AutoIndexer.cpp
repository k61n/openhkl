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

#include "../crystal/AutoIndexer.h"
#include "../crystal/FFTIndexing.h"
#include "../crystal/GruberReduction.h"
#include "../crystal/NiggliReduction.h"
#include "../crystal/Peak3D.h"
#include "../crystal/UBMinimizer.h"
#include "../crystal/UBSolution.h"
#include "../instrument/Detector.h"
#include "../instrument/Diffractometer.h"
#include "../instrument/Experiment.h"
#include "../instrument/Gonio.h"
#include "../instrument/Sample.h"
#include "../utils/ProgressHandler.h"

#include <string>

namespace nsx {

 
AutoIndexer::AutoIndexer(const std::shared_ptr<ProgressHandler>& handler):
    _peaks(),
    _solutions(),
    _handler(handler)
{
}

bool AutoIndexer::autoIndex(const IndexerParameters& params)
{
    _params = params;
    // Check that a minimum number of peaks have been selected for indexing
    if (_peaks.size() < 10) {
        if (_handler) {
            _handler->log("AutoIndexer: too few peaks to index!");
        }
        return false;
    }
        
    // Find the Q-space directions along which the projection of the the Q-vectors shows the highest periodicity
    computeFFTSolutions();

    refineSolutions();

    // Remove the solution whose percentage of successfully indexed peaks is under a given cutoff
    removeBad(_params.solutionCutoff);

    // refine the constrained unit cells in order to get the uncertainties
    refineConstraints();

    if (_handler) {
        _handler->log("Done refining solutions, building solution table.");
    }

    // finally, rank the solutions
    rankSolutions();

    return true;
}

void AutoIndexer::addPeak(const sptrPeak3D &peak)
{
    _peaks.emplace_back(peak);
}

void AutoIndexer::removeBad(double quality)
{
    // remove the bad solutions
    auto remove = std::remove_if(_solutions.begin(), _solutions.end(), [=] (const RankedSolution& s) { return s.second < quality; });
    _solutions.erase(remove, _solutions.end());
}

const std::vector<std::pair<sptrUnitCell, double> > &AutoIndexer::getSolutions() const
{
    return _solutions;
}

void AutoIndexer::computeFFTSolutions()
{
    _solutions.clear();
    const int npeaks = _peaks.size();
    
    if (_handler) {
        _handler->log("Searching direct lattice vectors using" + std::to_string(npeaks) + "peaks defined on numors:");
    }
    
    // Store the q-vectors of the peaks for auto-indexing
    std::vector<Eigen::Vector3d> qvects;
    qvects.reserve(npeaks);
    for (auto peak : _peaks) {
        // Keep only the peak that have selected and that are not masked
        if (peak->isSelected() && !peak->isMasked()) {
            qvects.push_back(peak->getQ());
        }
    }
    
    // Set up a FFT indexer object
    FFTIndexing indexing(_params.subdiv, _params.maxdim);
    
    // Find the best vectors via FFT
    std::vector<tVector> tvects = indexing.findOnSphere(qvects, _params.nStacks, _params.nSolutions);
    qvects.clear();
    
    for (int i = 0; i < _params.nSolutions; ++i) {

        for (int j = i+1; j < _params.nSolutions; ++j) {

            for (int k = j+1; k < _params.nSolutions; ++k) {

                // Build up a unit cell out of the current directions triplet
                Eigen::Matrix3d A;
                A.col(0) = tvects[i]._vect;
                A.col(1) = tvects[j]._vect;
                A.col(2) = tvects[k]._vect;
                // Build a unit cell with direct vectors
                auto cell = std::shared_ptr<UnitCell>(new UnitCell(A,false));

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
        cell->setHKLTolerance(_params.HKLTolerance);
        Eigen::Matrix3d B = cell->reciprocalBasis();
        std::vector<Eigen::RowVector3d> hkls;
        std::vector<Eigen::RowVector3d> qs;

        // Collect all the selected peaks for which the auto-indexing has been successful (integer Miller indices)
        int success = 0;
        for (auto peak : _peaks) {
            Eigen::RowVector3d hkl;
            bool indexingSuccess = peak->getMillerIndices(*cell,hkl,true);
            if (indexingSuccess && peak->isSelected() && !peak->isMasked()) {
                hkls.emplace_back(hkl);
                qs.emplace_back(peak->getQ());
                ++success;
            }
        }

        // The number of peaks must be at least for a proper minimization
        if (success < 10) {      
            continue;
        }

        // Lambda to compute residuals
        auto residuals = [&B, &hkls, &qs] (Eigen::VectorXd& f) -> int
        {
            int n = f.size() / 3;

            for (int i = 0; i < n; ++i) {
                auto dq = qs[i]-hkls[i]*B;
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
        minimizer.setxTol(1e-10);
        minimizer.setfTol(1e-10);
        minimizer.setgTol(1e-10);

        // fails to fit
        if (!minimizer.fit(100)) {
            continue;
        }

        // Update the cell with the fitter one and reduce it
        try {
            cell->setReciprocalBasis(B);
            cell->setHKLTolerance(_params.HKLTolerance);
            cell->reduce(_params.niggliReduction, _params.niggliTolerance, _params.gruberTolerance);
            *cell = cell->applyNiggliConstraints();
        } catch(std::exception& e) {
            if (_handler) {
                _handler->log("exception: " +std::string(e.what()));
            }
            continue;
        }

        // Define the final score of this solution by computing the percentage of the selected peaks which have been successfully indexed
        double score = 0.0;
        double maxscore = 0.0;
        for (auto peak : _peaks) {
            if (peak->isSelected() && !peak->isMasked()) {
                maxscore++;
                Eigen::RowVector3d hkl;
                bool indexingSuccess = peak->getMillerIndices(*cell,hkl,true);
                if (indexingSuccess) {
                    score++;
                }
            }
        }
        // Percentage of indexing
        score /= 0.01*maxscore;
        soln.second = score;
    }
}

void AutoIndexer::refineConstraints()
{
    //#pragma omp parallel for
    for (auto&& soln: _solutions) {
        auto cell = soln.first;

        if (soln.second <= 0.0) {
            continue;
        }

        // UBSolution constructor can throw if constraints are not met
        try {
            // The UB minimization will be performed with fixed instruments offset (no pointer for Detector, Sample and Source provided)
            UBSolution ub_soln(nullptr, nullptr, nullptr, cell);
            UBMinimizer min(ub_soln);

            int success = 0;
            for (auto peak : _peaks) {
                Eigen::RowVector3d hkl;
                bool indexingSuccess = peak->getMillerIndices(*cell,hkl,true);
                if (indexingSuccess && peak->isSelected() && !peak->isMasked()) {
                    min.addPeak(*peak, hkl);
                    ++success;
                }
            }

            // The number of peaks must be at least for a proper minimization
            if (success < 10) {      
                continue;
            }

            // fails to fit
            if (!min.run(100)) {
                continue;
            }

            ub_soln = min.solution();
            ub_soln.apply();
        } catch(...) {
            // force the solution to be excluded from the final list
            soln.second = -1.0;
            continue;
        }
 
        double score = 0.0;
        double maxscore = 0.0;
        for (auto peak : _peaks) {
            if (peak->isSelected() && !peak->isMasked()) {
                maxscore++;
                Eigen::RowVector3d hkl;
                bool indexingSuccess = peak->getMillerIndices(*cell,hkl,true);
                if (indexingSuccess) {
                    score++;
                }
            }
        }
        // Percentage of indexing
        score /= 0.01*maxscore;
        soln.second = score;
    }
}

} // end namespace nsx
