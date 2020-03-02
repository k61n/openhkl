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
#include "core/peak/PeakCollection.h"

namespace nsx {

//! Handles per-frame integration of a peak.

class IPeakIntegrator {
 public:
    IPeakIntegrator();
    virtual ~IPeakIntegrator();
    //! Compute the integrated intensity of the peak given the integration region.
    virtual bool
    compute(Peak3D* peak, ShapeLibrary* shape_library, const IntegrationRegion& region) = 0;
    //! Integrate all peaks in the list which are contained in the specified data set.
    //! @param peak_end Peak boundary (in sigma)
    //! @param bkg_begin Background beginning (in sigma)
    //! @param bkg_end Background end (in sigma)
    void integrate(std::vector<nsx::Peak3D*> peaks, ShapeLibrary* shape_library, sptrDataSet data);
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
    double _scale;
    //! The integrator values
    double _peak_end;
    //! The integrator values
    double _bkg_begin;
    //! The integrator values
    double _bkg_end;
    //! The integrator values
    double _d_min;
    //! The integrator values
    double _d_max;
    //! The integrator values
    double _radius;
    //! The integrator values
    double _n_frames;
    //! The integrator values
    bool _fit_center;
    //! The integrator values
    bool _fit_cov;

 public:
    //! get the value
    double scale() const { return _scale; };
    //! get the value
    double peakEnd() const { return _peak_end; };
    //! get the value
    double backBegin() const { return _bkg_begin; };
    //! get the value
    double backEnd() const { return _bkg_end; };
    //! get the value
    double dMin() const { return _d_min; };
    //! get the value
    double dMax() const { return _d_max; };
    //! get the value
    double radius() const { return _radius; };
    //! get the value
    double nFrames() const { return _n_frames; };
    //! get the value
    bool fitCenter() const { return _fit_center; };
    //! get the value
    bool fitCov() const { return _fit_cov; };

    //! get the value
    void setScale(double scale) { _scale = scale; };
    //! get the value
    void setPeakEnd(double peak_end) { _peak_end = peak_end; };
    //! get the value
    void setBkgBegin(double bkg_begin) { _bkg_begin = bkg_begin; };
    //! get the value
    void setBkgEnd(double bkg_end) { _bkg_end = bkg_end; };
    //! get the value
    void setDMin(double d_min) { _d_min = d_min; };
    //! get the value
    void setDMax(double d_max) { _d_max = d_max; };
    //! get the value
    void setRadius(double radius) { _radius = radius; };
    //! get the value
    void setNFrames(double n_frames) { _n_frames = n_frames; };
    //! get the value
    void setFitCenter(bool fit_center) { _fit_center = fit_center; };
    //! get the value
    void setFitCov(bool fit_cov) { _fit_cov = fit_cov; };
};

} // namespace nsx

#endif // CORE_PEAK_IPEAKINTEGRATOR_H
