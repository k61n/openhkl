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
#include "Peak3D.h"
#include "WeakPeakIntegrator.h"

namespace nsx {

WeakPeakIntegrator::WeakPeakIntegrator(): IPeakIntegrator()
{

}

// note that this assumes the profile has been normalized so that \sum_i p_i = 1
static void updateFit(Intensity& I, Intensity& B, const Eigen::RowVector3d& q_pred, const FitProfile& profile, const IntegrationRegion& region)
{
    Eigen::Matrix2d A;
    A.setZero();
    Eigen::Vector2d b(0,0);
    const auto& shape = region.shape();

    auto updateA = [&](const PeakData& data)
    {
        const auto& events = data.events();
        const auto& counts = data.counts();
        const auto& qs = data.qs();

        for (size_t i = 0; i < events.size(); ++i) {
            const double p = profile.predict(qs[i].rowVector() - q_pred);
            const double M = counts[i];
            const double var = B.value() + I.value()*p;

            A(0,0) += 1/var;
            A(0,1) += p/var;
            A(1,0) += p/var;
            A(1,1) += p*p/var;

            b(0) += M/var;
            b(1) += M*p/var;
        }
    };
    
    updateA(region.peakData());
    //updateA(region.bkgData());

    Eigen::Matrix2d AI = A.inverse();

    const Eigen::Vector2d& x = AI*b;

    const double new_B = x(0);
    const double new_I = x(1);

    // check this calculation!
    Eigen::Matrix2d cov = AI;

    // Note: this error estimate assumes the variances are correct (i.e., gain and baseline accounted for)
    B = Intensity(new_B, cov(0,0));
    I = Intensity(new_I, cov(1,1));
}

bool WeakPeakIntegrator::compute(sptrPeak3D peak, const IntegrationRegion& region)
{
    if (!peak || !peak->profile()) {
        return false;
    }

    double sum_bkg = 0.0;
    double sum_bkg2 = 0.0;

    const auto& bkgEvents = region.bkgData().events();
    const auto& bkgCounts = region.bkgData().counts();

    // TODO: should this be hard-coded??
    if (region.peakData().events().size() < 5) {
        throw std::runtime_error("WeakPeakIntegrator::compute(): too few data points in peak");
    }

    // TODO: should this be hard-coded??
    if (bkgEvents.size() < 5) {
        throw std::runtime_error("WeakPeakIntegrator::compute(): too few data points in background");
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
    _integratedIntensity = Intensity(0.0, 0.0);

    const Eigen::RowVector3d q_pred = peak->qPredicted().rowVector();

    const double tolerance = 1e-5;

    // todo: stopping criterion
    for (auto i = 0; i < 20; ++i) {
        const double I0 = _integratedIntensity.value();
        updateFit(_integratedIntensity, _meanBackground, q_pred, *peak->profile(), region);
        const double I1 = _integratedIntensity.value();

        if (I1 < 0.0 || (I1 < (1+tolerance)*I0 && I0 < (1+tolerance)*I1)) {
            break;
        }
    }

    // TODO: rocking curve!

    return true;
}

} // end namespace nsx
