#pragma once

#include <Eigen/Core>

#include "CrystalTypes.h"
#include "DetectorEvent.h"
#include "Ellipsoid.h"
#include "InterpolatedState.h"
#include "InstrumentTypes.h"

namespace nsx {

//! \class PeakCoordinateSystem
//! \brief This class implements the per-peak coordinate system described by Kabsch (1988, 2010)
//!
//! Following Kabsch 1988, we introduce about each peak a new coordinate system which is supposed
//! to undo the effects of detector geometry and distortion from the Lorentz factor.
//! In this system, the peaks intensity distribution is _approximately_ gaussian with an
//! _approximately_ diagonal covariance matrix. 
//!
//! This allows us to compare the shapes of peaks on different regions of the detector in a 
//! way that is independent of the detector geometry. This is crucial for shape prediction and profile
//! fitting.
class PeakCoordinateSystem {
public:
    //! Construct the coordinate system about the given peak.
    PeakCoordinateSystem(sptrPeak3D peak);
    //! Transform from detector coordinates in to peak coordinates.
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
    //! DetectorEvent corresponding to peak center
    DetectorEvent _event;
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
};

} // end namespace nsx
