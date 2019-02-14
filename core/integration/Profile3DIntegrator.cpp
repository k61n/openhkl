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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
 *
 */

#include "Profile3DIntegrator.h"
#include "DataSet.h"
#include "Ellipsoid.h"
#include "Intensity.h"
#include "Peak3D.h"
#include "PeakCoordinateSystem.h"
#include "ShapeLibrary.h"

namespace nsx {

Profile3DIntegrator::Profile3DIntegrator(sptrShapeLibrary library,
                                         double radius, double nframes,
                                         bool detector_space)
    : _library(library), _radius(radius), _nframes(nframes) {}

static void updateFit(Intensity &I, Intensity &B,
                      const std::vector<double> &profile,
                      const std::vector<double> &counts) {
  Eigen::Matrix2d A;
  A.setZero();
  Eigen::Vector2d b(0, 0);
  const size_t n = std::min(profile.size(), counts.size());

  for (size_t i = 0; i < n; ++i) {
    const double p = profile[i];
    const double M = counts[i];
    const double var = B.value() + I.value() * p;

    A(0, 0) += 1 / var;
    A(0, 1) += p / var;
    A(1, 0) += p / var;
    A(1, 1) += p * p / var;

    b(0) += M / var;
    b(1) += M * p / var;
  }

  Eigen::Matrix2d AI = A.inverse();
  const Eigen::Vector2d &x = AI * b;

  const double new_B = x(0);
  const double new_I = x(1);

  // check this calculation!
  Eigen::Matrix2d cov = AI;

  // Note: this error estimate assumes the variances are correct (i.e., gain and
  // baseline accounted for)
  B = Intensity(new_B, cov(0, 0));
  I = Intensity(new_I, cov(1, 1));
}

bool Profile3DIntegrator::compute(sptrPeak3D peak,
                                  const IntegrationRegion &region) {
  if (!_library) {
    return false;
  }

  if (!peak) {
    return false;
  }

  const auto &events = region.data().events();
  const auto &counts = region.data().counts();

  // TODO: should this be hard-coded??
  if (events.size() < 29) {
    throw std::runtime_error(
        "Profile3DIntegrator::compute(): too few data points in peak");
  }

  // dummy value for initial guess
  _meanBackground = Intensity(1.0, 1.0);
  _integratedIntensity = Intensity(0.0, 0.0);

  std::vector<double> profile;
  std::vector<double> obs_counts;

  profile.reserve(events.size());
  obs_counts.reserve(events.size());

  const double tolerance = 1e-5;

  Profile3D model_profile;
  DetectorEvent event(peak->shape().center());

  try {
    // throws if there are no neighboring peaks within the bounds
    model_profile = _library->meanProfile(event, _radius, _nframes);
  } catch (...) {
    return false;
  }

  PeakCoordinateSystem coord(peak);

  // evaluate the model profile at the given events
  for (int i = 0; i < events.size(); ++i) {
    Eigen::Vector3d x;
    if (_library->detectorCoords()) {
      x(0) = events[i]._px;
      x(1) = events[i]._py;
      x(2) = events[i]._frame;
      x -= peak->shape().center();
    } else {
      x = coord.transform(events[i]);
    }

    const double predict = model_profile.predict(x);

    // if (predict > 0.0001) {
    profile.push_back(predict);
    obs_counts.push_back(counts[i]);
    //}
  }

  // todo: stopping criterion
  for (auto i = 0; i < 20; ++i) {
    Intensity old_intensity = _integratedIntensity;
    const double I0 = _integratedIntensity.value();
    updateFit(_integratedIntensity, _meanBackground, profile, obs_counts);
    const double I1 = _integratedIntensity.value();

    if (std::isnan(I1) || std::isnan(_meanBackground.value())) {
      _integratedIntensity = old_intensity;
      break;
    }

    if (I1 < 0.0 || (I1 < (1 + tolerance) * I0 && I0 < (1 + tolerance) * I1)) {
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
