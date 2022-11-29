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

#ifndef OHKL_GUI_MODELS_MASKHANDLER_H
#define OHKL_GUI_MODELS_MASKHANDLER_H

#include "base/geometry/AABB.h"
#include "base/mask/BoxMask.h"
#include "base/mask/EllipseMask.h"
#include "base/mask/IMask.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/experiment/MaskExporter.h"
#include "core/experiment/MaskImporter.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/graphics_items/EllipseMaskItem.h"
#include "gui/graphics_items/MaskItem.h"
#include "gui/graphics_items/SXGraphicsItem.h"

#include <QGraphicsItem>
#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>

using GraphicItemsMap = std::map<ohkl::IMask*, QGraphicsItem*>;
using MaskCollectionMap = std::map<ohkl::sptrDataSet, GraphicItemsMap>;

class MaskHandler : public QObject {
    Q_OBJECT
 public:
    //! Default Constructor
    MaskHandler() = default;
    //! Default Destructor
    ~MaskHandler() = default;
    //! Get the QGraphicsItem* for the corresponding ohkl::sptrDataSet, ohkl::IMask*
    QGraphicsItem* getGraphicItem(ohkl::sptrDataSet dataset, ohkl::IMask* imask);
    //! get Map of ohkl::IMasks* for a given ohkl::sptrDataSet
    GraphicItemsMap getMaskMap(ohkl::sptrDataSet dataset);
    //! returns true when Maks stored in sptrDataSet and GraphicItems stored in MaskHandler are in
    //! sync
    bool check(ohkl::sptrDataSet dataset);
    //! Remove a given IMask* and its corresponding GraphicItem from sptrDataSet and Maskhandler
    bool removeMask(ohkl::sptrDataSet dataset, ohkl::IMask* mask);
    //! Remove all GraphicsItems for a given sptrDataSet from the MaskHandler
    bool removeDataSet(ohkl::sptrDataSet);
    //! Get the number of seleceted Masks for a given sptrDataSet
    size_t getNSelectedMasks(ohkl::sptrDataSet);
    //! Get the number of all GraphicItems in the handler
    size_t getTotalNMasks(ohkl::sptrDataSet);
    //! Select a Mask
    void selectMask(ohkl::sptrDataSet dataset, size_t id);
    //! Select a Mask
    void selectMask(ohkl::sptrDataSet dataset, ohkl::IMask*);
    //! Unselect a Mask
    void unselectMask(ohkl::sptrDataSet dataset, size_t id);
    //! Unselect a Mask
    void unselectMask(ohkl::sptrDataSet dataset, ohkl::IMask*);
    //! Set the Selection flag for a given a Mask
    void setSelectionFlag(ohkl::sptrDataSet dataset, size_t id, bool flag);
    //! Set the Selection flag for a given a Mas
    void setSelectionFlag(ohkl::sptrDataSet dataset, ohkl::IMask* mask, bool flag);
    //! get selection flag for a mask
    bool getSelectionFlag(ohkl::sptrDataSet dataset, size_t id);
    //! Set the Selection flag for a given a Mas
    bool getSelectionFlag(ohkl::sptrDataSet dataset, ohkl::IMask* mask);
    //! Sets the Selection flag of all Masks for a given sptrDataSet
    void setAllSelectionFlags(ohkl::sptrDataSet dataset, bool flag);
    //! Gets a vector of selection flags for all the masks in a given sptrDataSet
    std::vector<bool> getAllSelectionFlags(ohkl::sptrDataSet dataset);
    //! Remove selected masks from a given DataSet
    size_t removeSelectedMasks(ohkl::sptrDataSet dataset);
    //! Get a vector of all stored GraphicMaskItems for a given sptrDataSet
    std::vector<QGraphicsItem*> getGraphicalMaskItems(ohkl::sptrDataSet);
    //! Get a vector of IMasks from sptrDataSet
    std::vector<ohkl::IMask*> getMasks(ohkl::sptrDataSet dataset);
    //! Gets a vector of all stored map keys (IMask*) for a given sptrDatatSet
    std::vector<ohkl::IMask*> getKeys(ohkl::sptrDataSet);
    //! find the corresponding IMask for a given sptrDataSet, GraphicMaskItem
    ohkl::IMask* findIMask(ohkl::sptrDataSet dataset, QGraphicsItem* gmask);
    //! Clear Masks and GraphicMaskItems from dataset and MaskHandler
    void clearMasks(ohkl::sptrDataSet dataset);
    //! clear only GraphicMaskItems from MaskHandler
    void clearGraphicMaskItems(ohkl::sptrDataSet dataset);
    //! Import masks from an external file
    void importMasks(std::string filename, ohkl::sptrDataSet dataset);
    //! Export masks to an external file
    void exportMasks(std::string filename, ohkl::sptrDataSet dataset);
    //! Remove a given IMask* by it corresponding GraphicMaskItem
    bool removeMask(ohkl::sptrDataSet dataset, QGraphicsItem* gmask);
    //! Set visible flag in GraphicItems
    void setVisibleFlags(ohkl::sptrDataSet dataset, bool flag);
    //! Generate a QGraphicsItem for a given sptrDataSet, IMask*
    QGraphicsItem* generateGraphicItem(ohkl::IMask* mask, ohkl::sptrDataSet dataset);
    //! Generate a new IMask for a given QGraphicMaskItem
    ohkl::IMask* generateIMask(QGraphicsItem* mask_item);
    //! Add a new IMask (GraphicItem will be automatically generated and connected)
    bool addMask(ohkl::sptrDataSet dataset, ohkl::IMask* imask);
    //! Add a new GraphicItem (IMask will be automatically generated and connected)
    bool addMask(ohkl::sptrDataSet dataset, QGraphicsItem* gmask);
    //! Add a new IMask/GMaks pair
    bool addMask(ohkl::sptrDataSet dataset, ohkl::IMask* imask, QGraphicsItem* gmask);
    //! Remove a GraphicMaskItem only
    bool removeGraphicMaskItem(ohkl::sptrDataSet dataset, ohkl::IMask* mask);
    //! Remove a GraphicMaskItem only
    bool removeGraphicMaskItem(ohkl::sptrDataSet dataset, QGraphicsItem* gmask);
    //! Find the vector position of the corresponding IMask in of sptrDataSet->masks
    size_t findMaskPosition(ohkl::sptrDataSet dataset, QGraphicsItem* gmask);
    //! Rebuild all the GraphicMaskItems from sptrDataSet
    void rebuildMasks(ohkl::sptrDataSet);
    //! add IMask for an existing QGraphicMaskItem
    bool addIMask(ohkl::sptrDataSet dataset, QGraphicsItem* gmask, ohkl::IMask* imask);

 signals:
    void signalMaskChanged();

 private:
    MaskCollectionMap _mask_collection_map;
};

#endif
