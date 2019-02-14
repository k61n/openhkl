#pragma once

#include <complex>

#include <Eigen/Dense>

#include <yaml-cpp/yaml.h>

namespace YAML {

template <typename T> struct convert<std::complex<T>> {

  static Node encode(const std::complex<T> &rhs) {
    Node node;
    node.push_back(rhs.real());
    node.push_back(rhs.imag());
    return node;
  }

  static bool decode(const Node &node, std::complex<T> &rhs) {
    if (!node.IsSequence() || node.size() != 2) {
      return false;
    }

    rhs.real(node[0].as<T>());
    rhs.imag(node[1].as<T>());
    return true;
  }
};

template <> struct convert<Eigen::Vector3d> {

  static Node encode(const Eigen::Vector3d &rhs) {
    Node node;
    node.push_back(rhs[0]);
    node.push_back(rhs[1]);
    node.push_back(rhs[2]);
    return node;
  }

  static bool decode(const Node &node, Eigen::Vector3d &rhs) {
    if (!node.IsSequence() || node.size() != 3) {
      return false;
    }

    rhs[0] = node[0].as<double>();
    rhs[1] = node[1].as<double>();
    rhs[2] = node[2].as<double>();
    return true;
  }
};

template <> struct convert<Eigen::Vector2d> {

  static Node encode(const Eigen::Vector2d &rhs) {
    Node node;
    node.push_back(rhs[0]);
    node.push_back(rhs[1]);
    return node;
  }

  static bool decode(const Node &node, Eigen::Vector2d &rhs) {
    if (!node.IsSequence() || node.size() != 2) {
      return false;
    }

    rhs[0] = node[0].as<double>();
    rhs[1] = node[1].as<double>();
    return true;
  }
};

} // end namespace YAML
