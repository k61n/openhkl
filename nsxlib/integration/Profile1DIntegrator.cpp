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
#include "PeakCoordinateSystem.h"
#include "ShapeLibrary.h"
#include "Profile1DIntegrator.h"

namespace nsx {

Profile1DIntegrator::Profile1DIntegrator(sptrShapeLibrary library, double radius, double nframes):
    StrongPeakIntegrator(),
    _library(library),
    _radius(radius),
    _nframes(nframes)
{

}

static void updateFit(Intensity& I, Intensity& B, const std::vector<double>& dp, const std::vector<double>& dM, const std::vector<int>& dn)
{
    Eigen::Matrix2d A;
    A.setZero();
    Eigen::Vector2d b(0,0);
    const size_t n = dp.size();
    assert(dp.size() == dM.size() && dp.size() == dn.size());

    for (size_t i = 0; i < n; ++i) {
        const double p = dp[i];
        const double M = dM[i];
        const double var = B.value()*dn[i] + I.value()*dp[i];

        A(0,0) += 1/var;
        A(0,1) += p/var;
        A(1,0) += p/var;
        A(1,1) += p*p/var;

        b(0) += M/var;
        b(1) += M*p/var;
    }  

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

bool Profile1DIntegrator::compute(sptrPeak3D peak, const IntegrationRegion& region)
{
    if (!_library) {
        return false;
    }

    if (!peak) {
        return false;
    }

    // first get mean background
    StrongPeakIntegrator::compute(peak, region);
    const double mean_bkg = _meanBackground.value();
    const double var_bkg = _meanBackground.variance();

    const auto& events = region.data().events();
    const auto& counts = region.data().counts();

    // TODO: should this be hard-coded??
    if (events.size() < 29) {
        throw std::runtime_error("Profile1DIntegrator::compute(): too few data points in peak");
    }

    std::vector<Intensity> mean_profile;
    IntegratedProfile profile;

    Eigen::Vector3d c = peak->getShape().center();
    Eigen::Matrix3d A = peak->getShape().metric();

    try {
        // throws if there are no neighboring peaks within the bounds
        mean_profile = _library->meanIntegratedProfile(DetectorEvent(c), _radius, _nframes);
    } catch(...) {
        return false;
    }

    // construct the observed profile
    for (int i = 0; i < events.size(); ++i) {
        Eigen::Vector3d dx(events[i]._px, events[i]._py, events[i]._frame);
        dx -= c;
        const double r2 = dx.transpose()*A*dx;
        profile.addPoint(r2, counts[i]);
    }

    std::vector<int> dn;
    std::vector<double> dm;
    std::vector<double> dp;
    double p0 = mean_profile[0].value();

    // compute differences and rebin if necessary so that dn > 0
    for (size_t i = 1; i < mean_profile.size(); ++i) {
        const auto& counts = profile.counts();
        const auto& npoints = profile.npoints();

        if (npoints[i] > npoints[i-1]) {
            dn.push_back(npoints[i]-npoints[i-1]);
            dm.push_back(counts[i]-counts[i-1]);
            dp.push_back(mean_profile[i].value()-p0);
            p0 = mean_profile[i].value();
        }
    }

    Intensity I = 1e-6;
    Intensity B = _meanBackground.value();

    for (auto i = 0; i < 10 && I.value() > 0; ++i) {
        updateFit(I, B, dp, dm, dn);
    }

    double sigma = _integratedIntensity.sigma();

    if (std::isnan(sigma) || sigma <= 0.0) {
        return false;
    }

    // TODO: rocking curve!

    return true;
}

} // end namespace nsx
