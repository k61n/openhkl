//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/peak/IntegrationRegion.h
//! @brief     Defines class IntegrationRegion
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_PEAK_INTEGRATIONREGION_H
#define OHKL_CORE_PEAK_INTEGRATIONREGION_H

#include "base/geometry/ConvexHull.h"
#include "base/geometry/Ellipsoid.h"
#include "core/peak/PeakData.h"
#include "core/peak/RegionData.h"

#include <map>

namespace ohkl {

enum class RegionType { VariableEllipsoid, FixedEllipsoid, Count };

const std::map<RegionType, std::string> regionTypeDescription{
    {RegionType::VariableEllipsoid, "Variable ellipsoid"},
    {RegionType::FixedEllipsoid, "Fixed ellipsoid"}};

//! Used to construct a peak integration region.

class IntegrationRegion {
 public:
    //! Classify the type of detector event with respect to the current integration region
    enum class EventType : int { BACKGROUND = 1, PEAK = 2, FORBIDDEN = -1, EXCLUDED = 0 };

    IntegrationRegion() = delete;
    IntegrationRegion(const IntegrationRegion& region) = delete;
    IntegrationRegion(IntegrationRegion&& region) = default;

    IntegrationRegion(
        Peak3D* peak, double peak_end, double bkg_begin, double bkg_end,
        RegionType region_type = RegionType::VariableEllipsoid);

    //! Update the integration mask of the detector (peak, background, forbiddgen
    //! zones)
    void updateMask(Eigen::MatrixXi& mask, double z) const;
    //! Get matrices containing the integration region
    RegionData* getRegion();
    //! Returns the bounding box of the region
    const AABB& aabb() const;
    //! Returns the bounding box of the peak region
    AABB peakBB() const;
    //! Classify a detector event (peak, background, forbidden, etc.)
    EventType classify(const DetectorEvent& ev) const;
    //! Update the region with the next frame
    bool advanceFrame(
        const Eigen::MatrixXd& image, const Eigen::MatrixXi& mask, double frame,
        const Eigen::MatrixXd* gradient = nullptr);
    //! Update the region with the next frame (thread safe)
    PeakData threadSafeAdvanceFrame(
        const Eigen::MatrixXd& image, const Eigen::MatrixXi& mask, double frame,
        const Eigen::MatrixXd* gradient = nullptr);
    //! Reset the integration region (i.e. free memory)
    void reset();
    //! Returns the underlying data stored by the region
    const PeakData& peakData() const;
    //! Returns the data stored by the region
    PeakData& peakData();
    //! Append PeakData to this instance
    void appendPeakData(const PeakData& peak_data);
    //! Returns the peak shape used by the region
    const Ellipsoid& shape() const;
    //! Returns the convex hull of the region (e.g. BrillouinZone)
    const ConvexHull& hull() const;
    //! Returns the scaling factor used to determine the peak boundary
    double peakEnd() const { return _peakEnd; }
    //! Return a pointer to the peak
    Peak3D* peak() const { return _peak; };
    //! Returns whether the integration region is valid
    bool isValid() const;

 private:
    Ellipsoid _shape;
    double _peakEnd;
    double _bkgBegin;
    double _bkgEnd;
    PeakData _data;
    ConvexHull _hull;
    bool _fixed;
    RegionType _regionType;
    Peak3D* _peak;
    RegionData _region_data;
    bool _valid;
};

} // namespace ohkl

#endif // OHKL_CORE_PEAK_INTEGRATIONREGION_H
