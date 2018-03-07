#pragma once

#include <Eigen/Core>

#include "CrystalTypes.h"
#include "InterpolatedState.h"
#include "InstrumentTypes.h"

namespace nsx {

//! \class PeakCoordinateSystem
//!
//! This class implements the coordinate system described in Kabsch 1988 and Kabsch 2010.
class PeakCoordinateSystem {
public:
    PeakCoordinateSystem(sptrPeak3D peak);

    Eigen::Vector3d transform(const DetectorEvent& ev) const;

    //! Compute the Jacobian of the transformation from detector (x, y, frame) -> (alpha, beta, gamma).
    Eigen::Matrix3d jacobian() const;

    //! Estimate the beam divergence from the peak's shape in detector space
    double estimateDivergence() const;

    //! Estimage the crystal mosaicity from the peak's shape in detector space
    double estimateMosaicity() const;

    //! Compute the detector shape corresponding to given beam divergence and mosaicity.
    Ellipsoid detectorShape(double sigmaD, double sigmaM) const;

    //! Transform the detector shape into standard coordinates
    Ellipsoid standardShape() const;

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
