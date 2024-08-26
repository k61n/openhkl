//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/ShapeHandler.h
//! @brief     Handles shape collection manipulations for Experiment object
//! //! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_EXPERIMENT_SHAPEHANDLER_H
#define OHKL_CORE_EXPERIMENT_SHAPEHANDLER_H

#include "core/data/DataTypes.h"

#include <map>

namespace ohkl {


class ShapeModel;

using ShapeModelMap = std::map<std::string, std::unique_ptr<ShapeModel>>;

class ShapeHandler {

 public:
    ShapeHandler() = default;
    ~ShapeHandler();

    //! Get a pointer to the map of shape models
    const ShapeModelMap* getShapeModelMap() const;
    //! Add a shape model
    bool addShapeModel(const std::string& name, const ohkl::ShapeModel& shapes);
    //! Add a shape model from a unique_ptr
    bool addShapeModel(const std::string& name, std::unique_ptr<ohkl::ShapeModel>& shape);
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
    //! Get a vector of pointers to shape models associated with the given DataSet
    std::vector<ShapeModel*> getShapeModels(sptrDataSet data);

 private:
    //! Map of shape collections in Experiment
    ShapeModelMap _shape_models;

    unsigned int _last_index = 0;
};

} // namespace ohkl

#endif // OHKL_CORE_EXPERIMENT_SHAPEHANDLER_H
