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

#include "../crystal/CrystalTypes.h"
#include "../instrument/InstrumentTypes.h"
#include "../mathematics/Minimizer.h"
#include "../utils/LMFunctor.h"

namespace nsx {

/** @brief UB functor is used to refine UB-matrix and instrument offsets
 */
struct UBFunctor : public LMFunctor<double> {
    //! Default constructor
    UBFunctor();
    //! Copy constructor
    UBFunctor(const UBFunctor& other);
    //! Assignment operator
    UBFunctor& operator=(const UBFunctor& other);
    //! Destructor
    ~UBFunctor() = default;
    /*
     * @brief Call operator
     * @param x the input parameters
     * @param fvec the residuals
     */
    int operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const;

    /*
     * @brief Add a peak (e.g. an observation) to the minimizer
     * @param peak the peak to be added
     */
    void addPeak(const Peak3D& peak, const Eigen::RowVector3d& hkl);

    void clearPeaks();

    /*
     * @brief Returns the number of inputs of the functor (e.g. the number of parameters)
     * @return the number of inputs
     */
    int inputs() const;
    /*
     * @brief Returns the number of values of the functor (e.g. the number of observations)
     * @return the number of inputs
     */
    int values() const;
    /*
     * @brief Set the detector related to the peaks collected for the minimization
     * @param detector the detector
     */
    void setDetector(const sptrDetector& detector);
    /*
     * @brief Set the sample related to the peaks collected for the minimization
     * @param sample the sample
     */
    void setSample(const sptrSample& sample);
    //! Set the source
    void setSource(const sptrSource& source);
    //! Reset all the parameters (e.g. UB matrix + detector and sample offsets) to zero
    void resetParameters();
    /*
     * @brief Set the offsets that will be fixed during the minization
     * @param idx the index of the offset (starting from 9)
     */
    void refineParameter(unsigned int idx, bool refine);

    std::vector<std::pair<Peak3D,Eigen::RowVector3d>> _peaks;
    sptrDetector _detector;
    sptrSample _sample;
    sptrSource _source;
    std::set<int> _fixedParameters;
};


class UBMinimizer;

struct UBSolution {
    UBSolution(const UBSolution& other);
    friend class UBMinimizer;
    UBSolution();

    UBSolution(std::shared_ptr<Detector> detector,
               std::shared_ptr<Sample> sample,
               std::shared_ptr<Source> source,
               const Eigen::VectorXd& values,
               const Eigen::MatrixXd& cov,
               std::vector<bool> fixedParameters);

    UBSolution& operator=(const UBSolution& ubsol);

    std::shared_ptr<Detector> _detector;
    std::shared_ptr<Sample> _sample;
    std::shared_ptr<Source> _source;
    Eigen::Matrix3d _ub;
    Eigen::Matrix<double,9,9> _covub;
    double _sourceOffset,_sigmaSourceOffset;
    Eigen::VectorXd _detectorOffsets;
    Eigen::VectorXd _sigmaDetectorOffsets;
    Eigen::VectorXd _sampleOffsets;
    Eigen::VectorXd _sigmaSampleOffsets;
    std::vector<bool> _fixedParameters;
    friend std::ostream& operator<<(std::ostream& os, const UBSolution& solution);
};

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

    //! Reset all the parameters (e.g. UB matrix + detector and sample offsets) to zero
    void resetParameters();
    /*
     * @brief Set the detector related to the peaks collected for the minimization
     * @param detector the detector
     */
    void setDetector(const std::shared_ptr<Detector>& detector);
    /*
     * @brief Set the offsets that will be fixed during the minization
     * @param idx the index of the offset (starting from 9)
     */
    void refineParameter(unsigned int idx, bool refine);
    /*
     * @brief Set the sample related to the peaks collected for the minimization
     * @param sample the sample
     */
    void setSample(const std::shared_ptr<Sample>& sample);
    /*
     * @brief Set the starting values of the UB matrix
     * @param ub the UB matrix
     */
    void setSource(const std::shared_ptr<Source>& source);
    void setStartingUBMatrix(const Eigen::Matrix3d& ub);
    /*
     * @brief Set the starting value for a given parameter
     * @param idx the index of the parameter
     * @param value the value of the parameter to be fixed
     */
    void setStartingValue(unsigned int idx, double value);
    /*
     * @brief Unset the starting value for a given parameter
     * @param idx the index of the parameter
     */
    void unsetStartingValue(unsigned int idx);
    /*
     * @brief Run the minimization using GSL implementation
     * @return the status of the minimization (1 if everything OK)
     */
    int run(unsigned int maxIter);
    /*
     * @brief Returns the solution of the last minization
     * @return the solution
     */
    const UBSolution& getSolution() const;

private:
    UBFunctor _functor;
    UBSolution _solution;
    std::map<unsigned int,double> _start;
};

} // end namespace nsx
