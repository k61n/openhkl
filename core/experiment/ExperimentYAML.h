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

#include <yaml-cpp/node/node.h>
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


/*! \addtogroup python_api
 *  @{*/

/*! \brief Read and write yml file containing experiment parameters
 *
 *  Implements yaml parser for yml file containing experiment parameters (NOT)
 *  data or reduced data). Mainly for GUI purposes, but may have other uses.
 */
class ExperimentYAML {
 public:
    ExperimentYAML(const std::string& filename);
    ExperimentYAML(const YAML::Node& node);

    //! Get parameters from data reader
    void grabDataReaderParameters(DataReaderParameters* params) const;
    //! Set data reader parameters
    void setDataReaderParameters(DataReaderParameters* params);
    //! Get parameters from integrator
    void grabIntegrationParameters(IntegrationParameters* params);
    //! Set integrator parameters
    void setIntegrationParameters(IntegrationParameters* params);
    //! Get parameters from integrator
    void grabPeakFinderParameters(PeakFinderParameters* params);
    //! Set peak finder parameters
    void setPeakFinderParameters(PeakFinderParameters* params);
    //! Get parameters from indexer
    void grabAutoindexerParameters(IndexerParameters* params);
    //! Set indexer parameters
    void setAutoindexerParameters(IndexerParameters* params);
    //! Get parameters from shape model
    void grabShapeParameters(ShapeModelParameters* params);
    //! Set shape model parameters
    void setShapeParameters(ShapeModelParameters* params);
    //! Get parameters from predictor
    void grabPredictorParameters(PredictionParameters* params);
    //! Set shape model parameters
    void setPredictorParameters(PredictionParameters* params);
    //! Get parameters from merger
    void grabMergeParameters(MergeParameters* params);
    //! Set merger parameters
    void setMergeParameters(MergeParameters* params);

    //! Write the yml file
    void writeFile(const std::string& filename);

 private:
    //! The root yml node
    YAML::Node _node;

    //! Read an arbitrary type node, check whether it exists
    template <typename T> T getNode(const YAML::Node& node, const std::string& key) const
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
