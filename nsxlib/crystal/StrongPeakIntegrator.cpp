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
#include "StrongPeakIntegrator.h"

namespace nsx {

StrongPeakIntegrator::StrongPeakIntegrator(): IPeakIntegrator()
{

}

bool StrongPeakIntegrator::compute(sptrPeak3D peak, const IntegrationRegion& region)
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
        throw std::runtime_error("StrongPeakIntegrator::compute(): too few data points in peak");
    }

    // TODO: should this be hard-coded??
    if (bkgEvents.size() < 5) {
        throw std::runtime_error("StrongPeakIntegrator::compute(): too few data points in background");
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
    _integratedIntensity = Intensity(sum_peak, sum_peak);

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

    return true;
}

} // end namespace nsx
