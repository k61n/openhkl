//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/ShapeHandler.cpp
//! @brief     Handles peak manipulations for Experiment object
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/experiment/ShapeHandler.h"

#include "base/utils/Logger.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/ShapeModel.h"

namespace ohkl {

ShapeHandler::~ShapeHandler() = default;

const ShapeModelMap* ShapeHandler::getShapeModelMap() const
{
    return &_shape_models;
}

bool ShapeHandler::addShapeModel(const std::string& name, const ohkl::ShapeModel& shapes)
{
    if (hasShapeModel(name))
        return false;
    ohklLog(
        Level::Info, "ShapeHandler::addShapeModel '", name, "': ", shapes.numberOfPeaks(),
        " shapes");
    std::unique_ptr<ShapeModel> ptr = std::make_unique<ShapeModel>(shapes);
    ptr->setName(name);
    ptr->setId(_last_index++);
    _shape_models.insert_or_assign(name, std::move(ptr));
    return hasShapeModel(name); // now name must be in use
}

bool ShapeHandler::addShapeModel(const std::string& name, std::unique_ptr<ohkl::ShapeModel>& shapes)
{
    if (hasShapeModel(name))
        return false;
    ohklLog(
        Level::Info, "ShapeHandler::addShapeModel '", name, "': ", shapes->numberOfPeaks(),
        " shapes");
    shapes->setName(name);
    shapes->setId(_last_index++);
    _shape_models.insert_or_assign(name, std::move(shapes));
    return hasShapeModel(name); // now name must be in use
}

bool ShapeHandler::addEmptyModel(const std::string& name)
{
    if (hasShapeModel(name))
        return false;
    ohklLog(Level::Info, "ShapeHandler::addEmptyCollection '" + name + "'");
    std::unique_ptr<ShapeModel> ptr = std::make_unique<ShapeModel>(ShapeModel(name));
    ptr->setId(_last_index++);
    _shape_models.insert_or_assign(name, std::move(ptr));
    return hasShapeModel(name); // now name must be in use
}

bool ShapeHandler::hasShapeModel(const std::string& name) const
{
    auto shapes = _shape_models.find(name);
    return (shapes != _shape_models.end());
}

ShapeModel* ShapeHandler::getShapeModel(const std::string name)
{
    if (hasShapeModel(name)) {
        return _shape_models[name].get();
    }
    return nullptr;
}

void ShapeHandler::removeShapeModel(const std::string& name)
{
    if (hasShapeModel(name)) {
        auto shapes = _shape_models.find(name);
        shapes->second.reset();
        _shape_models.erase(shapes);
    }
}

std::vector<std::string> ShapeHandler::getCollectionNames() const
{

    std::vector<std::string> names;
    for (ShapeModelMap::const_iterator it = _shape_models.begin(); it != _shape_models.end();
         ++it) {
        names.push_back(it->second->name());
    }
    return names;
}

std::string ShapeHandler::generateName()
{
    int n = 4; // number of digits
    std::string str = std::to_string(_last_index);
    if (str.size() > n) { //
        return "Please enter name for this collection";
    }
    return std::string("ShapeModel") + std::string(n - str.size(), '0').append(str);
}

std::vector<ShapeModel*> ShapeHandler::getShapeModels()
{
    std::vector<ShapeModel*> collections;
    for (const auto& [name, ptr] : _shape_models)
        collections.push_back(ptr.get());
    return collections;
}

std::vector<ShapeModel*> ShapeHandler::getShapeModels(sptrDataSet data)
{
    std::vector<ShapeModel*> collections;
    for (const auto& [name, ptr] : _shape_models)
        if (ptr->data() == data)
            collections.push_back(ptr.get());
    return collections;
}

} // namespace ohkl
