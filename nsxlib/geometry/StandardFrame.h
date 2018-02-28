#pragma once

#include <Eigen/Core>

#include "CrystalTypes.h"
#include "InterpolatedState.h"
#include "InstrumentTypes.h"

namespace nsx {

//! \class StandardFrame
//!
//! This class implements the coordinate system described in Kabsch 1988 and Kabsch 2010.
class StandardFrame {
public:
    StandardFrame(sptrPeak3D peak);

    Eigen::Vector3d transform(const DetectorEvent& ev) const;

private:
    //! Reference peak about which the coordinate system is based
    sptrPeak3D _peak;
    //! State of the instrument at the time peak was observed.
    InterpolatedState _state;
    //! Incoming momentum.
    Eigen::RowVector3d _ki;
    //! Outgoing momentum.
    Eigen::RowVector3d _kf;
    //! First basis vector, direction kf x ki.
    Eigen::Vector3d _e1;
    //! Second basis vector, direction kf x e1.
    Eigen::Vector3d _e2;
    //! Zeta factor (see p135 of Kabsch 2010).
    double _zeta;
    //! Frame corresponding to peak center
    double _frame;
};

} // end namespace nsx
