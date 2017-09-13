/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2017- Laurent C. Chapon,  Eric Pellegrini, Jonathan Fisher
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

//! Struct to store UB and offset refinement
class UBSolution {
public:
    UBSolution(sptrSource source, sptrSample sample, sptrDetector detector, sptrUnitCell cell);
    //UBSolution(const UBSolution& other);
    //UBSolution();

    //UBSolution& operator=(const UBSolution& ubsol);

    //! Apply solution to selected cell, sample, source, and detector
    void apply();   

    //! Return the ub matrix
    Eigen::Matrix3d ub() const;
    
    //! Apply unit cell symmetry constraints
    void setNiggliConstraint(bool constrain, double weight);

    //! Return the number of Niggli constraints (if set)
    int numNiggliConstraints() const;

    //! Return the vector of Niggli constraints
    Eigen::VectorXd niggliConstraints() const;

    //! Return the weight for the Niggli constraints
    double niggliWeight() const;

    #ifndef SWIG
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    #endif

private:
    bool _niggliConstraint;
    double _niggliWeight;
    std::shared_ptr<UnitCell> _cell;
    std::shared_ptr<Detector> _detector;
    std::shared_ptr<Sample> _sample;
    std::shared_ptr<Source> _source;
    //! The original orientation matrix of the unit cell
    Eigen::Matrix3d _u0; 

    Eigen::VectorXd _initialCharacter;
    //! Niggli to Gruber transformation
    Eigen::Matrix3d _NP;


    
    //std::vector<bool> _fixedParameters;

    bool _refineU;
    bool _refineB;

    friend std::ostream& operator<<(std::ostream& os, const UBSolution& solution);
    int _nSampleAxes;
    int _nDetectorAxes;

public:
    //! U offsets
    Eigen::Vector3d _uOffsets;
    //! character, stored in terms of constraints
    Eigen::VectorXd _character;
    
    double _sourceOffset;
    double _sigmaSource;
    Eigen::VectorXd _detectorOffset;  
    Eigen::VectorXd _sigmaDetector;
    Eigen::VectorXd _sampleOffset;
    Eigen::VectorXd _sigmaSample;  
};

} // end namespace nsx
