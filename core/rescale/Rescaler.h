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
        std::vector<PeakCollection*> collections, SpaceGroup group, bool friedel,
        bool sum_intensity = true);

    void updateScaleFactors();
    void merge();

    void setFTol(double ftol) { _ftol = ftol; };
    void setCTol(double ctol) { _ctol = ctol; };
    void setMaxIter(double max_iter) { _max_iter = max_iter; };

    std::optional<double> rescale();

    MergedPeakCollection* mergedPeaks() { return _merged_peaks.get(); };

 private:
    std::vector<PeakCollection*> _peak_collections;
    SpaceGroup _space_group;
    bool _friedel;
    bool _sum_intensity;

    std::map<PeakCollection*, std::vector<double*>> _scale_factors;
    std::vector<double> _parameters;
    std::vector<EqualityConstraintData> _equality_constraints;
    std::vector<InequalityConstraintData> _inequality_constraints;

    std::unique_ptr<MergedPeakCollection> _merged_peaks;

    double _ftol;
    double _ctol;
    unsigned int _max_iter;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_RESCALE_RESCALER_H
