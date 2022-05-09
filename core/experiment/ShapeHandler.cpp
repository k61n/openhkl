//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/ShapeHandler.cpp
//! @brief     Handles peak manipulations for Experiment object
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/experiment/ShapeHandler.h"

#include "base/utils/Logger.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/ShapeCollection.h"
#include <memory>
#include <stdexcept>

namespace nsx {

ShapeHandler::~ShapeHandler() = default;

const ShapeCollectionMap* ShapeHandler::getShapeCollectionMap() const
{
    return &_shape_collections;
}

bool ShapeHandler::addShapeCollection(const std::string& name, const nsx::ShapeCollection& shapes)
{
    // abort if name is aleady in use
    if (hasShapeCollection(name))
        return false;
    nsxlog(
        Level::Info, "ShapeHandler::addShapeCollection '", name, "': ", shapes.numberOfPeaks(),
        " shapes");
    std::unique_ptr<ShapeCollection> ptr = std::make_unique<ShapeCollection>(shapes);
    ptr->setName(name);
    _shape_collections.insert_or_assign(name, std::move(ptr));
    return hasShapeCollection(name); // now name must be in use
}

bool ShapeHandler::addEmptyCollection(const std::string& name)
{
    if (hasShapeCollection(name))
        return false;
    nsxlog(Level::Info, "ShapeHandler::addEmptyCollection '" + name + "'");
    std::unique_ptr<ShapeCollection> ptr = std::make_unique<ShapeCollection>(ShapeCollection(name));
    _shape_collections.insert_or_assign(name, std::move(ptr));
    return hasShapeCollection(name); // now name must be in use
}

bool ShapeHandler::hasShapeCollection(const std::string& name) const
{
    auto shapes = _shape_collections.find(name);
    return (shapes != _shape_collections.end());
}

ShapeCollection* ShapeHandler::getShapeCollection(const std::string name)
{
    if (hasShapeCollection(name)) {
        return _shape_collections[name].get();
    }
    return nullptr;
}

void ShapeHandler::removeShapeCollection(const std::string& name)
{
    if (hasShapeCollection(name)) {
        auto shapes = _shape_collections.find(name);
        shapes->second.reset();
        _shape_collections.erase(shapes);
    }
}

std::vector<std::string> ShapeHandler::getCollectionNames() const
{

    std::vector<std::string> names;
    for (ShapeCollectionMap::const_iterator it = _shape_collections.begin();
         it != _shape_collections.end(); ++it) {
        names.push_back(it->second->name());
    }
    return names;
}

std::string ShapeHandler::generateName()
{
    int n = 4; // number of digits
    std::string str = std::to_string(numShapeCollections() + 1);
    if (str.size() > n) { //
        return "Please enter name for this collection";
    }
    return std::string("ShapeCollection") + std::string(n - str.size(), '0').append(str);
}

std::vector<ShapeCollection*> ShapeHandler::getShapeCollections()
{
    std::vector<ShapeCollection*> collections;
    for (const auto& [name, ptr] : _shape_collections)
        collections.push_back(ptr.get());
    return collections;
}

} // namespace nsx
