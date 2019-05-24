#ifndef CORE_AXES_TRANSAXIS_H
#define CORE_AXES_TRANSAXIS_H

#include <yaml-cpp/yaml.h>

#include <Eigen/Geometry>

#include "Axis.h"

namespace nsx {

//! Translational axis of an abstract goniometer
class TransAxis : public Axis {
public:
    //! Static constructor for a TransAxis
    static Axis* create(const YAML::Node& node);

    //! Default constructor
    TransAxis();
    //! Copy constructor
    TransAxis(const TransAxis& other);
    //! Constructs a translation axis with a given label
    TransAxis(const std::string& label);
    //! Constructs a translation axis with a given label and axis
    TransAxis(const std::string& label, const Eigen::Vector3d& axis);
    //! Construct a TransAxis from a property tree node.
    TransAxis(const YAML::Node& node);
    // Destructor
    ~TransAxis() override;
    //! Virtual copy constructor
    TransAxis* clone() const;

    //! Assignment operator
    TransAxis& operator=(const TransAxis& other);

    Eigen::Transform<double, 3, Eigen::Affine> affineMatrix(double value) const override;

    std::ostream& printSelf(std::ostream& os) const override;
};

} // end namespace nsx

#endif // CORE_AXES_TRANSAXIS_H
