#ifndef YAMPTYPE_H_
#define YAMPTYPE_H_

#include <complex>

#include "yaml-cpp/yaml.h"

namespace YAML {

template<typename T>
struct convert<std::complex<T>> {
  static Node encode(const std::complex<T>& rhs) {
    Node node;
    node.push_back(rhs.real);
    node.push_back(rhs.imag);
    return node;
  }

  static bool decode(const Node& node, std::complex<T>& rhs) {

	  if(!node.IsSequence() || node.size() != 2) {
      return false;
    }

    rhs.real(node[0].as<T>());
    rhs.imag(node[1].as<T>());
    return true;
  }
};
}

#endif /* YAMPTYPE_H_ */
