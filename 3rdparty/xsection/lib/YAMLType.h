//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/utils/YAMLType.h
//! @brief     Defines templated functions for YAML import and export
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef XSECTION_LIB_YAMLTYPE_H
#define XSECTION_LIB_YAMLTYPE_H

#include <yaml-cpp/yaml.h>

namespace YAML {

template <typename T> struct convert<std::complex<T>> {

    static Node encode(const std::complex<T>& rhs)
    {
        Node node;
        node.push_back(rhs.real());
        node.push_back(rhs.imag());
        return node;
    }

    static bool decode(const Node& node, std::complex<T>& rhs)
    {
        if (!node.IsSequence() || node.size() != 2)
            return false;

        rhs.real(node[0].as<T>());
        rhs.imag(node[1].as<T>());
        return true;
    }
};

} // namespace YAML

#endif // XSECTION_LIB_YAMLTYPE_H
