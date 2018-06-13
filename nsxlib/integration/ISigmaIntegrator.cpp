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
#include "ISigmaIntegrator.h"

namespace nsx {

ISigmaIntegrator::ISigmaIntegrator(sptrShapeLibrary library, double radius, double nframes):
    StrongPeakIntegrator(false, false),
    _library(library),
    _radius(radius),
    _nframes(nframes)
{

}

bool ISigmaIntegrator::compute(sptrPeak3D peak, const IntegrationRegion& region)
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
        throw std::runtime_error("ISigmaIntegrator::compute(): too few data points in peak");
    }

    std::vector<Intensity> mean_profile;
    IntegratedProfile profile;

    Eigen::Vector3d c = peak->shape().center();
    Eigen::Matrix3d A = peak->shape().metric();

    try {
        // throws if there are no neighboring peaks within the bounds
        mean_profile = _library->meanIntegratedProfile(DetectorEvent(c), _radius, _nframes);
    } catch(...) {
        return false;
    }

    // evaluate the model profile at the given events
    for (int i = 0; i < events.size(); ++i) {
        Eigen::Vector3d dx(events[i]._px, events[i]._py, events[i]._frame);
        dx -= c;
        const double r2 = dx.transpose()*A*dx;
        profile.addPoint(r2, counts[i]);
    }

    int best_idx = -1;
    double best_val = std::numeric_limits<double>::max();
    
    // now compute minimum of sigma(I)^2 / I^2 + sigma(p)^2 / p^2
    for (int i = 0; i < profile.counts().size(); ++i) {
        const double M = profile.counts()[i];
        const int n = profile.npoints()[i];
        const double I = M - n*mean_bkg;
        const double var_I = M + n*n*var_bkg;

        const double p = mean_profile[i].value();
        const double var_p = mean_profile[i].variance();

        const double val = var_I / I / I + var_p / p / p;

        if (val < best_val) {
            best_val = val;
            best_idx = i;
        }
    }

    // something went wrong (nans?)
    if (best_idx < 0) {
        return false;
    }
  
    const double M = profile.counts()[best_idx];
    const int n = profile.npoints()[best_idx];

    _integratedIntensity = Intensity(M-n*mean_bkg, M+n*n*var_bkg);
    _integratedIntensity = _integratedIntensity / mean_profile[best_idx];

    double sigma = _integratedIntensity.sigma();

    if (std::isnan(sigma) || sigma <= 0.0) {
        return false;
    }

    // TODO: rocking curve!

    return true;
}

} // end namespace nsx
