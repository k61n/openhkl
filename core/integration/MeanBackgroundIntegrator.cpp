//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/integration/MeanBackgroundIntegrator.cpp
//! @brief     Implements class MeanBackgroundIntegrator
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/integration/MeanBackgroundIntegrator.h"
#include "base/geometry/Ellipsoid.h"
#include "core/data/DataSet.h"
#include "core/peak/Intensity.h"
#include "core/peak/Peak3D.h"

namespace nsx {

bool MeanBackgroundIntegrator::compute(
    Peak3D* /*peak*/, ShapeCollection* /*shape_collection*/, const IntegrationRegion& region)
{
    double sum_bkg = 0.0;
    double sum_bkg2 = 0.0;
    size_t nbkg = 0;

    const auto& events = region.peakData().events();
    const auto& counts = region.peakData().counts();

    // TODO: should this be hard-coded??
    if (events.size() < 20) {
        nsxlog(Level::Debug, "MeanBackgroundIntegrator::compute: too few data points");
        return false;
    }

    // compute initial mean background and error
    for (auto i = 0; i < counts.size(); ++i) {
        if (region.classify(events[i]) != IntegrationRegion::EventType::BACKGROUND)
            continue;
        sum_bkg += counts[i];
        sum_bkg2 += counts[i] * counts[i];
        nbkg++;
    }

    double mean_bkg = sum_bkg / nbkg;
    double var_bkg = (sum_bkg2 - nbkg * mean_bkg * mean_bkg) / (nbkg - 1);
    double sigma_bkg = std::sqrt(var_bkg);

    // update mean, rejecting outliers
    for (auto i = 0; i < 20; ++i) {
        sum_bkg = 0;
        sum_bkg2 = 0;
        nbkg = 0;

        for (auto i = 0; i < counts.size(); ++i) {
            if (std::fabs(counts[i] - mean_bkg) > 3 * sigma_bkg
                || region.classify(events[i]) != IntegrationRegion::EventType::BACKGROUND) {
                continue;
            }
            sum_bkg += counts[i];
            sum_bkg2 += counts[i] * counts[i];
            nbkg++;
        }

        double old_mean = mean_bkg;
        mean_bkg = sum_bkg / nbkg;
        var_bkg = (sum_bkg2 - nbkg * mean_bkg * mean_bkg) / (nbkg - 1);
        sigma_bkg = std::sqrt(var_bkg);

        if (std::fabs((old_mean - mean_bkg) / mean_bkg) < 1e-9)
            break;
    }
    // note 1: assuming Poisson
    // note 2: the variance is the variance of the _estimated_ mean
    _meanBackground = Intensity(mean_bkg, mean_bkg / nbkg);

    return true;
}

} // namespace nsx
