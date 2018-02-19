/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2017- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
          Jonathan Fisher, Forschungszentrum Juelich GmbH
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

#include "DataSet.h"
#include "Ellipsoid.h"
#include "Intensity.h"
#include "PeakIntegrator.h"

namespace nsx {

static double profile(const Ellipsoid& shape, const DetectorEvent& ev)
{
    Eigen::Vector3d x = {ev._px, ev._py, ev._frame};
    x -= shape.center();
    return std::exp(-0.5*x.dot(shape.metric()*x));
}

static void updateFit(Intensity& I, Intensity& B, const IntegrationRegion& region)
{
    Eigen::Matrix2d A;
    A.setZero();
    Eigen::Vector2d b(0,0);
    double sum_p = 0.0;
    const auto& shape = region.shape();

    auto updateA = [&](const PeakData& data)
    {
        const auto& events = data.events();
        const auto& counts = data.counts();

        for (size_t i = 0; i < events.size(); ++i) {
            const double p = profile(shape, events[i]);
            const double M = counts[i];
            const double var = B.value() + I.value()*p;

            sum_p += p;

            A(0,0) += 1/var;
            A(0,1) += p/var;
            A(1,0) += p/var;
            A(1,1) += p*p/var;

            b(0) += M/var;
            b(1) += M*p/var;
        }
    };
    
    updateA(region.peakData());
    updateA(region.bkgData());

    const Eigen::Vector2d& x = A.fullPivLu().solve(b);

    const double new_B = x(0);
    const double new_I = x(1)*sum_p;

    B = Intensity(new_B);
    I = Intensity(new_I);
}

Intensity PeakIntegrator::meanBackground() const
{
    return _meanBackground;
}

Intensity PeakIntegrator::peakIntensity() const
{
    return _peakIntensity;
}

const std::vector<Intensity>& PeakIntegrator::rockingCurve() const
{
    return _rockingCurve;
}

void PeakIntegrator::compute(const IntegrationRegion& region)
{
    double sum_bkg = 0.0;
    double sum_bkg2 = 0.0;
    double sum_peak = 0.0;

    const auto& peakEvents = region.peakData().events();
    const auto& peakCounts = region.peakData().counts();
    const auto& bkgEvents = region.bkgData().events();
    const auto& bkgCounts = region.bkgData().counts();

    // TODO: should this be hard-coded??
    if (peakEvents.size() < 5) {
        throw std::runtime_error("PeakIntegrator::compute(): too few data points in peak");
    }

    // TODO: should this be hard-coded??
    if (bkgEvents.size() < 5) {
        throw std::runtime_error("PeakIntegrator::compute(): too few data points in background");
    }

    // compute mean background and error
    for (auto count: bkgCounts) {
        sum_bkg += count;
        sum_bkg2 += count*count;
    }

    const double Nbkg = bkgCounts.size();
    const double mean_bkg = sum_bkg / Nbkg;
    const double var_bkg = (sum_bkg2 - Nbkg*mean_bkg) / (Nbkg-1);

    _meanBackground = Intensity(mean_bkg, var_bkg);

    // compute total peak intensity
    for (auto count: peakCounts) {
        sum_peak += count;
    }
    sum_peak -= peakCounts.size()*mean_bkg;

    // TODO: ERROR ESTIMATE!!
    _peakIntensity = Intensity(sum_peak, std::sqrt(sum_peak));

    // compute rocking curve
    double f_min = int(peakEvents[0]._frame);
    double f_max = f_min;

    size_t Npeak = peakEvents.size();

    for (size_t i = 0; i < Npeak; ++i) {
        f_min = std::min(peakEvents[i]._frame, f_min);
        f_max = std::max(peakEvents[i]._frame, f_max);
    }

    size_t nframes = size_t(f_max-f_min)+1;
    _rockingCurve.resize(nframes);

    // fitted intensity and background
    _fitBackground = _meanBackground;
    _fitIntensity = 0.0;

    // todo: stopping criterion
    for (auto i = 0; i < 3; ++i) {
        updateFit(_fitIntensity, _fitBackground, region);
    }
}

Intensity PeakIntegrator::fitBackground() const
{
    return _fitBackground;
}

Intensity PeakIntegrator::fitIntensity() const
{
    return _fitIntensity;
}

} // end namespace nsx
