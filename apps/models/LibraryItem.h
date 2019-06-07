//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/LibraryItem.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <string>

#include <QJsonObject>

#include <core/GeometryTypes.h>
#include <core/ShapeLibrary.h>

#include "TreeItem.h"

class QWidget;

class LibraryItem : public TreeItem {
public:
    explicit LibraryItem();

    void incorporateCalculatedPeaks();

    nsx::sptrShapeLibrary& library() { return _library; }

private:
    nsx::sptrShapeLibrary _library;
};
