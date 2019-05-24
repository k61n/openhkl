#ifndef CORE_DETECTOR_CYLINDRICALDETECTOR_H
#define CORE_DETECTOR_CYLINDRICALDETECTOR_H

#include <yaml-cpp/yaml.h>

#include "Detector.h"
#include "GeometryTypes.h"

namespace nsx {

//! Class implementing cylindrical and banana detectors.
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
    //! Return a pointer to a copy of a CylindricalDetector
    Detector* clone() const override;
    //! Destructor
    virtual ~CylindricalDetector();

    //! Assignment operator
    CylindricalDetector& operator=(const CylindricalDetector& other) = default;

    //! Set the height of the detector (meters)
    void setHeight(double height) override;
    //! Set the width of the detector (meters)
    void setWidth(double width) override;

    //! Set the angular height of the detector (radians)
    void setAngularHeight(double angle) override;
    //! Set the angular width of the detector (radians)
    void setAngularWidth(double angle) override;

    //! Returns the position of a given pixel in detector space. This takes into
    //! account the detector motions in detector space.
    DirectVector pixelPosition(double px, double py) const override;

    //! Determine whether detector at rest can receive a scattering event with
    //! direction given by Kf. px and py are detector position if true.
    virtual DetectorEvent
    constructEvent(const DirectVector& from, const ReciprocalVector& kf) const override;

    Eigen::Matrix3d jacobian(double px, double py) const override;
};

} // end namespace nsx

#endif // CORE_DETECTOR_CYLINDRICALDETECTOR_H
