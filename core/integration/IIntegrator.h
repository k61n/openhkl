//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/integration/IIntegrator.h
//! @brief     Defines class IIntegrator
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OPENHKL_CORE_INTEGRATION_IINTEGRATOR_H
#define OPENHKL_CORE_INTEGRATION_IINTEGRATOR_H

#include "base/utils/ProgressHandler.h"
#include "core/data/ImageGradient.h"
#include "core/peak/IntegrationRegion.h"

namespace ohkl {

class IntegrationRegion;
enum class Level;

enum class IntegratorType { PixelSum, Gaussian, ISigma, Profile1D, Profile3D, Count };

/*! \addtogroup python_api
 *  @{*/

//! Structure containing parameters for all integrators
struct IntegrationParameters {
    //! End of peak region for RegionType::VariableEllipsoid (sigmas)
    double peak_end = 3.0;
    //! Beginning of background region (sigmas)
    double bkg_begin = 3.0;
    //! End of background region (sigmas)
    double bkg_end = 6.0;
    //! End of peak region for RegionType::FixedEllipsoid (pixels)
    double fixed_peak_end = 8;
    //! Beginning of background region (factor of fixed_peak_end)
    double fixed_bkg_begin = 1.0;
    //! End of background region (factor of fixed_peak_end)
    double fixed_bkg_end = 2.0;
    //! Maximum per-pixel count
    double max_counts = 50000.0;
    //! Search radius for neighbouring peaks (pixels)
    double neighbour_range_pixels = 500.0;
    //! Search radius for neighbouring peaks (frames)
    double neighbour_range_frames = 10.0;
    //! Whether to update the peak centre after integration
    bool fit_center = true;
    //! Whether to update the peak covariance after integration
    bool fit_cov = true;
    //! Whether to discard peaks with saturated pixels
    bool discard_saturated = false;
    //! Minimum number of neighbouring shapes for predicted shape
    int min_neighbors = 10;
    //! Type of integrator
    IntegratorType integrator_type = IntegratorType::PixelSum;
    //! Use gradient to discriminate heterogeneous background regions
    bool use_gradient = false;
    //! Kernel to use for gradient convolution
    GradientKernel gradient_type = GradientKernel::Sobel;
    //! Whether to use FFT or real space gradient computation
    bool fft_gradient = false;
    //! Whether to use fixed or sigma-dependent integration regions
    RegionType region_type = RegionType::VariableEllipsoid;

    void log(const Level& level) const;
};

class ShapeModel;

/*! \brief Base class for integrators. Handles per-frame integration of a peak.
 *
 *  All integrators inherit from this class.
 */
class IIntegrator {
 public:
    IIntegrator();
    virtual ~IIntegrator();
    //! Integrate all peaks in the list which are contained in the specified data set.
    void integrate(std::vector<Peak3D*> peaks, ShapeModel* shape_model, sptrDataSet data);
    //! Return the mean background.
    Intensity meanBackground() const;
    //! Return the mean background gradient
    Intensity meanBkgGradient() const;
    //! Return the integrated intensity.
    Intensity integratedIntensity() const;
    //! Return the peak rocking curve.
    const std::vector<Intensity>& rockingCurve() const;
    //! Set the progress handler.
    void setHandler(sptrProgressHandler handler);

 protected:
    //! Mean local background of peak. The uncertainty is the uncertainty of the
    //! _estimate_ of the background.
    Intensity _meanBackground;
    //! Mean gradient of background (Gaussian statistics)
    Intensity _meanBkgGradient;
    //! Net integrated intensity, after background correction.
    Intensity _integratedIntensity;
    //! The rocking curve of the peak.
    std::vector<Intensity> _rockingCurve;
    //! Optional pointer to progress handler.
    sptrProgressHandler _handler;
    //! Container for user-defined integration parameters
    IntegrationParameters _params;

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

 private:
    //! Compute the integrated intensity of the peak given the integration region.
    virtual bool compute(
        Peak3D* peak, ShapeModel* shape_model, const IntegrationRegion& region) = 0;
};

/*! @}*/
} // namespace ohkl

#endif // OPENHKL_CORE_INTEGRATION_IINTEGRATOR_H
