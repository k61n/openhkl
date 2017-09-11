/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2017- Laurent C. Chapon,  Eric Pellegrini, Jonathan Fisher
 *
 */
#ifndef NSXLIB_UBSOLUTION_H
#define NSXLIB_UBSOLUTION_H

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

//! Struct to store UB and offset refinement
class UBSolution {
public:
    UBSolution(const UBSolution& other);
    UBSolution();

    UBSolution& operator=(const UBSolution& ubsol);

    //! Apply solution to selected cell, sample, source, and detector
    void apply();

    //! Store UB and offsets into a flattened vector
    Eigen::VectorXd zip() const;
    //! Recover UB and offsets from a flattened vector
    void unzip(const Eigen::VectorXd& x);

    //! @brief Returns the number of inputs of the functor (e.g. the number of parameters)
    //! @return the number of inputs
    int inputs() const;

    //! Reset all the parameters (e.g. UB matrix + detector and sample offsets) to zero
    void resetParameters();

    //! @brief Set the starting value for a given parameter
    //! @param idx the index of the parameter
    //! @param value the value of the parameter to be fixed
    void setValue(unsigned int idx, double value);

    //! @brief Set the offsets that will be fixed during the minization
    //! @param idx the index of the offset (starting from 9)
    void refineParameter(unsigned int idx, bool refine);

    //! Return the covariance matrix of the refined parameters
    const Eigen::Matrix<double, 9, 9>& covub() const;

    //! Return the ub matrix
    Eigen::Matrix3d ub() const;

    //! Set the unit cell
    void setCell(sptrUnitCell cell);

    //! Set the source
    void setSource(sptrSource source);

    //! Set the sample
    void setSample(sptrSample sample);

    //! Set the detector
    void setDetector(sptrDetector detector);

    //! Return the array specifying which parameters are fixed
    const std::vector<bool> fixedParameters() const;

    //! Update the offsets and covariance matrix
    void update(const Eigen::VectorXd& x, const Eigen::MatrixXd& cov);

    //! Return the sample offsets
    const Eigen::VectorXd& sampleOffsets() const;
    //! Return the error in the sample offsets
    const Eigen::VectorXd& sigmaSampleOffsets() const;

    //! Return the detector offsets
    const Eigen::VectorXd& detectorOffsets() const;
    //! Return the error in the detector offsets
    const Eigen::VectorXd& sigmaDetectorOffsets() const;

    //! Return the source offset
    double sourceOffset() const;
    
    //! Apply unit cell symmetry constraints
    void setNiggliConstraint(bool constrain, double weight);

    //! Return the number of Niggli constraints (if set)
    int numNiggliConstraints() const;

    //! Return the vector of Niggli constraints
    Eigen::VectorXd niggliConstraints() const;

    //! Return the weight for the Niggli constraints
    double niggliWeight() const;

      

private:
    bool _niggliConstraint;
    double _niggliWeight;
    std::shared_ptr<UnitCell> _cell;
    std::shared_ptr<Detector> _detector;
    std::shared_ptr<Sample> _sample;
    std::shared_ptr<Source> _source;
    //! The original orientation matrix of the unit cell
    Eigen::Matrix3d _u0; 
    //! U offsets
    Eigen::Vector3d _uOffsets;
    //! character, stored in terms of constraints
    Eigen::VectorXd _character;
    Eigen::VectorXd _character0;
    //! Niggli to Gruber transformation
    Eigen::Matrix3d _NP;
    Eigen::Matrix<double,9,9> _covub;
    double _sourceOffset,_sigmaSourceOffset;
    Eigen::VectorXd _detectorOffsets;
    Eigen::VectorXd _sigmaDetectorOffsets;
    Eigen::VectorXd _sampleOffsets;
    Eigen::VectorXd _sigmaSampleOffsets;
    std::vector<bool> _fixedParameters;
    friend std::ostream& operator<<(std::ostream& os, const UBSolution& solution);
    int _nSampleAxes;
    int _nDetectorAxes;
};

} // end namespace nsx

#endif // NSXLIB_UBSOLUTION_H
