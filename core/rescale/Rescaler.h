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
#include "core/statistics/MergedPeakCollection.h"
#include "tables/crystal/SpaceGroup.h"

#include <map>
#include <memory>
#include <optional>
#include <vector>

namespace ohkl {

class PeakCollection;

/*! \brief Rescale peak intensity as a function of image number
 *
 *
 *
 *
 *
 */
class Rescaler {
 public:
    Rescaler(
        PeakCollection* collection, SpaceGroup group, bool friedel,
        bool sum_intensity = true);

    static double objective(
        const std::vector<double>& params, std::vector<double>& grad, void* f_data);
    static double equality_constraint(
        const std::vector<double>& params, std::vector<double>& grad, void* f_data);
    static double inequality_constraint(
        const std::vector<double>& params, std::vector<double>& grad, void* f_data);

    void updateScaleFactors(const std::vector<double>& parameters);
    void merge();

    double rfactor() const;
    double sumChi2() const;

    void setFTol(double ftol) { _ftol = ftol; };
    void setXTol(double xtol) { _xtol = xtol; };
    void setCTol(double ctol) { _ctol = ctol; };
    void setMaxIter(double max_iter) { _max_iter = max_iter; };
    void setInitStep(double init_step) { _init_step = init_step; };

    std::optional<double> rescale();

    MergedPeakCollection* mergedPeaks() { return _merged_peaks.get(); };
    const std::vector<double>& parameters() const { return _parameters; };
    int nIter() const { return _niter; };
    bool sumIntensity() const { return _sum_intensity; };

 private:
    PeakCollection* _peak_collection;
    SpaceGroup _space_group;
    bool _friedel;
    bool _sum_intensity;

    std::vector<double> _parameters;
    std::vector<EqualityConstraintData> _equality_constraints;
    std::vector<InequalityConstraintData> _inequality_constraints;

    std::unique_ptr<MergedPeakCollection> _merged_peaks;

    //! The maximum fraction by which scale factor can differ from adjacent images
    double _frame_ratio;

    // Minimizer parameters
    double _ftol;
    double _xtol;
    double _ctol;
    unsigned int _max_iter;
    double _init_step;

    static std::vector<double> _minf;
    static int _niter;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_RESCALE_RESCALER_H
