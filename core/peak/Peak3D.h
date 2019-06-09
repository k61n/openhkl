//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/peak/Peak3D.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_PEAK_PEAK3D_H
#define CORE_PEAK_PEAK3D_H

#include <map>
#include <memory>
#include <vector>

#include <Eigen/Dense>

#include "core/crystal/Intensity.h"
#include "core/experiment/CrystalTypes.h"
#include "core/experiment/DataTypes.h"
#include "core/geometry/Ellipsoid.h"
#include "core/geometry/GeometryTypes.h"
#include "core/instrument/InstrumentTypes.h"
#include "core/peak/IPeakIntegrator.h"

namespace nsx {

//! \class Peak3D
//! \brief Data type used to store integrated peaks, including their shape and
//! location.
class Peak3D {

public:
    //! Create peak belonging to data without setting a position, shape, or intensity
    Peak3D(sptrDataSet data);
    //! Create peak belonging to data with given shape
    Peak3D(sptrDataSet data, const Ellipsoid& shape);

    Peak3D(sptrDataSet data, const Peak3D& other) = delete;

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
    //! eturn the peak masking state
    bool masked() const;

    //! Returns true if peak is enable (selected and not masked)
    bool enabled() const;

    //! Sets the transmission factor
    void setTransmission(double transmission);
    //! Returns the transmission factor
    double transmission() const;

    //! Add a unit cell to the peak, optionally make it the active cell
    void setUnitCell(sptrUnitCell uc);
    //! Returns the active unit cell
    sptrUnitCell unitCell() const;

    //! Sets whether the peak is observed or predicted
    void setPredicted(bool predicted);
    //! Returns if the peak is predicted
    bool predicted() const;

    //! Update the integration of the peak
    void updateIntegration(
        const IPeakIntegrator& integrator, double peakEnd, double bkgBegin, double bkgEnd);
    //! Returns the q vector of the peak, transformed into sample coordinates.
    ReciprocalVector q() const;
    //! Returns the predicted q vector of the peak, based on Miller index.
    ReciprocalVector qPredicted() const;
    //! Returns the data set to which this peak belongs
    sptrDataSet data() const { return _data; }
    //! Sets raw intensity count (from image), with no corrections
    void setRawIntensity(const Intensity& i);
    //! Returns peak center at the given frame
    DetectorEvent predictCenter(double frame) const;

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

    sptrUnitCell _unitCell;

    double _scale;
    bool _selected;
    bool _masked;
    bool _predicted;
    double _transmission;

    sptrDataSet _data;
    //! Peak profile along frame (rotation) axis
    std::vector<Intensity> _rockingCurve;
};

} // end namespace nsx

#endif // CORE_PEAK_PEAK3D_H
