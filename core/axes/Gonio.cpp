#include <algorithm>
#include <stdexcept>

#include <Eigen/Geometry>

#include "DirectVector.h"
#include "Gonio.h"
#include "RotAxis.h"
#include "Units.h"

namespace nsx {

Gonio::Gonio() : _name("goniometer") {}

Gonio::Gonio(const std::string &name) : _name(name) {}

Gonio::Gonio(const Gonio &other) : _name(other._name) {
  _axes.reserve(other._axes.size());
  for (auto &&ax : other._axes) {
    _axes.emplace_back(std::unique_ptr<Axis>(ax->clone()));
  }
}

Gonio::Gonio(const YAML::Node &node) {
  _name = node["name"] ? node["name"].as<std::string>() : "";

  // Set the axis of the detector goniometer from the XML node
  for (auto &&axisItem : node["axis"]) {
    _axes.emplace_back(std::unique_ptr<Axis>(Axis::create(axisItem)));
  }
}

Gonio &Gonio::operator=(const Gonio &other) {
  if (this != &other) {
    _name = other._name;
    _axes.reserve(other._axes.size());
    for (auto &&ax : other._axes) {
      _axes.emplace_back(std::unique_ptr<Axis>(ax->clone()));
    }
  }

  return *this;
}

Gonio::~Gonio() {}

size_t Gonio::nAxes() const { return _axes.size(); }

Axis &Gonio::axis(size_t index) {
  if (index >= _axes.size()) {
    throw std::runtime_error("Invalid axis id.");
  }

  return *_axes[index];
}

const Axis &Gonio::axis(size_t index) const {
  if (index >= _axes.size()) {
    throw std::runtime_error("Invalid axis id.");
  }

  return *_axes[index];
}

Eigen::Transform<double, 3, Eigen::Affine>
Gonio::affineMatrix(const std::vector<double> &state) const {
  if (static_cast<size_t>(state.size()) != _axes.size()) {
    throw std::range_error("Trying to set Gonio " + _name +
                           " with wrong number of parameters");
  }

  Eigen::Transform<double, 3, Eigen::Affine> result =
      Eigen::Transform<double, 3, Eigen::Affine>::Identity();
  std::vector<std::unique_ptr<Axis>>::const_reverse_iterator it;
  int axis = state.size() - 1;

  for (it = _axes.rbegin(); it != _axes.rend(); ++it) {
    result = (*it)->affineMatrix(state[axis]) * result;
    axis--;
  }

  return result;
}

DirectVector Gonio::transform(const DirectVector &v,
                              const std::vector<double> &state) const {
  Eigen::Transform<double, 3, Eigen::Affine> result = affineMatrix(state);
  const Eigen::Vector3d &d_vector = v.vector();
  return DirectVector((result * d_vector.homogeneous()));
}

} // end namespace nsx
