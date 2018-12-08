/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon Institut Laue-Langevin
    BP 156
    6, rue Jules Horowitz
    38042 Grenoble Cedex 9
    France
    chapon[at]ill.fr
    pellegrini[at]ill.fr
    j.fisher[at]fz-juelide.de

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

#include <vector>
#include <Eigen/Dense>

#include "PeakList.h"
#include "DataTypes.h"
#include "Ellipsoid.h"
#include "GeometryTypes.h"
#include "Intensity.h"
#include "IPeakIntegrator.h"
#include "UnitCell.h"

namespace nsx {

//! \class Peak3D
//! \brief Data type used to store integrated peaks, including their shape and location.
class Peak3D {

public:
    //! Create peak belonging to data without setting a position, shape, or intensity
    Peak3D(sptrDataSet data);
    //! Create peak belonging to data with given shape
    Peak3D(sptrDataSet data, const Ellipsoid& shape);
    //! Copy constructor deleted
    Peak3D(sptrDataSet data, const Peak3D& other) = delete;
    //! Assignment operator deleted
    Peak3D& operator=(const Peak3D& other) = delete;

    //! Comparison operator used to sort peaks
    friend bool operator<(const Peak3D& p1, const Peak3D& p2);

    //! Set the Peak region. Peak shaped is owned after setting
    void setShape(const Ellipsoid& peak);

    //! Get the projection of total data in the bounding box.
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

    //! Return shape scale used to define peak region
    double peakEnd() const;
    //! Return shape scale used to define beginning of background region
    double bkgBegin() const;
    //! Return shape scale used to define end of background region
    double bkgEnd() const;

    //! Return the scaling factor.
    double scale() const;
    //! Set the scaling factor.
    void setScale(double factor);

    //! Set the peak selection state
    void setSelected(bool);

    //! Return the peak selection state
    bool selected() const;

    //! Set the peak masking state
    void setMasked(bool masked);
    //! eturn the peak masking state
    bool masked() const;

    //! Return true if peak is enable (selected and not masked)
    bool enabled() const;

    //! Set the transmission factor
    void setTransmission(double transmission);
    //! Return the transmission factor
    double transmission() const;

    //! Add a unit cell to the peak, optionally make it the active cell
    void setUnitCell(sptrUnitCell uc);
    //! Return the active unit cell
    sptrUnitCell unitCell() const;

    //! Set whether the peak is observed or predicted
    void setPredicted(bool predicted);
    //! Return if the peak is predicted
    bool predicted() const;

    //! Update the integration of the peak
    void updateIntegration(const IPeakIntegrator& integrator, double peakEnd, double bkgBegin, double bkgEnd);
    //! Return the q vector of the peak, transformed into sample coordinates.
    ReciprocalVector q() const;
    //! Return the predicted q vector of the peak, based on Miller index.
    ReciprocalVector qPredicted() const;
    //! Return the data set to which this peak belongs
    sptrDataSet data() const { return _data; }
    //! Set raw intensity count (from image), with no corrections
    void setRawIntensity(const Intensity& i);
    //! Return peak center at the given frame
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
