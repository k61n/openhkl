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

#include <map>
#include <memory>
#include <vector>

#include <Eigen/Dense>

#include "CrystalTypes.h"
#include "DataTypes.h"
#include "Ellipsoid.h"
#include "GeometryTypes.h"
#include "InstrumentTypes.h"
#include "IntegrationRegion.h"
#include "Intensity.h"
#include "PeakIntegrator.h"
#include "Profile.h"

namespace nsx {

class Peak3D {

public:

    Peak3D(sptrDataSet data);
    
    Peak3D(sptrDataSet data, const Ellipsoid& shape);

    //! Copy constructor
    Peak3D(sptrDataSet data, const Peak3D& other) = delete;

    //! Assignment operator
    Peak3D& operator=(const Peak3D& other) = delete;

    //! Set the Peak region. Peak shaped is owned after setting
    void setShape(const Ellipsoid& peak);

    //! Get the projection of total data in the bounding box.
    const std::vector<Intensity>& rockingCurve() const;

    //! Compute the shape in q-space. May throw if there is no valid q-space ellipsoid.
    Ellipsoid qShape() const;

    const Ellipsoid& getShape() const { return _shape; }
    //const IntegrationRegion& getIntegrationRegion() const { return _integrationRegion; }

    //! Return the scaled intensity of the peak.
    Intensity getScaledIntensity() const;

    //! Return the intensity, after scaling, transmission, and Lorentz factor corrections
    Intensity correctedIntensity() const;

    //! Return the raw intensity of the peak.
    Intensity getRawIntensity() const;

    //! Return the scaling factor.
    double getScale() const;
    //! Rescale the current scaling factor by scale.
    void rescale(double factor);
    //! Set the scaling factor.
    void setScale(double factor);
   
    friend bool operator<(const Peak3D& p1, const Peak3D& p2);
    void setSelected(bool);
    bool isSelected() const;
    void setMasked(bool masked);
    void setTransmission(double transmission);
    double getTransmission() const;

    void addUnitCell(sptrUnitCell uc, bool activate=true);
    int activeUnitCellIndex() const;
    sptrUnitCell activeUnitCell() const;
    sptrUnitCell unitCell(int index) const;

    friend bool operator<(const Peak3D& p1, const Peak3D& p2);

    bool isIndexed() const;

    void setPredicted(bool predicted);
    bool isPredicted() const;

    bool hasUnitCells() const;
    void scaleShape(double scale);

    //! update the integration
    void updateIntegration(const PeakIntegrator& integrator);

    //! compute P value that there is actually an observed peak, assuming Poisson statistics
    double pValue() const;

    //! Return fitted peak profile
    const Profile& getProfile() const;

    //const PeakIntegrator& getIntegration() const;

    //! Return the q vector of the peak, transformed into sample coordinates.
    ReciprocalVector q() const;
    //! Return the predicted q vector of the peak, based on Miller index.
    ReciprocalVector qPredicted() const;

    sptrDataSet data() const { return _data; }

    void setRawIntensity(const Intensity& i);

    //! Return peak center at the given frame
    DetectorEvent predictCenter(double frame) const;

    #ifndef SWIG
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    #endif

private:
    //! Shape describing the Peak zone
    Ellipsoid _shape;

    //! Peak profile along frame (rotation) axis
    std::vector<Intensity> _rockingCurve;

    //! Raw intensity (count), background corrected
    Intensity _rawIntensity;

    //! Mean background estimate
    Intensity _meanBackground;

    UnitCellList _unitCells;
   
    double _scale;
    bool _selected;
    bool _masked;
    bool _predicted;
    double _transmission;
    int _activeUnitCellIndex;

    Profile _profile;

    //! Raw p value
    double _pValue;

    sptrDataSet _data;
};

} // end namespace nsx
