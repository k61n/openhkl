//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/shape/IPeakIntegrator.h
//! @brief     Defines class IPeakIntegrator
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_SHAPE_IPEAKINTEGRATOR_H
#define NSX_CORE_SHAPE_IPEAKINTEGRATOR_H

#include "base/utils/ProgressHandler.h"
#include "core/peak/IntegrationRegion.h"

namespace nsx {

enum class Level;

struct IntegrationParameters {
    double peak_end = 3.0; //!< End of peak region (sigmas) (same as peak_scale? - zamaan)
    double bkg_begin = 3.0; //!< Beginning of background region (sigmas)
    double bkg_end = 6.0; //!< End of background region (sigmas)
    double neighbour_range_pixels = 400.0; //!< Search radius for neighbouring peaks (pixels)
    double neighbour_range_frames = 20.0; //!< Search radius for neighbouring peaks (frames)
    bool fit_center = true; //!< Whether to fit the peak centre
    bool fit_cov = true; //!< Whether to fit the peak covariance

    void log(const Level& level) const;
};

class ShapeLibrary;

//! Handles per-frame integration of a peak.
class IPeakIntegrator {
 public:
    IPeakIntegrator();
    virtual ~IPeakIntegrator();
    //! Compute the integrated intensity of the peak given the integration region.
    virtual bool compute(
        Peak3D* peak, ShapeLibrary* shape_library, const IntegrationRegion& region) = 0;
    //! Integrate all peaks in the list which are contained in the specified data set.
    //! @param peak_end Peak boundary (in sigma)
    //! @param bkg_begin Background beginning (in sigma)
    //! @param bkg_end Background end (in sigma)
    void integrate(
        std::vector<nsx::Peak3D*> peaks, ShapeLibrary* shape_library, sptrDataSet data,
        int n_numor);
    //! Returns the mean background.
    Intensity meanBackground() const;
    //! Returns the integrated intensity.
    Intensity integratedIntensity() const;
    //! Returns the peak rocking curve.
    const std::vector<Intensity>& rockingCurve() const;
    //! Sets the progress handler.
    void setHandler(sptrProgressHandler handler);
    //! Set the number of numors for progress handler
    void setNNumors(int n_numors);

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
    //! Container for user-defined integration parameters
    IntegrationParameters _params;
    //! Number of numors in the dataset to be integrated
    int _n_numors;

 public:
    double peakEnd() const { return _params.peak_end; };
    double backBegin() const { return _params.bkg_begin; };
    double backEnd() const { return _params.bkg_end; };
    double radius() const { return _params.neighbour_range_pixels; };
    double nFrames() const { return _params.neighbour_range_frames; };
    bool fitCenter() const { return _params.fit_center; };
    bool fitCov() const { return _params.fit_cov; };

    void setParameters(const IntegrationParameters& params);
};

} // namespace nsx

#endif // NSX_CORE_SHAPE_IPEAKINTEGRATOR_H
