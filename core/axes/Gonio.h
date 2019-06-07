//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/axes/Gonio.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_AXES_GONIO_H
#define CORE_AXES_GONIO_H

#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <yaml-cpp/yaml.h>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include "Axis.h"
#include "GeometryTypes.h"

namespace nsx {

//! \brief Class Gonio.
//! Base class for all goniometers (system of several rotation axis). Any number
//! of axis can be used. The rotation is applied in reverse order of the given
//! order. For example, when defining a goniometer with 3 axes a,b,c, it is
//! supposed that in such collection of axes, b is attached to the a shaft and c
//! is attached to the b shaft. Such gonio will rotate a vector v into a.b.c.v.
//! Once the Gonio is constructed, it is not allowed to change the number of
//! axes. Axes, their labels and respective limits can be modified by the class
//! methods or by template accessor: e.g. Axis<0>(g)=RotAxis(UnitZ,CW).
class Gonio {

public:
    //! Default constructor
    Gonio();

    //! Copy constructor
    Gonio(const Gonio& other);

    //! Constructs a gonio with a given name
    Gonio(const std::string& name);

    //! Constructs a Gonio from a property tree node
    Gonio(const YAML::Node& node);

    //! Destructor
    ~Gonio();

    //! Assignment operator
    Gonio& operator=(const Gonio& other);

    //! Return the number of axis of this goniometer
    size_t nAxes() const;

    //! Get a pointer to axis with name, throw range_error if not found
    Axis& axis(size_t index);

    //! Get a pointer to axis with name, throw range_error if not found
    const Axis& axis(size_t index) const;

    //! Return the homogeneous matrix corresponding to this set of parameters.
    //! Throw if angles outside limits.
    Eigen::Transform<double, 3, Eigen::Affine>
    affineMatrix(const std::vector<double>& values) const;

    //! Transform a point in 3D space, given a vector of parameters
    DirectVector transform(const DirectVector& v, const std::vector<double>& state) const;

#ifndef SWIG
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
#endif

private:
    //! Given name of the gonio
    std::string _name;
    //! Set of axis
    std::vector<std::unique_ptr<Axis>> _axes;
};

} // end namespace nsx

#endif // CORE_AXES_GONIO_H
