//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/models/meta.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_MODELS_META_H
#define GUI_MODELS_META_H

#include <QMetaType>

#include "core/experiment/CrystalTypes.h"
#include "core/experiment/DataTypes.h"

Q_DECLARE_METATYPE(nsx::PeakList)
Q_DECLARE_METATYPE(nsx::sptrUnitCell)
Q_DECLARE_METATYPE(nsx::sptrDataSet)

#endif // GUI_MODELS_META_H
