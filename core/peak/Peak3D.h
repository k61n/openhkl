//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/peak/Peak3D.h
//! @brief     Defines class Peak3D
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_PEAK_PEAK3D_H
#define OHKL_CORE_PEAK_PEAK3D_H

#include "base/geometry/Ellipsoid.h"
#include "base/geometry/ReciprocalVector.h"
#include "core/data/DataTypes.h"
#include "core/instrument/InstrumentState.h"
#include "core/peak/Intensity.h"
#include "tables/crystal/UnitCell.h"

#include <map>

namespace ohkl {

class IIntegrator;
class MillerIndex;

/*! \addtogroup python_api
 *  @{*/

//! Specifies reason why a peak was rejected
enum class RejectionFlag {
    NotRejected,
    Masked,
    OutsideThreshold,
    OutsideFrames,
    OutsideDetector, // from PeakFinder
    IntegrationFailure,
    TooFewPoints,
    NoNeighbours,
    TooFewNeighbours,
    NoUnitCell,
    NoDataSet,
    InvalidRegion, // from integrator
    InterpolationFailure,
    InvalidShape,
    InvalidSigma,
    InvalidBkgSigma,
    SaturatedPixel,
    OverlappingBkg,
    OverlappingPeak,
    InvalidCentroid,
    InvalidCovariance,
    CentreOutOfBounds,
    BadIntegrationFit,
    NoShapeModel,
    NoISigmaMinimum,
    PredictionUpdateFailure, // from refiner
    ManuallyRejected,
    OutsideIndexingTol,
    Outlier, // from peak statistics
    Count
};

/*! \brief A peak object storing real-space information on the peak
 *
 *  This object stores the real-space shape as an Ellipsoid object, including
 *  center of the detector spot in 3D (x, y, frame). The frame does not have to
 *  be an integer value, as the reflexis center is generally between two frames.

 *  Also implements integration and reciprocal space transformation, as well as
 *  storing various metadata.
 */

class Peak3D {
 public:
    //! Create peak belonging to data without setting a position, shape, or intensity
    Peak3D(sptrDataSet dataSet);
    //! Create peak belonging to data with given shape
    Peak3D(sptrDataSet dataSet, const Ellipsoid& shape);
    //! Create peak belonging to data with given shape
    Peak3D(sptrDataSet dataSet, const MillerIndex& hkl);
    //! Creat the peak from another peak
    Peak3D(std::shared_ptr<ohkl::Peak3D> peak);

    //! Comparison operator used to sort peaks
    friend bool operator<(const Peak3D& p1, const Peak3D& p2);

    //! Sets the Peak region. Peak shaped is owned after setting
    void setShape(const Ellipsoid& shape);
    //! Set the Miller indices
    void setMillerIndices();
    //! Set the Miller indices from the given triple. NB. ONLY TO BE USED BY EXPERIMENTIMPORTER
    void setMillerIndices(const MillerIndex& hkl) { _hkl = hkl; };

    //! Gets the projection of total data in the bounding box.
    const std::vector<Intensity>& rockingCurve() const;

    //! Compute the shape in q-space. May throw if there is no valid q-space ellipsoid.
    Ellipsoid qShape() const;
    //! Return the shape of the peak as an ellipsoid in detector coordinates
    const Ellipsoid& shape() const;

    //! Return the intensity, after scaling, transmission, and Lorentz factor corrections
    Intensity correctedIntensity() const;
    //! Return the raw intensity of the peak.
    Intensity rawIntensity() const;
    //! Return mean background of the peak
    Intensity meanBackground() const;
    //! Return the mean background gradient of the peak
    Intensity meanBkgGradient() const;

    //! Return shape scale used to define peak region
    double peakEnd() const;
    //! Return shape scale used to define beginning of background region
    double bkgBegin() const;
    //! Return shape scale used to define end of background region
    double bkgEnd() const;

    //! Returns the scaling factor.
    double scale() const;
    //! Set the scaling factor.
    void setScale(double factor);

    //! Is the peak selected? Selected peaks are "valid", and have not been
    //! automatically rejected by the integrator.
    bool selected() const;
    //! Set the peak selection state
    void setSelected(bool);
    //! Reject a peak
    void reject(RejectionFlag flag);
    //! Is the peak masked? Masked peaks are "invalid", and have been manually
    //! deselected via a selection box in a DetectorScene.
    bool masked() const;
    //! Return the peak masking state
    void setMasked(bool masked);

    //! Is the peak enabled (selected and not masked)?
    bool enabled() const;

    //! Set the transmission factor
    void setTransmission(double transmission);
    //! Return the transmission factor
    double transmission() const;

    //! Assign a unit cell to the peak
    void setUnitCell(const sptrUnitCell& uc);
    //! Returns the unit cell
    const UnitCell* unitCell() const;

    //! Designates the peak as predicted (true) or observed (false)
    void setPredicted(bool predicted);
    //! Is the peak predicted (as opposed to observed)?
    bool predicted() const;

