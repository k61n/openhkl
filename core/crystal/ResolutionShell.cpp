//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/crystal/ResolutionShell.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <cmath>
#include <limits>

#include <Eigen/Eigenvalues>

#include "core/peak/Peak3D.h"
#include "core/rec_space/ReciprocalVector.h"
#include "core/crystal/ResolutionShell.h"

namespace nsx {

ResolutionShell::ResolutionShell(double dmin, double dmax, size_t num_shells)
    : _shells(std::max(num_shells, size_t(1)))
{
    size_t n_shells = _shells.size();

    const double q3max = std::pow(dmin, -3);
    const double dq3 = (std::pow(dmin, -3) - std::pow(dmax, -3)) / double(n_shells);

    _shells[0].dmin = dmin;

    for (size_t i = 0; i < n_shells - 1; ++i) {
        _shells[i].dmax = std::pow(q3max - (i + 1) * dq3, -1.0 / 3.0);
        _shells[i + 1].dmin = _shells[i].dmax;
    }

    _shells[n_shells - 1].dmax = dmax;
}

void ResolutionShell::addPeak(const sptrPeak3D& peak)
{
    auto q = peak->q();
    const double d = 1.0 / q.rowVector().norm();

    double dmin;
    double dmax;
    for (size_t i = 0; i < _shells.size(); ++i) {
        dmin = _shells[i].dmin;
        dmax = _shells[i].dmax;
        if (dmin <= d && d <= dmax) {
            _shells[i].peaks.push_back(peak);
            return;
        }
    }
}

const DShell& ResolutionShell::shell(size_t i) const
{
    if (i >= _shells.size()) {
        throw std::runtime_error("ResolutionShell::shell index out of bounds");
    }
    return _shells[i];
}

size_t ResolutionShell::nShells() const
{
    return _shells.size();
}

} // end namespace nsx
