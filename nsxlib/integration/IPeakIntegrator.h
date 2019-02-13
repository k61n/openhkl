/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2017- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
          Jonathan Fisher, Forschungszentrum Juelich GmbH
    BP 156
    6, rue Jules Horowitz
    38042 Grenoble Cedex 9
    France
    chapon[at]ill.fr
    pellegrini[at]ill.fr
    j.fisher[at]fz-juelich.de

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
#include <vector>

#include <Eigen/Dense>

#include "BrillouinZone.h"
#include "GeometryTypes.h"
#include "IntegrationRegion.h"
#include "Intensity.h"
#include "ProgressHandler.h"
#include "UtilsTypes.h"

namespace nsx {

class DataSet;

//! \class IPeakIntegrator
//! \brief This is a helper class to handle per-frame integration of a peak.
class IPeakIntegrator {
public:
    //! Default constructor
    IPeakIntegrator();
    //! Destructor
    virtual ~IPeakIntegrator();
    //! Compute the integrated intensity of the peak given the integration region.
    virtual bool compute(sptrPeak3D peak, const IntegrationRegion& region) = 0;
    //! Integrate all peaks in the list which are contained in the specified data set.
    /** \param peak_end Peak boundary (in sigma)
     *  \param bkg_begin Background beginning (in sigma)
     *  \param bkg_end Background end (in sigma)
     */
    void integrate(PeakList peaks, sptrDataSet data, double peak_end, double bkg_begin, double bkg_end);
    //! Return the mean background.
    Intensity meanBackground() const;
    //! Return the integrated intensity.
    Intensity integratedIntensity() const;
    //! Return the peak rocking curve.
    const std::vector<Intensity>& rockingCurve() const;
    //! Set the progress handler.
    void setHandler(sptrProgressHandler handler);

protected:
    //! Mean local background of peak. The uncertainty is the uncertainty of the _estimate_ of the background.
    Intensity _meanBackground;
    //! Net integrated intensity, after background correction.
    Intensity _integratedIntensity;
    //! The rocking curve of the peak.
    std::vector<Intensity> _rockingCurve;
    //! Optional pointer to progress handler.
    sptrProgressHandler _handler;
};

} // end namespace nsx
