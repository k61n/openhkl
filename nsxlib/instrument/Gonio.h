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

#ifndef SX_GONIO_H_
#define SX_GONIO_H_

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <utility>

#include <boost/property_tree/ptree.hpp>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include "Axis.h"
#include "RotAxis.h"

namespace nsx {

namespace proptree=boost::property_tree;

using Eigen::Vector3d;
/* !
 * \brief Class Gonio.
 * Base class for all goniometers (system of several rotation axis). Any number of axis can be used.
 * The rotation is applied in reverse order of the given order. For example, when defining a goniometer with
 * 3 axes a,b,c, it is supposed that in such collection of axes, b is attached to the a shaft and c is attached to the
 * b shaft. Such gonio will rotate a vector v into a.b.c.v.
 * Once the Gonio is constructed, it is not allowed to change the number of axes.
 * Axes, their labels and respective limits can be modified by the class methods or by template accessor:
 * e.g. Axis<0>(g)=RotAxis(UnitZ,CW).
 */
class Gonio
{
public:
    // Default constructor
    Gonio();
    //! Copy constructor
    Gonio(const Gonio& other);
    //! Constructs a gonio with a given name
    Gonio(const std::string& name);
    //! Constructs a Gonio from a property tree node
    Gonio(const proptree::ptree& node);
    //! Destructor
    ~Gonio();

    //! Assignment operator
    Gonio& operator=(const Gonio& other);

    // Getters and setters
    //! Gets the axes of this goniometer
    const std::vector<Axis*>& getAxes() const;
    //! Gets the names of the axes of this goniometer
    std::vector<std::string> getAxesNames() const;
    //! Gets the ids of the physical axes of this goniometer
    std::vector<unsigned int> getPhysicalAxesIds() const;
    //! Gets the names of the physical axes of this goniometer
    std::vector<std::string> getPhysicalAxesNames() const;
    //! Gets the names of the physical axes of this goniometer
    std::map<unsigned int,std::string> getPhysicalAxisIdToNames() const;
    //! Has physical axis with this name
    bool hasPhysicalAxis(const std::string&) const;
    //! Has axis with this name
    bool hasAxis(const std::string&) const;
    //! Get a pointer to axis i, throw range_error if not found
    Axis*  getAxis(unsigned int i);
    //! Get a pointer to axis with id id, throw range_error if not found
    Axis* getAxisFromId(unsigned int id);
    //! Get a pointer to axis with label, throw range_error if not found
    Axis* getAxis(const std::string& label);
    //! Return the homogeneous matrix corresponding to this set of parameters. Throw if angles outside limits.
    Eigen::Transform<double,3,Eigen::Affine> getHomMatrix(const std::vector<double>& values=std::vector<double>()) const;
    //! Return the number of axes attached to this goniometer
    std::size_t getNAxes() const;
    //! Return the number of physical axis defined in the gonio
    std::size_t getNPhysicalAxes() const;

    //! Add an Axis to this Goniometer.
    void addAxis(Axis* axis);
    //! Add a rotation axis to this goniometer
    Axis* addRotation(const std::string& label,const Vector3d& axis, RotAxis::Direction dir=RotAxis::Direction::CCW);
    //! Add a translation axis to this goniometer
    Axis* addTranslation(const std::string& label,const Vector3d& axis);
    //! Return the inverse of the homogeneous matrix corresponding to this set of parameters. Throw if angles outside limits.
    Eigen::Transform<double,3,Eigen::Affine> getInverseHomMatrix(const std::vector<double>& values=std::vector<double>()) const;
    //! Transform a point in 3D space, given a vector of parameters
    Vector3d transform(const Vector3d& v,const std::vector<double>& values=std::vector<double>());
    //! Reverse transform a point in 3D space, given a vector of parameters
    Vector3d transformInverse(const Vector3d& v,const std::vector<double>& values=std::vector<double>());
    //! Transform a vector inplace, for a values of Gonio parameters
    void transformInPlace(Vector3d& v,const std::vector<double>& values=std::vector<double>());
    //! Reverse transform a vector inplace, for a values of Gonio parameters
    void transformInverseInPlace(Vector3d& v,const std::vector<double>& values=std::vector<double>());
    //! Reset all offsets
    void resetOffsets();

protected:
    //! Check whether axis i within the range of Axis
    void isAxisValid(unsigned int i) const;
    //! Check whether s names a valid axis
    unsigned int isAxisValid(const std::string& s) const;
    //! Check whether id matches one of the axis id
    unsigned int isAxisIdValid(unsigned int id) const;

    //! Given name of the gonio
    std::string _label;
    //! Set of axis
    std::vector<Axis*> _axes;

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

} // end namespace nsx

#endif /* SX_GONIO_H_ */
