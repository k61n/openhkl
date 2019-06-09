//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/gonio/Axis.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_AXES_AXIS_H
#define CORE_AXES_AXIS_H

#include <iostream>
#include <string>

#include <yaml-cpp/yaml.h>

#include <Eigen/Dense>
#include <Eigen/Geometry>

namespace nsx {

/** @brief Interface use for homogeneous transform, Rotation and translation,
 * scale
 *
 * Each axis define a normalized direction (about which one rotates or along
 * which one translates). An offset can be defined and a valid range of
 * parameters (lowLimit, HighLimit). The Axis class does not maintain a given
 * state and only return the homogeneous matrix for a input value, i.e. for a
 * rotation of M_PI about the axis, the method getHomMatrix(M_PI) will return
 * the hom. Matrix.
 */
class Axis {

public:
    //! Static constructor for an Axis
    static Axis* create(const YAML::Node& node);
    Axis();
    Axis(const Axis& other);

    //! Contruct a default z-axis
    Axis(const std::string& name);

    //! Construct an axis from a vector
    Axis(const std::string& name, const Eigen::Vector3d& axis);

    //! Construct an Axis from a property tree node.
    Axis(const YAML::Node& node);
    Axis& operator=(const Axis& other);
    virtual ~Axis() = 0;
    virtual Axis* clone() const = 0;

    //! Give a name to this axis
    void setName(const std::string& name);

    //! Returns the axis name
    const std::string& name() const;

    //! Sets and normalize the axis direction
    void setAxis(const Eigen::Vector3d& axis);

    //! Gets the normalized direction of this axis
    const Eigen::Vector3d& axis() const;

    //! Gets the instrument id of the axis
    unsigned int id() const;

    //! Sets the instrument id of the axis
    void setId(unsigned int id);

    //! Gets the homogeneous (4x4) matrix corresponding to the value
    virtual Eigen::Transform<double, 3, Eigen::Affine> affineMatrix(double value) const = 0;

    //! Transform vector
    Eigen::Vector3d transform(const Eigen::Vector3d& v, double value);

    //! Sets the axis to physical (true) or virtual (true)
    void setPhysical(bool physical);

    //! Returns whether or not the axis is physical or not
    bool physical() const;

    virtual std::ostream& printSelf(std::ostream& os) const = 0;

protected:
    //! Label of the axis.
    std::string _name;
    //! Axis direction, a normalized vector.
    Eigen::Vector3d _axis;
    //! Defines whether the axis is physical or not. A physical axis is related to
    //! metadata.
    bool _physical;
    //! The instrument id (e.g. MAD number for instrument related to ILL ASCII
    //! Data).
    unsigned int _id;
};

std::ostream& operator<<(std::ostream& os, const Axis& axis);

} // end namespace nsx

#endif // CORE_AXES_AXIS_H
