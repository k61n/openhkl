//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/detector/FlatDetector.h
//! @brief     Defines class FlatDetector
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_DETECTOR_FLATDETECTOR_H
#define NSX_CORE_DETECTOR_FLATDETECTOR_H

#include "core/detector/Detector.h"

namespace nsx {

//! A flat detector.

class FlatDetector : public Detector {
 public:
    //! Static constructor of a FlatDetector from a property tree node
    static Detector* create(const YAML::Node& node);

    //! Construct a FlatDetector
    FlatDetector() = default;
    //! Construct a FlatDetector from another one
    FlatDetector(const FlatDetector& other) = default;
    //! Construct a FlatDetector with a given name
    FlatDetector(const std::string& name);
    //! Constructs a FlatDetector from a property tree node
    FlatDetector(const YAML::Node& node);
    //! Returns a pointer to a copy of a FlatDetector
    Detector* clone() const override;

    FlatDetector& operator=(const FlatDetector& other) = default;

    //! Sets the height of the detector (meters)
    virtual void setHeight(double height) override;
    //! Sets the width of the detector (meters)
    virtual void setWidth(double width) override;

    //! Sets the angular height of the detector (radians)
    virtual void setAngularHeight(double angle) override;
    //! Sets the angular width of the detector (radians)
    virtual void setAngularWidth(double angle) override;

    //! Returns the position of a given pixel in detector space. This takes into
    //! account the detector motions in detector space.
    virtual DirectVector pixelPosition(double px, double py) const override;

    //! Determine whether detector at rest can receive a scattering event with
    //! direction given by Kf. px and py are detector position if true.
    DetectorEvent constructEvent(
        const DirectVector& from, const ReciprocalVector& kf, const double frame) const override;

    virtual Eigen::Matrix3d jacobian(double x, double y) const override;
};

} // namespace nsx

#endif // NSX_CORE_DETECTOR_FLATDETECTOR_H
