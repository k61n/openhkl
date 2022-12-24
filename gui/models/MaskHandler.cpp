//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/models/MaskHandler.h
//! @brief     Defines class MaskHandler
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/models/MaskHandler.h"

GraphicItemsMap MaskHandler::getMaskMap(ohkl::sptrDataSet dataset)
{
    return _mask_collection_map[dataset];
}

QGraphicsItem* MaskHandler::getGraphicItem(ohkl::sptrDataSet dataset, ohkl::IMask* imask)
{
    return _mask_collection_map[dataset][imask];
}

bool MaskHandler::check(ohkl::sptrDataSet dataset)
{
    if (dataset->masks().size() != _mask_collection_map[dataset].size())
        return false;
    for (auto& m : dataset->masks())
        if (_mask_collection_map[dataset].find(m) == _mask_collection_map[dataset].end())
            return false;
    return true;
}

bool MaskHandler::addMask(ohkl::sptrDataSet dataset, ohkl::IMask* imask)
{
    return addMask(dataset, imask, nullptr);
}

bool MaskHandler::addMask(ohkl::sptrDataSet dataset, QGraphicsItem* gmask)
{
    return addMask(dataset, nullptr, gmask);
}

bool MaskHandler::addMask(ohkl::sptrDataSet dataset, ohkl::IMask* imask, QGraphicsItem* gmask)
{
    if (!dataset)
        return false;

    // we need at least one valid item
    if (!imask && !gmask)
        return false;
    else if (!imask && gmask) {
        // create a corresponding imask
        imask = generateIMask(gmask);
    } else if (imask && !gmask) {
        // create a corresponding gmask
        gmask = generateGraphicItem(imask, dataset);
    }

    dataset->addMask(imask);

    // check wether there is already a map entry for this dataset at all
    if (_mask_collection_map.find(dataset) == _mask_collection_map.end()) {
        _mask_collection_map.insert({dataset, GraphicItemsMap()});
    }

    _mask_collection_map[dataset].insert({imask, gmask});

    return true;
}

bool MaskHandler::removeMask(ohkl::sptrDataSet dataset, ohkl::IMask* imask)
{
    auto imasks = dataset->masks();

    auto itr = std::find(imasks.begin(), imasks.end(), imask);
    if (itr == imasks.end())
        return false;

    dataset->removeMask(imask);
    if (_mask_collection_map[dataset].erase(imask) == 0)
        return false;

    return true;
}

void MaskHandler::clearMasks(ohkl::sptrDataSet dataset)
{
    clearGraphicMaskItems(dataset);
    dataset->removeAllMasks();
}
void MaskHandler::clearGraphicMaskItems(ohkl::sptrDataSet dataset)
{
    auto imasks = dataset->masks();
    for (auto& m : imasks) {
        if (!m)
            return;
        delete getGraphicItem(dataset, m);
        _mask_collection_map[dataset].erase(m);
    }
}

ohkl::IMask* MaskHandler::generateIMask(QGraphicsItem* mask_item)
{
    if (!mask_item)
        return nullptr;

    if (dynamic_cast<const MaskItem*>(mask_item) != nullptr)
        return new ohkl::BoxMask(*((MaskItem*)mask_item)->getAABB());
    else
        return new ohkl::EllipseMask(*((EllipseMaskItem*)mask_item)->getAABB());
}

QGraphicsItem* MaskHandler::generateGraphicItem(ohkl::IMask* mask, ohkl::sptrDataSet dataset)
{
    if (!mask)
        return nullptr;
    if (dynamic_cast<const ohkl::BoxMask*>(mask) != nullptr) {
        MaskItem* gmask_item = new MaskItem(dataset, new ohkl::AABB(mask->aabb()));
        gmask_item->setFrom(mask->aabb().lower());
        gmask_item->setTo(mask->aabb().upper());
        return gmask_item;
    } else {
        EllipseMaskItem* gmask_item = new EllipseMaskItem(dataset, new ohkl::AABB(mask->aabb()));
        gmask_item->setFrom(mask->aabb().lower());
        gmask_item->setTo(mask->aabb().upper());
        return gmask_item;
    }
}

bool MaskHandler::removeDataSet(ohkl::sptrDataSet dataset)
{
    clearGraphicMaskItems(dataset);
    return true;
}

size_t MaskHandler::getNSelectedMasks(ohkl::sptrDataSet dataset)
{
    size_t nSelectedMasks = 0;
    auto masks = dataset->masks();
    for (auto& m : masks)
        if (_mask_collection_map[dataset][m]->isSelected())
            ++nSelectedMasks;

    return nSelectedMasks;
}

size_t MaskHandler::getTotalNMasks(ohkl::sptrDataSet dataset)
{
    return _mask_collection_map[dataset].size();
}

void MaskHandler::selectMask(ohkl::sptrDataSet dataset, size_t id)
{
    setSelectionFlag(dataset, id, true);
}

void MaskHandler::selectMask(ohkl::sptrDataSet dataset, ohkl::IMask* imask)
{
    setSelectionFlag(dataset, imask, true);
}

void MaskHandler::unselectMask(ohkl::sptrDataSet dataset, size_t id)
{
    setSelectionFlag(dataset, id, false);
}

void MaskHandler::unselectMask(ohkl::sptrDataSet dataset, ohkl::IMask* imask)
{
    setSelectionFlag(dataset, imask, false);
}

void MaskHandler::setSelectionFlag(ohkl::sptrDataSet dataset, size_t id, bool flag)
{
    auto mask = dataset->masks()[id];
    setSelectionFlag(dataset, mask, flag);
}

