//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/parser/ExperimentYAML.h
//! @brief     Declares function eigenToVector
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_EXPERIMENT_EXPERIMENTYAML_H
#define OHKL_CORE_EXPERIMENT_EXPERIMENTYAML_H

#include "base/utils/Logger.h"
#include "core/loader/IDataReader.h"

#include <yaml-cpp/yaml.h>

#include <optional>

namespace ohkl {

class IDataReader;

class ExperimentYAML {
 public:
    ExperimentYAML(const std::string& filename);

    void setDataReaderParameters(DataReaderParameters* params) const;

 private:
    YAML::Node _node;

    template <typename T>
    T getNode(const YAML::Node& node, const std::string& key) const
    {
        if (node[key])
            return node[key].as<T>();
        else {
            ohklLog(Level::Warning, "Warning: node \"", key, "\" not found in .yml file");
            return {};
        }
    }
};

} // namespace ohkl



#endif // OHKL_CORE_EXPERIMENT_EXPERIMENTYAML_H
