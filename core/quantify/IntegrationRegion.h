//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/quantify/IntegrationRegion.h
//! @brief     Defines class IntegrationRegion
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_PEAK_INTEGRATIONREGION_H
#define CORE_PEAK_INTEGRATIONREGION_H

#include "core/hull/ConvexHull.h"
#include "core/quantify/PeakData.h"

namespace nsx {

//! Class used to construct a peak integration region.
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
    //! Returns the bounding box of the region
    const AABB aabb() const;
    //! Returns the bounding box of the peak region
    const AABB peakBB() const;
    //! Classify a detector event (peak, background, forbidden, etc.)
    EventType classify(const DetectorEvent& ev) const;
    //! Update the region with the next frame
    bool advanceFrame(const Eigen::MatrixXd& image, const Eigen::MatrixXi& mask, double frame);
    //! Reset the integration region (i.e. free memory)
    void reset();
    //! Returns the underlying data stored by the region
    const PeakData& data() const;
    //! Returns the data stored by the region
    PeakData& data();
    //! Returns the peak shape used by the region
    const Ellipsoid& shape() const;
    //! Returns the convex hull of the region (e.g. BrillouinZone)
    const ConvexHull& hull() const;
    //! Returns the scaling factor used to determine the peak boundary
    double peakEnd() const { return _peakEnd; }

private:
    Ellipsoid _shape;
    double _peakEnd;
    double _bkgBegin;
    double _bkgEnd;
    PeakData _data;
    ConvexHull _hull;
};

} // namespace nsx

#endif // CORE_PEAK_INTEGRATIONREGION_H
