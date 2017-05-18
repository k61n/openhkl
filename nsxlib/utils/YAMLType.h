#ifndef NSXLIB_YAMLTYPE_H
#define NSXLIB_YAMLTYPE_H

#include <complex>

#include "yaml-cpp/yaml.h"

namespace YAML {

template<typename T>
struct convert<std::complex<T>> {

    static Node encode(const std::complex<T>& rhs) {
        Node node;
        node.push_back(rhs.real());
        node.push_back(rhs.imag());
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

} // end namespace YAML

#endif // NSXLIB_YAMLTYPE_H
