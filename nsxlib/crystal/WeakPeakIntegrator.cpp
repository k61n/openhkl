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
#include "WeakPeakIntegrator.h"

namespace nsx {

WeakPeakIntegrator::WeakPeakIntegrator(sptrShapeLibrary library, double radius, double nframes):
    IPeakIntegrator(), _library(library), _radius(radius), _nframes(nframes)
{

}

// note that this assumes the profile has been normalized so that \sum_i p_i = 1
static void updateFit(Intensity& I, Intensity& B, const PeakCoordinateSystem& frame, const FitProfile& profile, const IntegrationRegion& region)
{
    Eigen::Matrix2d A;
    A.setZero();
    Eigen::Vector2d b(0,0);
    const auto& shape = region.shape();

    const auto& events = region.peakData().events();
    const auto& counts = region.peakData().counts();

    for (size_t i = 0; i < events.size(); ++i) {
        const Eigen::Vector3d s = frame.transform(events[i]);
        const double p = profile.predict(s);
        const double M = counts[i];
        const double var = B.value() + I.value()*p;

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

bool WeakPeakIntegrator::compute(sptrPeak3D peak, const IntegrationRegion& region)
{
    if (!peak || !_library) {
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


    const double tolerance = 1e-5;
    auto profile = _library->average(DetectorEvent(peak->getShape().center()), _radius, _nframes);
    PeakCoordinateSystem frame(peak);

    // todo: stopping criterion
    for (auto i = 0; i < 20; ++i) {
        Intensity old_intensity = _integratedIntensity;
        const double I0 = _integratedIntensity.value();
        updateFit(_integratedIntensity, _meanBackground, frame, profile, region);
        const double I1 = _integratedIntensity.value();

        if (std::isnan(I1) || std::isnan(_meanBackground.value())) {
            _integratedIntensity = old_intensity;
            break;
        }

        if (I1 < 0.0 || (I1 < (1+tolerance)*I0 && I0 < (1+tolerance)*I1)) {
            break;
        }
    }

    double sigma = _integratedIntensity.sigma();

    if (std::isnan(sigma) || sigma <= 0.0) {
        return false;
    }

    // TODO: rocking curve!

    return true;
}

} // end namespace nsx
