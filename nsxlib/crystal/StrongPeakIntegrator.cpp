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

#include "Blob3D.h"
#include "DataSet.h"
#include "Ellipsoid.h"
#include "Intensity.h"
#include "MeanBackgroundIntegrator.h"
#include "Peak3D.h"
#include "StrongPeakIntegrator.h"

namespace nsx {

StrongPeakIntegrator::StrongPeakIntegrator(): MeanBackgroundIntegrator()
{
}

StrongPeakIntegrator::~StrongPeakIntegrator()
{
}

bool StrongPeakIntegrator::compute(sptrPeak3D peak, const IntegrationRegion& region)
{
    MeanBackgroundIntegrator::compute(peak, region);
    PeakCoordinateSystem frame(peak);

    const auto& events = region.data().events();
    const auto& counts = region.data().counts();

    double sum_peak = 0.0;
    const double mean_bkg = _meanBackground.value();
    const double std_bkg = _meanBackground.sigma();
    size_t npeak = 0.0;
    size_t nbkg = 0.0;
    Blob3D blob;

    // compute total peak intensity
    for (auto i = 0; i < counts.size(); ++i) {
        const auto& ev = events[i];
        auto ev_type = region.classify(ev);

        if (ev_type == IntegrationRegion::EventType::BACKGROUND) {
            nbkg++;
            continue;
        }

        if (ev_type == IntegrationRegion::EventType::PEAK) {
            sum_peak += counts[i];
            npeak++;

            // update blob if pixel is strong
            if (counts[i] > mean_bkg+3*std_bkg) {
                blob.addPoint(ev._px, ev._py, ev._frame, counts[i]-mean_bkg);
            }
        }
    }

    sum_peak -= npeak*_meanBackground.value();

    // TODO: ERROR ESTIMATE!!
    // This INCORRECTLY assumes Poisson statistics (no gain or baseline)
    _integratedIntensity = Intensity(sum_peak, sum_peak + npeak*npeak*std_bkg*std_bkg);

    // TODO: compute rocking curve
    double f_min = int(events[0]._frame);
    double f_max = f_min;


    for (size_t i = 0; i < counts.size(); ++i) {
        const auto& ev = events[i];
        f_min = std::min(ev._frame, f_min);
        f_max = std::max(ev._frame, f_max);
    }

    bool update_shape = false;


    if (!update_shape) {
        return true;
    }

    Eigen::Vector3d center = blob.center();
    Eigen::Matrix3d cov = blob.covariance();

    // center of mass is consistent
    if (!((center-peak->getShape().center()).norm() < 100)) {
        return false;
    }

    Eigen::Matrix3d A0 = peak->getShape().metric();
    Eigen::Matrix3d A1 = cov.inverse();

    // check that the covariance is consistent
    if (!((A1-A0).norm() / A0.norm() < 2.0)) {
        return false;
    }

    // shape is not too small or too large
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(cov);
    auto w = solver.eigenvalues();

    if (w.minCoeff() < 0.1 || w.maxCoeff() > 100) {
        return false;
    }

    peak->setShape(Ellipsoid(center, A1));

    size_t nframes = size_t(f_max-f_min)+1;
    _rockingCurve.resize(nframes);

    return true;
}

} // end namespace nsx
