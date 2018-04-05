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
#include "GaussianIntegrator.h"

namespace nsx {

GaussianIntegrator::GaussianIntegrator(): IPeakIntegrator()
{

}

static void updateFit(Intensity& I, Intensity& B, const std::vector<double>& profile, const std::vector<double>& counts)
{
    Eigen::Matrix2d A;
    A.setZero();
    Eigen::Vector2d b(0,0);
    const size_t n = std::min(profile.size(), counts.size());

    for (size_t i = 0; i < n; ++i) {
        const double p = profile[i];
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

bool GaussianIntegrator::compute(sptrPeak3D peak, const IntegrationRegion& region)
{
    if (!peak) {
        return false;
    }

    const auto& obs_counts = region.data().counts();
    std::vector<double> counts(obs_counts.size());

    for (size_t i = 0; i < counts.size(); ++i) {
        counts[i] = obs_counts[i];
    }

    const auto& profile = this->profile(peak, region);
    const double tolerance = 1e-6;

    // first give dummy values
    _integratedIntensity = Intensity(1e-2);
    _meanBackground = Intensity(1.0);

    // todo: stopping criterion
    for (size_t i = 0; i < 20; ++i) {
        Intensity old_intensity = _integratedIntensity;
        const double I0 = _integratedIntensity.value();
        updateFit(_integratedIntensity, _meanBackground, profile, counts);
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

std::vector<double> GaussianIntegrator::profile(sptrPeak3D peak, const IntegrationRegion& region)
{
    const auto& events = region.data().events();
    const Eigen::Matrix3d A = peak->getShape().metric();
    const Eigen::Vector3d x0 = peak->getShape().center();
    const double factor = std::sqrt(A.determinant() / 8.0 / M_PI / M_PI / M_PI);
    std::vector<double> result(events.size(), 0.0);

    for (size_t i = 0; i < events.size(); ++i) {
        const DetectorEvent& ev = events[i];
        Eigen::Vector3d dx(ev._px, ev._py, ev._frame);
        dx -= x0;
        result[i] = std::exp(-0.5 * dx.dot(A*dx)) * factor;
    }
    return result;
}

} // end namespace nsx
