//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/gonio/Gonio.h
//! @brief     Defines class Gonio
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_GONIO_GONIO_H
#define OHKL_CORE_GONIO_GONIO_H

#include "base/geometry/DirectVector.h"
#include "core/gonio/Axis.h"

namespace ohkl {

//! A goniometer, i.e. system of several rotation axes.

//! Any number of axes can be used. The rotation is applied in reverse order of the given
//! order. For example, when defining a goniometer with 3 axes a,b,c, it is
//! supposed that in such collection of axes, b is attached to the a shaft and c
//! is attached to the b shaft. Such gonio will rotate a vector v into a.b.c.v.
//! Once the Gonio is constructed, it is not allowed to change the number of
//! axes. Axes, their labels and respective limits can be modified by the class
//! methods or by template accessor: e.g. Axis<0>(g)=RotAxis(UnitZ,CW).

class Gonio {
 public:
    Gonio();
    Gonio(const Gonio& other);

    //! Constructs a gonio with a given name
    Gonio(const std::string& name);

    //! Constructs a Gonio from a property tree node
    Gonio(const YAML::Node& node);
    ~Gonio();
    Gonio& operator=(const Gonio& other);

    //! Returns the number of axis of this goniometer
    size_t nAxes() const;

    //! Gets a pointer to axis with name, throw range_error if not found
    Axis& axis(size_t index);

    //! Gets a pointer to axis with name, throw range_error if not found
    const Axis& axis(size_t index) const;

    //! Returns the homogeneous matrix corresponding to this set of parameters.
    //! Throw if angles outside limits.
    Eigen::Transform<double, 3, Eigen::Affine> affineMatrix(const std::vector<double>& state) const;

    //! Transform a point in 3D space, given a vector of parameters
    DirectVector transform(const DirectVector& v, const std::vector<double>& state) const;

#ifndef SWIG
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
#endif

 private:
    //! Given name of the gonio
    std::string _name;
    //! Sets of axis
    std::vector<std::unique_ptr<Axis>> _axes;
};

} // namespace ohkl

#endif // OHKL_CORE_GONIO_GONIO_H
