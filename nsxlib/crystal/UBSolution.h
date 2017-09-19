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
    //! \brief Construct with given source, sample, detector, and unit cell. Only the cell must be non-null.
    //! WARNING: If the cell parameters do not match its Niggli number, the UBSolution object cannot be
    //! created and this constructor will throw std::exception.
    UBSolution(sptrSource source, sptrSample sample, sptrDetector detector, sptrUnitCell cell);

    //! Apply solution to selected cell, sample, source, and detector
    void apply();   

    //! Return the ub matrix. This is calculated from the cell-parameters.
    Eigen::Matrix3d ub() const;    

    #ifndef SWIG
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    #endif

private:
    std::shared_ptr<UnitCell> _cell;
    std::shared_ptr<Detector> _detector;
    std::shared_ptr<Sample> _sample;
    std::shared_ptr<Source> _source;
    //! The original orientation matrix of the unit cell
    Eigen::Matrix3d _u0; 

    //! Niggli to Gruber transformation
    Eigen::Matrix3d _NP;

    friend std::ostream& operator<<(std::ostream& os, const UBSolution& solution);
    int _nSampleAxes;
    int _nDetectorAxes;

public:
    //! U offsets
    Eigen::Vector3d _uOffsets;
    //! Cell parameters, internal format. Used internally by UBMinimizer.
    Eigen::VectorXd _cellParameters;
    //! Error in cell parameters, internal format. Used internally by UBMinimizer.
    Eigen::VectorXd _sigmaCellParameters;
    
    double _sourceOffset;
    double _sigmaSource;
    Eigen::VectorXd _detectorOffset;  
    Eigen::VectorXd _sigmaDetector;
    Eigen::VectorXd _sampleOffset;
    Eigen::VectorXd _sigmaSample;  
};

} // end namespace nsx
