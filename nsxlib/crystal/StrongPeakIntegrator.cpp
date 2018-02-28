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
#include "StandardFrame.h"
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
    StandardFrame frame(peak);

    const auto& peakEvents = region.peakData().events();
    const auto& peakCounts = region.peakData().counts();

    double sum_peak = 0.0;

    const double npeak = peakEvents.size();
    const double nbkg = region.bkgData().events().size();

    const double mean_bkg = _meanBackground.value();
    const double std_bkg = _meanBackground.sigma();

    // compute total peak intensity
    for (auto count: peakCounts) {
        sum_peak += count;
    }
    sum_peak -= npeak*mean_bkg;

    // TODO: ERROR ESTIMATE!!
    // This INCORRECTLY assumes Poisson statistics (no gain or baseline)
    _integratedIntensity = Intensity(sum_peak, sum_peak + npeak*npeak*std_bkg*std_bkg / nbkg);

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

    // Get shape in standard coordinate system
    Blob3D detector_blob, q_blob, standard_blob;

    for (auto i = 0; i < Npeak; ++i) {
        const double value = peakCounts[i] - mean_bkg;

        // value too small to contribute
        if (value < 0.5 * std_bkg) {
            continue;
        }
        auto ev = peakEvents[i];
        auto q = region.peakData().qs()[i];
        Eigen::Vector3d coord = frame.transform(peakEvents[i]);
        detector_blob.addPoint(ev._px, ev._py, ev._frame, value);
        standard_blob.addPoint(coord(0), coord(1), coord(2), value);
        q_blob.addPoint(q[0], q[1], q[2], value);
    }

    Ellipsoid detector_ellipsoid(detector_blob.getCenterOfMass(), detector_blob.covariance().inverse());

    Eigen::Vector3d com = detector_ellipsoid.center();
    Eigen::Matrix3d cov = detector_ellipsoid.inverseMetric();

    Eigen::Matrix3d cov2 = peak->getShape().inverseMetric();

    Eigen::Vector3d dx = com - peak->getShape().center();

    double pearson = (cov.transpose()*cov2).trace() / cov.norm() / cov2.norm();

    //std::cout << dx.transpose() << "; " << pearson << std::endl;

    Eigen::Vector3d extents = detector_ellipsoid.aabb().extents();

    // testing...
    #if 0
    if (pearson > 0.9 && dx.norm() < 2.0 && extents.norm() < 100.0 && extents.minCoeff() > 1.0) {
        // peak is good, update shape
        peak->setShape(detector_ellipsoid);
    } else {
        std::cout << "peak integration failed" << std::endl;
        peak->setSelected(false);
        return false;
    }
    #endif

    return true;
}

} // end namespace nsx