    //! Designates the peak as "caught" by a filter
    void caughtYou(bool caught);
    //! Designates the peak as "rejected" by a filter
    void rejectYou(bool reject);
    //! Has the peak been caught by a filter?
    bool caughtByFilter() const;
    //! Has the peak been rejected by a filter?
    bool rejectedByFilter() const;

    //! Manually set the integration parameters for this peak
    void setManually(
        Intensity intensity, double peakEnd, double bkgBegin, double bkgEnd, double scale,
        double transmission, Intensity mean_bkg, bool predicted, bool selected, bool masked,
        int rejection_flag, int integration_flag, Intensity mean_bkg_grad = {});

    //! Update the integration parameters for this peak
    void updateIntegration(
        const std::vector<Intensity>& rockingCurve, const Intensity& meanBackground,
        const Intensity& meanBkgGradient, const Intensity& integratedIntensity, double peakEnd,
        double bkgBegin, double bkgEnd);
    //! Return the q vector of the peak, transformed into sample coordinates.
    ReciprocalVector q() const;
    //! Return q vector in cases where we do *not* want to interpolate the InstrumentState
    //! (e.g. when indexing a single frame)
    ReciprocalVector q(const InstrumentState& state) const;
    //! Return the data set to which this peak belongs
    sptrDataSet dataSet() const { return _data; }
    //! Set the raw intensity count (from image), with no corrections
    void setRawIntensity(const Intensity& i);
    //! Get the Miller indices for this peak
    const MillerIndex& hkl() const;
    //! Return the peak scale
    double getPeakEnd() { return _peakEnd; };
    //! Return the beginniing of the background region (in peak scales)
    double getBkgBegin() { return _bkgBegin; };
    //! Return the end of the background region (in peak scales)
    double getBkgEnd() { return _bkgEnd; };
    //! Set the reason for this peak being rejected (unselected)
    void setRejectionFlag(RejectionFlag flag, bool overwrite = false);
    //! Set the reason for rejection during integration
    void setIntegrationFlag(RejectionFlag flag) { _integration_flag = flag; };
    //! Return the rejection flag only
    RejectionFlag getRejectionFlag() const { return _rejection_flag; };
    //! Return the integration flag only
    RejectionFlag getIntegrationFlag() const { return _integration_flag; };
    //! Return the integration flag, or rejection flag if the former is not set
    RejectionFlag rejectionFlag() const;
    //! Return a string explaining the rejection
    std::string rejectionString() const;
    //! Return a string representation of the peak
    std::string toString() const;
    //! Return the map of RejectionFlag definitions
    static const std::map<RejectionFlag, std::string>& rejectionMap();


#ifndef SWIG
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
#endif

 private:
    //! Shape describing the Peak zone
    Ellipsoid _shape;
    //! Raw intensity (count), background corrected
    Intensity _rawIntensity;
    //! Mean background estimate
    Intensity _meanBackground;
    //! Mean background gradient
    Intensity _meanBkgGradient;
    //! Shape scale factor for peak
    double _peakEnd;
    //! Shape scale factor for start of background
    double _bkgBegin;
    //! Shape scale factor for end of background
    double _bkgEnd;
    //! Miller indices calculated during autoindexing
    MillerIndex _hkl = {0, 0, 0};

    //! Unit cell assigned to this peak
    std::weak_ptr<UnitCell> _unitCell;

    double _scale;
    //! Whether this peak is selected (valid)
    bool _selected;
    //! Whether this peak has been masked (invalid)
    bool _masked;
    //! Whether this peak has been predicted (as opposed to observed)
    bool _predicted;
    //! Whether this peak has been caught by a filter
    bool _caught_by_filter;
    //! Whether this peak has been rejected by a filter
    bool _rejected_by_filter;
    //! The transmission factor
    double _transmission;
    //! Reason for rejection
    ohkl::RejectionFlag _rejection_flag;
    //! Reason for rejection during integration
    ohkl::RejectionFlag _integration_flag;

    //! Pointer to the dataset from which this peak is derived
    sptrDataSet _data;
    //! Peak profile along frame (rotation) axis
    std::vector<Intensity> _rockingCurve;

    //! Map of rejection flag descriptions
    static const std::map<RejectionFlag, std::string> _rejection_map;

    //! Threshold for valid sigma2
    static constexpr double _sigma2_eps = 1.0e-8;
};

using sptrPeak3D = std::shared_ptr<Peak3D>;
using PeakList = std::vector<sptrPeak3D>;
using sptrPeakList = std::shared_ptr<PeakList>;

//! Sort peak into a list of equivalent peaks, using the space group symmetry,
//! optionally including Friedel pairs (if this is not already a symmetry of the space group)
// --- unused
// std::vector<PeakList> findEquivalences(
//    const SpaceGroup& group, const PeakList& peak_list, bool friedel);

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_PEAK_PEAK3D_H
