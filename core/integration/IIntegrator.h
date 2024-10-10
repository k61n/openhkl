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

#include "core/data/DataTypes.h"
#include "core/data/ImageGradient.h"
#include "core/peak/IntegrationRegion.h"
#include "core/peak/Intensity.h"

#include <optional>

namespace ohkl {

enum class Level;
enum class RejectionFlag;
class Profile;

enum class IntegratorType { PixelSum = 0, Gaussian, ISigma, Profile1D, Profile3D, Shape, Count };

/*! \addtogroup python_api
 *  @{*/

//! Return value of all integrator compute methods. Contains any mutable data from integration.
struct ComputeResult {
    //! Construct an invalid result by default
    ComputeResult();
    ComputeResult(const ComputeResult& other) = default;
    ComputeResult& operator=(const ComputeResult& other) = default;

    //! Rejection flag for integration
    RejectionFlag integration_flag;
    //! Pixel sum integrated intensity after background subtraction
    Intensity sum_intensity;
    //! Profile integrated intensity after background correction
    Intensity profile_intensity;
    //! Mean local pixel sum background of peak. The uncertainty is the uncertainty of the
    //! _estimate_ of the background.
    Intensity sum_background;
    //! Profile integrated background of peak. The uncertainty is the uncertainty of the
    //! _estimate_ of the background.
    Intensity profile_background;
    //! Mean gradient of background (Gaussian statistics)
    Intensity bkg_gradient;
    //! The rocking curve of the peak.
    std::vector<Intensity> rocking_curve;
    //! Type of integrator used
    IntegratorType integrator_type;
    //! Shape of peak when centre/covariance are changed during integration
    std::optional<Ellipsoid> shape;
};

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
    //! Maximum strength for profile integration
    double max_strength = 1;
    //! Maximum d (minimum resolution) for profile integration
    double max_d = 2.5;
    //! Maximum number of images a peak/shape can span
    int max_width = 10;
    //! Whether to update the peak centre after integration
    bool fit_center = true;
    //! Whether to update the peak covariance after integration
    bool fit_cov = true;
    //! Whether to discard peaks with saturated pixels
    bool discard_saturated = false;
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
    //! Whether to skip peaks that intersect masks
    bool skip_masked = true;
    //! Whether to remove peaks with overlapping peak integration regions
    bool remove_overlaps = false;
    //! Skip profile integration of peaks with strength above threshold
    bool use_max_strength = false;
    //! Skip profile integration of peaks with resolution below threshold
    bool use_max_d = false;
    //! Skip integration of peaks spanning too many images
    bool use_max_width = false;

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
    virtual ~IIntegrator() = default;
    //! Integrate a vector of peaks
    void integrate(std::vector<Peak3D*> peaks, ShapeModel* shape_model, sptrDataSet data);
    //! Thread-parallel integrate
    void parallelIntegrate(std::vector<Peak3D*> peaks, ShapeModel* shape_model, sptrDataSet data);
    //! Set the progress handler.
    void setHandler(sptrProgressHandler handler);
    //! Assign a parameter set to the integrator
    void setParameters(const IntegrationParameters& params);
    //! Toggle parallel integration
    void setParallel(bool parallel) { _thread_parallel = parallel; };
    //! Remove overlapping peak intensity regions
    void removeOverlaps(const std::map<Peak3D*, std::unique_ptr<IntegrationRegion>>& regions);

 protected:
    //! Optional pointer to progress handler.
    sptrProgressHandler _handler;
    //! Container for user-defined integration parameters
    IntegrationParameters _params;

    sptrDataSet _data;
    std::atomic_int _n_failures;
    std::atomic_int _n_frames_done;
    std::atomic_int _n_peaks_done;
    bool _profile_integration;
    double _peak_end;
    double _bkg_begin;
    double _bkg_end;

    bool _thread_parallel;
    unsigned int _max_threads;

 private:
    //! Compute the integrated intensity of the peak given the integration region.
    virtual ComputeResult compute(
        Peak3D* peak, Profile* profile, const IntegrationRegion& region) = 0;
    //! Construct the correct profile depending on the integrator type
    Profile* buildProfile(Peak3D* peak, ShapeModel* shapes);
};

/*! @}*/
} // namespace ohkl

#endif // OPENHKL_CORE_INTEGRATION_IINTEGRATOR_H
