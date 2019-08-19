//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/peak/IPeakIntegrator.h
//! @brief     Defines class IPeakIntegrator
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_PEAK_IPEAKINTEGRATOR_H
#define CORE_PEAK_IPEAKINTEGRATOR_H

#include "base/utils/ProgressHandler.h"
#include "core/experiment/DataTypes.h"
#include "core/peak/IntegrationRegion.h"
#include "core/peak/Intensity.h"

namespace nsx {

//! Handles per-frame integration of a peak.

class IPeakIntegrator {
 public:
    IPeakIntegrator();
    virtual ~IPeakIntegrator();
    //! Compute the integrated intensity of the peak given the integration region.
    virtual bool compute(sptrPeak3D peak, const IntegrationRegion& region) = 0;
    //! Integrate all peaks in the list which are contained in the specified data set.
    //! @param peak_end Peak boundary (in sigma)
    //! @param bkg_begin Background beginning (in sigma)
    //! @param bkg_end Background end (in sigma)
    void integrate(
       PeakList peaks, sptrDataSet data, 
       double peak_end, double bkg_begin, 
       double bkg_end);
    //! Returns the mean background.
    Intensity meanBackground() const;
    //! Returns the integrated intensity.
    Intensity integratedIntensity() const;
    //! Returns the peak rocking curve.
    const std::vector<Intensity>& rockingCurve() const;
    //! Sets the progress handler.
    void setHandler(sptrProgressHandler handler);

 protected:
    //! Mean local background of peak. The uncertainty is the uncertainty of the
    //! _estimate_ of the background.
    nsx::Intensity _meanBackground;
    //! Net integrated intensity, after background correction.
    nsx::Intensity _integratedIntensity;
    //! The rocking curve of the peak.
    std::vector<Intensity> _rockingCurve;
    //! Optional pointer to progress handler.
    sptrProgressHandler _handler;
    
 private:
    //! The integrator values
    double _peak_end;
    //! The integrator values
    double _bkg_begin;
    //! The integrator values
    double _bkg_end;

 public:
    //! get the value
    double peakEnd() const {return _peak_end;};
    //! get the value
    double backBegin() const {return _bkg_begin;};
    //! get the value
    double backEnd() const {return _bkg_end;};
};

} // namespace nsx

#endif // CORE_PEAK_IPEAKINTEGRATOR_H
