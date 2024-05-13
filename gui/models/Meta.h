//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/models/Meta.h
//! @brief     Declares some Qt meta types
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_MODELS_META_H
#define OHKL_GUI_MODELS_META_H

#include "core/data/DataTypes.h"

#include <QMetaType>

Q_DECLARE_METATYPE(ohkl::PeakList)
Q_DECLARE_METATYPE(ohkl::sptrUnitCell)
Q_DECLARE_METATYPE(ohkl::sptrDataSet)

#endif // OHKL_GUI_MODELS_META_H