void MaskHandler::setSelectionFlag(ohkl::sptrDataSet dataset, ohkl::IMask* imask, bool flag)
{
    _mask_collection_map[dataset][imask]->setSelected(flag);
}

bool MaskHandler::getSelectionFlag(ohkl::sptrDataSet dataset, size_t id)
{
    return getSelectionFlag(dataset, dataset->masks()[id]);
}

bool MaskHandler::getSelectionFlag(ohkl::sptrDataSet dataset, ohkl::IMask* imask)
{
    return _mask_collection_map[dataset][imask]->isSelected();
}

void MaskHandler::setAllSelectionFlags(ohkl::sptrDataSet dataset, bool flag)
{
    for (auto& m : dataset->masks())
        setSelectionFlag(dataset, m, flag);
}

std::vector<bool> MaskHandler::getAllSelectionFlags(ohkl::sptrDataSet dataset)
{
    std::vector<bool> flags;
    for (auto& m : dataset->masks())
        flags.emplace_back(getGraphicItem(dataset, m)->isSelected());
    return flags;
}

size_t MaskHandler::removeSelectedMasks(ohkl::sptrDataSet dataset)
{
    size_t nMasksRemoved = 0;
    auto masks = dataset->masks();
    for (auto& m : masks) {
        // MaskHandler and sptrDataSet are out of sync ... -> in that case a error should be thrown
        // if (_mask_collection_map[dataset].find(m) == _mask_collection_map[dataset].end())
        //    continue;
        if (_mask_collection_map[dataset][m]->isSelected()) {
            delete _mask_collection_map[dataset][m];
            _mask_collection_map[dataset].erase(m);
            dataset->removeMask(m);
            ++nMasksRemoved;
        }
    }
    return nMasksRemoved;
}

std::vector<QGraphicsItem*> MaskHandler::getGraphicalMaskItems(ohkl::sptrDataSet dataset)
{
    std::vector<QGraphicsItem*> gmasks;
    for (auto& m : dataset->masks())
        gmasks.emplace_back(getGraphicItem(dataset, m));
    return gmasks;
}

std::vector<ohkl::IMask*> MaskHandler::getMasks(ohkl::sptrDataSet dataset)
{
    return dataset->masks();
}

std::vector<ohkl::IMask*> MaskHandler::getKeys(ohkl::sptrDataSet dataset)
{
    std::vector<ohkl::IMask*> keys;
    for (auto& item : _mask_collection_map[dataset])
        keys.emplace_back(item.first);
    return keys;
}

void MaskHandler::importMasks(std::string filename, ohkl::sptrDataSet dataset)
{
    ohkl::MaskImporter mimp(filename);

    for (auto& m : mimp.getMasks())
        addMask(dataset, m);
}

void MaskHandler::exportMasks(std::string filename, ohkl::sptrDataSet dataset)
{
    ohkl::MaskExporter mexp(dataset->masks());
    mexp.exportToFile(filename);
}

ohkl::IMask* MaskHandler::findIMask(ohkl::sptrDataSet dataset, QGraphicsItem* gmask)
{
    for (auto m : dataset->masks())
        if (getGraphicItem(dataset, m) == gmask)
            return m;
    return nullptr;
}

bool MaskHandler::removeMask(ohkl::sptrDataSet dataset, QGraphicsItem* gmask)
{
    for (auto& m : dataset->masks())
        if (getGraphicItem(dataset, m) == gmask) {
            _mask_collection_map[dataset].erase(m);
            dataset->removeMask(m);
            return true;
        }
    return false;
}

void MaskHandler::setVisibleFlags(ohkl::sptrDataSet dataset, bool flag)
{
    for (auto& m : dataset->masks())
        getGraphicItem(dataset, m)->setVisible(flag);
}

size_t MaskHandler::findMaskPosition(ohkl::sptrDataSet dataset, QGraphicsItem* gmask)
{
    size_t idx = 0;
    for (auto& m : dataset->masks()) {
        if (getGraphicItem(dataset, m) == gmask)
            return idx;
        ++idx;
    }
    return -1;
}

bool MaskHandler::removeGraphicMaskItem(ohkl::sptrDataSet dataset, ohkl::IMask* imask)
{
    if (_mask_collection_map[dataset].erase(imask) == 1)
        return true;
    return false;
}

bool MaskHandler::removeGraphicMaskItem(ohkl::sptrDataSet dataset, QGraphicsItem* gmask)
{
    for (auto& m : dataset->masks())
        if (getGraphicItem(dataset, m) == gmask)
            return removeGraphicMaskItem(dataset, m);

    return false;
}

void MaskHandler::rebuildMasks(ohkl::sptrDataSet dataset)
{
    auto masks = dataset->masks();
    for (auto& m : masks) {
        delete getGraphicItem(dataset, m);
        _mask_collection_map[dataset].erase(m);
    }

    for (auto& m : masks) {
        auto gmask = generateGraphicItem(m, dataset);
        _mask_collection_map[dataset].insert({m, gmask});
    }
}

bool MaskHandler::addIMask(ohkl::sptrDataSet dataset, QGraphicsItem* gmask, ohkl::IMask* imask)
{
    if (_mask_collection_map[dataset].find(imask) == _mask_collection_map[dataset].end()) {
        // okay apparently this entry doesnt exist yet
        dataset->addMask(imask);
        _mask_collection_map[dataset].insert({imask, gmask});
        return true;
    }
    return false;
}
