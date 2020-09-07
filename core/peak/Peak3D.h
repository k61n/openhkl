//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/peak/Peak3D.h
//! @brief     Defines class Peak3D
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_PEAK_PEAK3D_H
#define NSX_CORE_PEAK_PEAK3D_H

#include "base/geometry/Ellipsoid.h"
#include "core/data/DataTypes.h"
#include "core/peak/Intensity.h"
#include "tables/crystal/UnitCell.h"

namespace nsx {

class IPeakIntegrator;
class MillerIndex;

//! Stores integrated peaks, including their shape and location.

class Peak3D {
 public:
    //! Create peak belonging to data without setting a position, shape, or intensity
    Peak3D(sptrDataSet dataSet);
    //! Create peak belonging to data with given shape
    Peak3D(sptrDataSet dataSet, const Ellipsoid& shape);
    //! Creat the peak from another peak
    Peak3D(std::shared_ptr<nsx::Peak3D> peak);

    //! Comparison operator used to sort peaks
    friend bool operator<(const Peak3D& p1, const Peak3D& p2);

    //! Sets the Peak region. Peak shaped is owned after setting
    void setShape(const Ellipsoid& peak);

    //! Gets the projection of total data in the bounding box.
    const std::vector<Intensity>& rockingCurve() const;

    //! Compute the shape in q-space. May throw if there is no valid q-space ellipsoid.
    Ellipsoid qShape() const;
    //! Returns the shape of the peak as an ellipsoid in detector coordinates
    const Ellipsoid& shape() const;

    //! Returns the intensity, after scaling, transmission, and Lorentz factor corrections
    Intensity correctedIntensity() const;
    //! Returns the raw intensity of the peak.
    Intensity rawIntensity() const;

    //! Returns mean background of the peak
    Intensity meanBackground() const;

    //! Returns shape scale used to define peak region
    double peakEnd() const;
    //! Returns shape scale used to define beginning of background region
    double bkgBegin() const;
    //! Returns shape scale used to define end of background region
    double bkgEnd() const;

    //! Returns the scaling factor.
    double scale() const;
    //! Sets the scaling factor.
    void setScale(double factor);

    //! Sets the peak selection state
    void setSelected(bool);

    //! Returns the peak selection state
    bool selected() const;

    //! Sets the peak masking state
    void setMasked(bool masked);
    //! Return the peak masking state
    bool masked() const;

    //! Returns true if peak is enable (selected and not masked)
    bool enabled() const;

    //! Sets the transmission factor
    void setTransmission(double transmission);
    //! Returns the transmission factor
    double transmission() const;

    //! Add a unit cell to the peak, optionally make it the active cell
    void setUnitCell(const UnitCell* uc);
    //! Returns the active unit cell
    const UnitCell* unitCell() const;

    //! Sets whether the peak is observed or predicted
    void setPredicted(bool predicted);
    //! Returns if the peak is predicted
    bool predicted() const;

    //! Sets whether the peak is observed or predicted
    void caughtYou(bool caught);
    //! Sets whether the peak is observed or predicted
    void rejectYou(bool reject);
    //! Returns if the peak is predicted
    bool caughtByFilter() const;

    //! Update the integration of the peak
    void setManually(
        Intensity intensity, double peakEnd, double bkgBegin, double bkgEnd, double scale,
        double transmission, Intensity mean_bkg, bool predicted, bool selected, bool masked);

    //! Update the integration of the peak
    void updateIntegration(
        const std::vector<Intensity>& rockingCurve, const Intensity& meanBackground,
        const Intensity& integratedIntensity, double peakEnd, double bkgBegin, double bkgEnd);
    //! Returns the q vector of the peak, transformed into sample coordinates.
    ReciprocalVector q() const;
    //! Returns the predicted q vector of the peak, based on Miller index.
    // unused --- ReciprocalVector qPredicted() const;
    //! Returns the data set to which this peak belongs
    sptrDataSet dataSet() const { return _data; }
    //! Sets raw intensity count (from image), with no corrections
    void setRawIntensity(const Intensity& i);
    //! Returns peak center at the given frame
    // unused --- DetectorEvent predictCenter(double frame) const;
    //! Get the Miller indices for this peak
    const MillerIndex& hkl() const;
    //! Set the Miller indices given the q-vector and unit cell
    void setMillerIndices();

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
    //! Shape scale factor for peak
    double _peakEnd;
    //! Shape scale factor for start of background
    double _bkgBegin;
    //! Shape scale factor for end of background
    double _bkgEnd;
    //! Miller indices calculated during autoindexing
    MillerIndex _hkl;

    const UnitCell* _unitCell;

    double _scale;
    bool _selected;
    bool _masked;
    bool _predicted;
    bool _caught_by_filter;
    bool _rejected_by_filter;
    double _transmission;

    sptrDataSet _data;
    //! Peak profile along frame (rotation) axis
    std::vector<Intensity> _rockingCurve;
};

using sptrPeak3D = std::shared_ptr<Peak3D>;
using PeakList = std::vector<sptrPeak3D>;
using sptrPeakList = std::shared_ptr<PeakList>;

//! Sort peak into a list of equivalent peaks, using the space group symmetry,
//! optionally including Friedel pairs (if this is not already a symmetry of the space group)
// --- unused
// std::vector<PeakList> findEquivalences(
//    const SpaceGroup& group, const PeakList& peak_list, bool friedel);

} // namespace nsx

#endif // NSX_CORE_PEAK_PEAK3D_H
