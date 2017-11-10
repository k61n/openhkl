/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon Eric Pellegrini
 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

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

#pragma once

#include <map>
#include <ostream>
#include <set>
#include <vector>

#include <Eigen/Dense>

#include "CrystalTypes.h"
#include "InstrumentTypes.h"
#include "LMFunctor.h"
#include "Minimizer.h"
#include "UBSolution.h"

namespace nsx {

class UBSolution;

//! Class for UB and offset refinement
class UBMinimizer {
public:
    //! Construct minimizer with a given set of initial values.
    UBMinimizer(const UBSolution& initialState);
    //! Add a peak to be used in the residual calculation.
    void addPeak(const Peak3D& peak, const Eigen::RowVector3d& hkl);

    //! Run the fit with the maximum specified number of iterations.
    int run(unsigned int maxIter);
    //! Returns the current value of the solution (even if the fit did not converge).
    const UBSolution& solution() const;

    void refineSource(bool refine);
    void refineSample(unsigned int id, bool refine);
    void refineDetector(unsigned int id, bool refine);

private:
    //! Update the error from given covariance matrix
    void updateError(const Eigen::MatrixXd& cov);
    //! Compute the residuals given an input vector
    int residuals(Eigen::VectorXd& r);
    //! Return the set of fit parameters constructed from the UBSolution
    FitParameters fitParameters();

    UBSolution _solution;
    std::vector<std::pair<Peak3D,Eigen::RowVector3d>> _peaks;
    bool _refineSource;
    std::vector<bool> _refineSample;
    std::vector<bool> _refineDetector;
    Eigen::VectorXd _sigmaDetector;
    Eigen::VectorXd _sigmaSample;
    int _sourceID;
    std::vector<int> _sampleID;
    std::vector<int> _detectorID;
    std::vector<int> _ucID;
};

} // end namespace nsx
