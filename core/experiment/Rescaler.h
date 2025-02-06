//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/Rescaler.h
//! @brief     Defines class Rescaler
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_EXPERIMENT_RESCALER_H
#define OHKL_CORE_EXPERIMENT_RESCALER_H

#include "core/data/DataTypes.h"
#include "base/fit/FitParameters.h"

namespace ohkl {

class MergedPeakCollection;

class Rescaler {
 public:
    Rescaler(sptrDataSet data, MergedPeakCollection* merged_peaks);

    bool optimize(unsigned int max_iter);

    const Eigen::VectorXd& scaleFactors() const { return _scale_factors; };

 private:
    int residuals(Eigen::VectorXd& fvec);

    Eigen::VectorXd _scale_factors;
    FitParameters _params;
    std::vector<std::vector<int>> _constraints;

    sptrDataSet _data;
    MergedPeakCollection* _merged_peaks;
};

} // namespace ohkl

#endif // OHKL_CORE_EXPERIMENT_RESCALER_H
