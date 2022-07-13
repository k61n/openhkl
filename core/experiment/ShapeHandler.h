//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/ShapeHandler.h
//! @brief     Handles shape collection manipulations for Experiment object
//! //! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_EXPERIMENT_SHAPEHANDLER_H
#define NSX_CORE_EXPERIMENT_SHAPEHANDLER_H

#include <map>
#include <string>
#include <vector>

#include "core/shape/ShapeModel.h"

namespace ohkl {


class Peak3D;

using ShapeModelMap = std::map<std::string, std::unique_ptr<ShapeModel>>;

class ShapeHandler {

 public:
    ShapeHandler() = default;
    ~ShapeHandler();

    //! Get a pointer to the map of shape models
    const ShapeModelMap* getShapeModelMap() const;
    //! Add a shape model
    bool addShapeModel(const std::string& name, const ohkl::ShapeModel& shapes);
    //! Add an empty shape model
    bool addEmptyModel(const std::string& name);
    //! Returns true if the experiment has named shape model
    bool hasShapeModel(const std::string& name) const;
    //! Returns the named shape model
    ShapeModel* getShapeModel(const std::string name);
    // !Remove a shape model
    void removeShapeModel(const std::string& name);
    //! Get a vector of shape model
    std::vector<std::string> getCollectionNames() const;
    //! Get the number of shape models
    int numShapeModels() const { return _shape_models.size(); };
    //! Generate name for new shape model
    std::string generateName();
    //! Get a vector of pointers to shape models
    std::vector<ShapeModel*> getShapeModels();

 private:
    //! Map of shape collections in Experiment
    ShapeModelMap _shape_models;
};

} // namespace ohkl

#endif // NSX_CORE_EXPERIMENT_SHAPEHANDLER_H
