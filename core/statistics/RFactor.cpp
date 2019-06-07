/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2016- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher
 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr
 j.fisher[at]fz-juelich.de

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "RFactor.h"
#include "MergedData.h"
#include "MergedPeak.h"
#include "Peak3D.h"

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
