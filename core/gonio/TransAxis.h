//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/gonio/TransAxis.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_GONIO_TRANSAXIS_H
#define CORE_GONIO_TRANSAXIS_H

#include <yaml-cpp/yaml.h>

#include <Eigen/Geometry>

#include "core/gonio/Axis.h"

namespace nsx {

//! Translational axis of an abstract goniometer
class TransAxis : public Axis {
public:
    //! Static constructor for a TransAxis
    static Axis* create(const YAML::Node& node);
    TransAxis();
    TransAxis(const TransAxis& other);
    //! Constructs a translation axis with a given label
    TransAxis(const std::string& label);
    //! Constructs a translation axis with a given label and axis
    TransAxis(const std::string& label, const Eigen::Vector3d& axis);
    //! Construct a TransAxis from a property tree node.
    TransAxis(const YAML::Node& node);
    // Destructor
    ~TransAxis() override;
    TransAxis* clone() const override;
    TransAxis& operator=(const TransAxis& other);

    Eigen::Transform<double, 3, Eigen::Affine> affineMatrix(double value) const override;

    std::ostream& printSelf(std::ostream& os) const override;
};

} // end namespace nsx

#endif // CORE_GONIO_TRANSAXIS_H
