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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>
#include <utility>

#include <yaml-cpp/yaml.h>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include "Axis.h"
#include "GeometryTypes.h"
#include "InstrumentTypes.h"
#include "RotAxis.h"

namespace nsx {

//! \brief Class Gonio.
//! Base class for all goniometers (system of several rotation axis). Any number of axis can be used.
//! The rotation is applied in reverse order of the given order. For example, when defining a goniometer with
//! 3 axes a,b,c, it is supposed that in such collection of axes, b is attached to the a shaft and c is attached to the
//! b shaft. Such gonio will rotate a vector v into a.b.c.v.
//! Once the Gonio is constructed, it is not allowed to change the number of axes.
//! Axes, their labels and respective limits can be modified by the class methods or by template accessor:
//! e.g. Axis<0>(g)=RotAxis(UnitZ,CW).
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
    //! Gets the axes of this goniometer
    const std::vector<Axis*>& axes() const;
    //! Get a pointer to axis with name, throw range_error if not found
    Axis* axis(const std::string& name);
    //! Return the homogeneous matrix corresponding to this set of parameters. Throw if angles outside limits.
    Eigen::Transform<double,3,Eigen::Affine> affineMatrix(const std::vector<double>& values) const;
    //! Add an Axis to this Goniometer.
    void addAxis(Axis* axis);
    //! Add a rotation axis to this goniometer
    Axis* addRotation(const std::string& name,const Eigen::Vector3d& axis, RotAxis::Direction dir=RotAxis::Direction::CCW);
    //! Add a translation axis to this goniometer
    Axis* addTranslation(const std::string& name,const Eigen::Vector3d& axis);
    //! Transform a point in 3D space, given a vector of parameters
    DirectVector transform(const DirectVector& v, const std::vector<double>& state) const;

#ifndef SWIG
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
#endif

private:

    //! Given name of the gonio
    std::string _name;
    //! Set of axis
    std::vector<Axis*> _axes;
};

} // end namespace nsx
