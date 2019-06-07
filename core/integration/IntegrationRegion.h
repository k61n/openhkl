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

#pragma once

#include <Eigen/Core>

#include "ConvexHull.h"
#include "Ellipsoid.h"
#include "GeometryTypes.h"
#include "PeakData.h"

namespace nsx {

//! \brief Class used to construct a peak integration region.
class IntegrationRegion {
public:
    //! Classify the type of detector event with respect to the current
    //! integration region
    enum class EventType : int { BACKGROUND = 1, PEAK = 2, FORBIDDEN = -1, EXCLUDED = 0 };

    IntegrationRegion();

    IntegrationRegion(IntegrationRegion&& other) = default;

    IntegrationRegion(sptrPeak3D peak, double peak_end, double bkg_begin, double bkg_end);

    //! Update the integration mask of the detector (peak, background, forbiddgen
    //! zones)
    void updateMask(Eigen::MatrixXi& mask, double frame) const;
    //! Return the bounding box of the region
    const AABB aabb() const;
    //! Return the bounding box of the peak region
    const AABB peakBB() const;
    //! Classify a detector event (peak, background, forbidden, etc.)
    EventType classify(const DetectorEvent& ev) const;
    //! Update the region with the next frame
    bool advanceFrame(const Eigen::MatrixXd& image, const Eigen::MatrixXi& mask, double frame);
    //! Reset the integration region (i.e. free memory)
    void reset();
    //! Return the underlying data stored by the region
    const PeakData& data() const;
    //! Return the data stored by the region
    PeakData& data();
    //! Return the peak shape used by the region
    const Ellipsoid& shape() const;
    //! Return the convex hull of the region (e.g. BrillouinZone)
    const ConvexHull& hull() const;
    //! Return the scaling factor used to determine the peak boundary
    double peakEnd() const { return _peakEnd; }

private:
    Ellipsoid _shape;
    double _peakEnd;
    double _bkgBegin;
    double _bkgEnd;
    PeakData _data;
    ConvexHull _hull;
};

} // end namespace nsx
