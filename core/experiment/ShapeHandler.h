//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
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

#include "core/shape/ShapeCollection.h"

namespace nsx {


class Peak3D;

using ShapeCollectionMap = std::map<std::string, std::unique_ptr<ShapeCollection>>;

class ShapeHandler {

 public:
    ShapeHandler() = default;
    ~ShapeHandler();

    //! Get a pointer to the map of shape collections
    const ShapeCollectionMap* getShapeCollectionMap() const;
    //! Add a shape collection
    bool addShapeCollection(const std::string& name, const nsx::ShapeCollection& shapes);
    //! Add an empty shape collection
    bool addEmptyCollection(const std::string& name);
    //! Returns true if the experiment has named shape collection
    bool hasShapeCollection(const std::string& name) const;
    //! Returns the named shape collection
    ShapeCollection* getShapeCollection(const std::string name);
    // !Remove a shape collection from the experiment
    void removeShapeCollection(const std::string& name);
    //! Get a vector of shape collection names from the handler
    std::vector<std::string> getCollectionNames() const;
    //! Get the number of shape collections
    int numShapeCollections() const { return _shape_collections.size(); };
    //! Generate name for new shape collection
    std::string generateName();
    //! Get a vector of pointers to shape collections
    std::vector<ShapeCollection*> getShapeCollections();

 private:
    //! Map of shape collections in Experiment
    ShapeCollectionMap _shape_collections;
};

} // namespace nsx

#endif // NSX_CORE_EXPERIMENT_SHAPEHANDLER_H
