//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/ResolutionShell.cpp
//! @brief     Implements class ResolutionShell
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/statistics/ResolutionShell.h"

#include "base/geometry/ReciprocalVector.h"
#include "core/peak/Peak3D.h"

namespace ohkl {

ResolutionShell::ResolutionShell(double dmin, double dmax, std::size_t num_shells)
    : _shells(std::max(num_shells, std::size_t(1)))
{
    std::size_t n_shells = _shells.size();

    const double q3max = std::pow(dmin, -3);
    const double dq3 = (std::pow(dmin, -3) - std::pow(dmax, -3)) / double(n_shells);

    _shells[0].dmin = dmin;

    for (std::size_t i = 0; i < n_shells - 1; ++i) {
        _shells[i].dmax = std::pow(q3max - (i + 1) * dq3, -1.0 / 3.0);
        _shells[i + 1].dmin = _shells[i].dmax;
    }

    _shells[n_shells - 1].dmax = dmax;
}

void ResolutionShell::addPeak(Peak3D* peak)
{
    // Not discarding peaks that are disabled yet because they are used to compute completeness
    auto q = peak->q();
    if (q.isValid()) {
        const double d = 1.0 / q.rowVector().norm();

        double dmin;
        double dmax;
        for (std::size_t i = 0; i < _shells.size(); ++i) {
            dmin = _shells[i].dmin;
            dmax = _shells[i].dmax;
            if (dmin <= d && d <= dmax) {
                _shells[i].peaks.push_back(peak);
                return;
            }
        }
    } else { // interpolation error
        return;
    }
}

const DShell& ResolutionShell::shell(std::size_t i) const
{
    if (i >= _shells.size())
        throw std::runtime_error("ResolutionShell::shell index out of bounds");
    return _shells[i];
}

std::size_t ResolutionShell::nShells() const
{
    return _shells.size();
}

} // namespace ohkl
