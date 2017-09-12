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
#ifndef NSXLIB_UBMINIMIZER_H
#define NSXLIB_UBMINIMIZER_H

#include <map>
#include <ostream>
#include <set>
#include <vector>

#include <Eigen/Dense>

#include "../crystal/CrystalTypes.h"
#include "../crystal/UBSolution.h"
#include "../instrument/InstrumentTypes.h"
#include "../mathematics/Minimizer.h"
#include "../utils/LMFunctor.h"

namespace nsx {

class UBSolution;

//! Class for UB and offset refinement
class UBMinimizer {
public:
    //! Default constructor
    UBMinimizer();

    //! Destructor
    ~UBMinimizer();

    /*
     * @brief Add a peak (e.g. an observation) to the minimizer
     * @param peak the peak to be added
     */
    void addPeak(const Peak3D& peak, const Eigen::RowVector3d& hkl);

    void clearPeaks();





    /*
     * @brief Run the minimization using GSL implementation
     * @return the status of the minimization (1 if everything OK)
     */
    int run(const UBSolution& initialState, unsigned int maxIter);
    /*
     * @brief Returns the solution of the last minization
     * @return the solution
     */
    const UBSolution& solution() const;


    /*
     * @brief Returns the number of values of the functor (e.g. the number of observations)
     * @return the number of inputs
     */
    int values() const;

    //! Compute the residuals given an input vector
    int residuals(Eigen::VectorXd& r);

    void refineSource(bool refine);
    void refineSample(unsigned int idx, bool refine);
    void refineDetector(unsigned int idx, bool refine);

private:
    UBSolution _solution;

    std::vector<std::pair<Peak3D,Eigen::RowVector3d>> _peaks;
    //sptrDetector _detector;
    //sptrSample _sample;
    //sptrSource _source;
    //std::vector<bool> _fixedParameters;

};

} // end namespace nsx

#endif // NSXLIB_UBMINIMIZER_H
