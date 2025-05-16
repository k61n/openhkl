//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/rescale/Rescaler.h
//! @brief     Defines class Rescaler
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_RESCALE_RESCALER_H
#define OHKL_CORE_RESCALE_RESCALER_H

#include "base/fit/MinimizerNLopt.h"
#include "base/utils/Logger.h"
#include "core/statistics/MergedPeakCollection.h"
#include "tables/crystal/SpaceGroup.h"

#include <map>
#include <memory>
#include <optional>
#include <vector>

namespace ohkl {

class PeakCollection;

struct RescalerParameters {
    bool sum_intensity = true; //! Use sum integrated intensity (not profile)
    bool friedel = true; //! Use Friedel's law for merging
    double ftol = 1.0e-4; //! Relative tolerance for objective function evaluations
    double xtol = 1.0e-6; //! Relative tolerance for parameters
    double ctol = 1.0e-6; //! Constraint tolerance
    int max_iter = 1e7; //! Maximum number of iterations
    double frame_ratio = 0.05; //! Maximum variation in scale factor between adjacent frames

    void log(const Level& level) const;
};

/*! \brief Rescale peak intensity as a function of image number
 *
 * Peak intensities are scaled by a factor dependent on image number. Optimize the scale
 * factors such that the R-factor (Rmerge) of the peak collection is minimized. The first
 * scale factor is fixed to 1, and each scale factor can vary by a maximum of 5% relative
 * to its neighbours.
 */
class Rescaler {
 public:
    Rescaler();

    static double objective(
        const std::vector<double>& params, std::vector<double>& grad, void* f_data);
    static double equality_constraint(
        const std::vector<double>& params, std::vector<double>& grad, void* f_data);
    static double inequality_constraint(
        const std::vector<double>& params, std::vector<double>& grad, void* f_data);

    void setPeakCollection(PeakCollection* collection, const SpaceGroup& group);
    void updateScaleFactors(const std::vector<double>& parameters);
    void merge();

    double rfactor() const;
    double sumChi2() const;

    std::optional<double> rescale();

    MergedPeakCollection* mergedPeaks() { return _merged_peaks.get(); };
    const std::vector<double>& scaleFactors() const { return _scale_factors; };
    const std::vector<double>& convergence() const { return _minf; };
    int nIter() const { return _niter; };
    RescalerParameters* parameters() { return &_parameters; };

 private:
    PeakCollection* _peak_collection;
    SpaceGroup _space_group;
    RescalerParameters _parameters;

    std::vector<double> _scale_factors;
    std::vector<EqualityConstraintData> _equality_constraints;
    std::vector<InequalityConstraintData> _inequality_constraints;

    std::unique_ptr<MergedPeakCollection> _merged_peaks;

    static std::vector<double> _minf;
    static int _niter;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_RESCALE_RESCALER_H
