//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/gonio/RotAxis.h
//! @brief     Defines classes RotAxis, Direction
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_GONIO_ROTAXIS_H
#define OHKL_CORE_GONIO_ROTAXIS_H

#include "core/gonio/Axis.h"

namespace ohkl {

//! A rotation axis in 3D space, constructed by a vector and a direction (CW or CCW).

//! The direction vector needs not be normalized.
//! TODO (#24): The direction flag (CW/CCW) is not needed, confusing, and should be removed.

class RotAxis : public Axis {
 public:
    //! Axis direction (clockwise, counter-clockwise)
    enum class Direction { CW, CCW };

    //! Static constructor for a RotAxis
    static Axis* create(const YAML::Node& node);
    RotAxis();

    //! Constructs an axis with a given name
    explicit RotAxis(const std::string& label);
    //! Explicit
    explicit RotAxis(
        const std::string& label, const Eigen::Vector3d& axis,
        Direction direction = Direction::CCW);
    //! Construct a RotAxis from a property tree node.
    RotAxis(const YAML::Node& node);

    RotAxis* clone() const override;
    //! Gets rotation direction.
    void setRotationDirection(Direction);
    //! Returns rotation direction: 0 for CCW and 1 for CW
    Direction rotationDirection() const;
    //! Gets the rotation matrix associated with this rotation
    //! @param angle : rotation angle in radians by default, use Units to convert
    //! @return rotation matrix
    Eigen::Matrix3d rotationMatrix(double angle) const;
    //! Returns the 4x4 Homogeous matrix corresponding to this transformation.
    Eigen::Transform<double, 3, Eigen::Affine> affineMatrix(double angle) const override;
    //! Gets the quaternion associated with this rotation
    //! @param angle : rotation angle in radians by default
    //! @return rotation matrix
    Eigen::Quaterniond quaternion(double angle) const;

    std::ostream& printSelf(std::ostream& os) const override;

 protected:
    //! Rotation direction
    Direction _dir;
};

} // namespace ohkl

#endif // OHKL_CORE_GONIO_ROTAXIS_H
