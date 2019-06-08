//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/statistics/RFactor.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/statistics/RFactor.h"
#include "core/peak/MergedData.h"
#include "core/peak/MergedPeak.h"
#include "core/peak/Peak3D.h"

static const double g_sqrt2pi = std::sqrt(2.0 / M_PI);

namespace nsx {

void RFactor::calculate(const MergedData& data)
{
    auto&& peaks = data.peaks();

    _Rmerge = 0;
    _Rmeas = 0;
    _Rpim = 0;

    _expectedRmerge = 0;
    _expectedRmeas = 0;
    _expectedRpim = 0;

    double I_total = 0.0;

    // go through each equivalence class of peaks
    for (auto&& peak : peaks) {
        const double n = double(peak.redundancy());

        // skip if there are fewer than two peaks
        if (n < 1.999) {
            continue;
        }

        const double Iave = peak.intensity().value();
        const double Fmeas = std::sqrt(n / (n - 1));
        const double Fpim = std::sqrt(1 / (n - 1));

        I_total += std::fabs(Iave) * peak.redundancy();

        for (auto&& p : peak.peaks()) {
            auto I = p->correctedIntensity();

            double diff = std::fabs(I.value() - Iave);

            _Rmerge += diff;
            _Rmeas += Fmeas * diff;
            _Rpim += Fpim * diff;

            _expectedRmerge += I.sigma();
            _expectedRmeas += I.sigma() * Fmeas;
            _expectedRpim += I.sigma() * Fpim;
        }
    }

    if (I_total <= 0.0) {
        // something wrong! too few peaks?
        _Rmerge = 0.0;
        _Rmeas = 0.0;
        _Rpim = 0.0;

        _expectedRmerge = 0;
        _expectedRmeas = 0;
        _expectedRpim = 0;
    } else {
        _Rmerge /= I_total;
        _Rmeas /= I_total;
        _Rpim /= I_total;

        _expectedRmerge *= g_sqrt2pi / I_total;
        _expectedRmeas *= g_sqrt2pi / I_total;
        _expectedRpim *= g_sqrt2pi / I_total;
    }
}

} // end namespace nsx
