//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/models/Meta.h
//! @brief     Declares some Qt meta types
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_MODELS_META_H
#define NSX_GUI_MODELS_META_H

#include "core/data/DataTypes.h"
#include "core/peak/Peak3D.h"
#include "tables/crystal/UnitCell.h"
#include <QMetaType>

Q_DECLARE_METATYPE(nsx::PeakList)
Q_DECLARE_METATYPE(nsx::sptrUnitCell)
Q_DECLARE_METATYPE(nsx::sptrDataSet)

#endif // NSX_GUI_MODELS_META_H
