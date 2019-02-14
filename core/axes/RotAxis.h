/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
        BP 156
        6, rue Jules Horowitz
        38042 Grenoble Cedex 9
        France
        chapon[at]ill.fr
    pellegrini[at]ill.fr

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
        Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
 USA
 *
 */

#pragma once

#include <yaml-cpp/yaml.h>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include "Axis.h"

namespace nsx {

//! \brief Class defining a rotation axis.
//!
//! Allows to define a rotation axis in 3D space , constructed by a direction
//! vector and a rotation direction (CW or CCW). The direction vector needs not
//! be normalized.
class RotAxis : public Axis {
public:
  //! Axis direction (clockwise, counter-clockwise)
  enum class Direction { CW, CCW };

  //! Static constructor for a RotAxis
  static Axis *create(const YAML::Node &node);

  //! Default constructor
  RotAxis();
  //! Copy constructor
  RotAxis(const RotAxis &other);
  //! Constructs an axis with a given name
  explicit RotAxis(const std::string &label);
  //! Explicit
  explicit RotAxis(const std::string &label, const Eigen::Vector3d &axis,
                   Direction direction = Direction::CCW);
  //! Construct a RotAxis from a property tree node.
  RotAxis(const YAML::Node &node);
  //! Assignment operator
  RotAxis &operator=(const RotAxis &other);
  //! Destructor
  ~RotAxis();
  //! Virtual copy constructor
  RotAxis *clone() const;
  //! Get rotation direction.
  void setRotationDirection(Direction);
  //! Return rotation direction: 0 for CCW and 1 for CW
  Direction rotationDirection() const;
  //! Get the rotation matrix associated with this rotation
  //@param angle : rotation angle in radians by default, use Units to convert
  //@return rotation matrix
  Eigen::Matrix3d rotationMatrix(double angle) const;
  //! Return the 4x4 Homogeous matrix corresponding to this transformation.
  Eigen::Transform<double, 3, Eigen::Affine> affineMatrix(double angle) const;
  //! Get the quaternion associated with this rotation
  //@param angle : rotation angle in radians by default
  //@return rotation matrix
  Eigen::Quaterniond quaternion(double angle) const;

  std::ostream &printSelf(std::ostream &os) const override;

protected:
  //! Rotation direction
  Direction _dir;
};

} // end namespace nsx
