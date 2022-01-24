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

class IntegrationRegion;
enum class Level;

enum class IntegratorType { PixelSum, Gaussian, ISigma, Profile1D, Profile3D, Count };

/*! \addtogroup python_api
 *  @{*/

//! Structure containing parameters for all integrators
struct IntegrationParameters {
    double peak_end = 3.0; //!< End of peak region (sigmas)
    double bkg_begin = 3.0; //!< Beginning of background region (sigmas)
    double bkg_end = 6.0; //!< End of background region (sigmas)
    double neighbour_range_pixels = 500.0; //!< Search radius for neighbouring peaks (pixels)
    double neighbour_range_frames = 10.0; //!< Search radius for neighbouring peaks (frames)
    bool fit_center = true; //!< Whether to update the peak centre after integration
    bool fit_cov = true; //!< Whether to update the peak covariance after integration
    int min_neighbors = 10; //!< Minimum number of neighbouring shapes for predicted shape
    IntegratorType integrator_type = IntegratorType::PixelSum; //!< Type of integrator
    RegionType region_type =
        RegionType::VariableEllipsoid; //!< Set peak end in pixels instead of sigmas

    void log(const Level& level) const;
};

class ShapeCollection;

/*! \brief Base class for integrators. Handles per-frame integration of a peak.
 *
 *  All integrators inherit from this class.
 */
class IPeakIntegrator {
 public:
    IPeakIntegrator();
    virtual ~IPeakIntegrator();
    //! Compute the integrated intensity of the peak given the integration region.
    virtual bool compute(
        Peak3D* peak, ShapeCollection* shape_collection, const IntegrationRegion& region) = 0;
    //! Integrate all peaks in the list which are contained in the specified data set.
    void integrate(
        std::vector<nsx::Peak3D*> peaks, ShapeCollection* shape_collection, sptrDataSet data,
        int n_numor);
    //! Return the mean background.
    Intensity meanBackground() const;
    //! Return the integrated intensity.
    Intensity integratedIntensity() const;
    //! Return the peak rocking curve.
    const std::vector<Intensity>& rockingCurve() const;
    //! Set the progress handler.
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
    //! Return the peak scale
    double peakEnd() const { return _params.peak_end; };
    //! Return the beginning of the background region in peak scales
    double backBegin() const { return _params.bkg_begin; };
    //! Return the end of the background region in peak scales
    double backEnd() const { return _params.bkg_end; };
    //! Return the neighbour search radius in pixels (profile integration)
    double radius() const { return _params.neighbour_range_pixels; };
    //! Return the neighbour search radius in frames (profile integration)
    double nFrames() const { return _params.neighbour_range_frames; };
    //! Update the peak center as part of integration
    bool fitCenter() const { return _params.fit_center; };
    //! Update the peak shape covariance matrix as part of integration
    bool fitCov() const { return _params.fit_cov; };

    //! Assign a parameter set to the integrator
    void setParameters(const IntegrationParameters& params);
};

/*! @}*/
} // namespace nsx

#endif // NSX_CORE_SHAPE_IPEAKINTEGRATOR_H
