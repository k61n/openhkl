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

#pragma once

#include <Eigen/Core>

#include "ConvexHull.h"
#include "Ellipsoid.h"
#include "GeometryTypes.h"
#include "PeakData.h"

namespace nsx {

class IntegrationRegion {
public:
    enum class EventType: int {BACKGROUND = 1, PEAK = 2, FORBIDDEN = 0, EXCLUDED = -1};

    IntegrationRegion();

    IntegrationRegion(IntegrationRegion&& other) = default;

    IntegrationRegion(sptrPeak3D peak, double peak_end, double bkg_begin, double bkg_end);

    void updateMask(Eigen::MatrixXi& mask, double frame) const;

    const AABB aabb() const;

    EventType classify(const DetectorEvent& ev) const;

    bool advanceFrame(const Eigen::MatrixXi& image, const Eigen::MatrixXi& mask, double frame);

    void reset();

    const PeakData& peakData() const;
    const PeakData& bkgData() const;
    const PeakData& data() const;
    PeakData& data();

    PeakData& peakData();
    PeakData& bkgData();

    const Ellipsoid& shape() const;

    const ConvexHull& hull() const;
private:
    Ellipsoid _shape;
    double _peakEnd;
    double _bkgBegin;
    double _bkgEnd;
    PeakData _peakData;
    PeakData _bkgData;
    PeakData _data;
    ConvexHull _hull;
};

} // end namespace nsx
