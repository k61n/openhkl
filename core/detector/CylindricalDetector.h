//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/detector/CylindricalDetector.h
//! @brief     Defines class CylindricalDetector
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_DETECTOR_CYLINDRICALDETECTOR_H
#define NSX_CORE_DETECTOR_CYLINDRICALDETECTOR_H

#include "core/detector/Detector.h"

namespace nsx {

//! A cylindrical or banana detector.

class CylindricalDetector : public Detector {
 public:
    //! Static constructor of a CylindricalDetector from a property tree node
    static Detector* create(const YAML::Node& node);

    //! Construct a CylindricalDetector
    CylindricalDetector() = default;
    //! Construct a CylindricalDetector from another one
    CylindricalDetector(const CylindricalDetector& other) = default;
    //! Constructs a CylindricalDetector with a given name
    CylindricalDetector(const std::string& name);
    //! Constructs a CylindricalDetector from a property tree node
    CylindricalDetector(const YAML::Node& node);
    //! Returns a pointer to a copy of a CylindricalDetector
    Detector* clone() const override;

    CylindricalDetector& operator=(const CylindricalDetector& other) = default;

    //! Sets the height of the detector (meters)
    void setHeight(double height) override;
    //! Sets the width of the detector (meters)
    void setWidth(double width) override;

    //! Sets the angular height of the detector (radians)
    void setAngularHeight(double angle) override;
    //! Sets the angular width of the detector (radians)
    void setAngularWidth(double angle) override;

    //! Returns the position of a given pixel in detector space. This takes into
    //! account the detector motions in detector space.
    DirectVector pixelPosition(double px, double py) const override;

    //! Determine whether detector at rest can receive a scattering event with
    //! direction given by Kf. px and py are detector position if true.
    DetectorEvent constructEvent(
        const DirectVector& from, const ReciprocalVector& kf, const double frame) const override;

    Eigen::Matrix3d jacobian(double px, double py) const override;
};

} // namespace nsx

#endif // NSX_CORE_DETECTOR_CYLINDRICALDETECTOR_H
