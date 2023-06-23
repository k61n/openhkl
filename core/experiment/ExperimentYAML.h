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

#include <yaml-cpp/yaml.h>

#include <string>

namespace ohkl {

struct DataReaderParameters;
struct IndexerParameters;
struct IntegrationParameters;
struct MergeParameters;
struct PeakFinderParameters;
struct PredictionParameters;
struct ShapeModelParameters;

class ExperimentYAML {
 public:
    ExperimentYAML(const std::string& filename);

    void grabDataReaderParameters(DataReaderParameters* params) const;
    void setDataReaderParameters(DataReaderParameters* params);
    void grabIntegrationParameters(IntegrationParameters* params);
    void setIntegrationParameters(IntegrationParameters* params);
    void grabPeakFinderParameters(PeakFinderParameters* params);
    void setPeakFinderParameters(PeakFinderParameters* params);
    void grabAutoindexerParameters(IndexerParameters* params);
    void setAutoindexerParameters(IndexerParameters* params);
    void grabShapeParameters(ShapeModelParameters* params);
    void setShapeParameters(ShapeModelParameters* params);
    void grabPredictorParameters(PredictionParameters* params);
    void setPredictorParameters(PredictionParameters* params);
    void grabMergeParameters(MergeParameters* params);
    void setMergeParameters(MergeParameters* params);

    void writeFile(const std::string& filename);

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
