//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/Sentinel.cpp
//! @brief     Implements class Sentinel
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/connect/Sentinel.h"

#include "gui/utility/LinkedComboBox.h"

#include "gui/MainWin.h"

Sentinel::Sentinel() { }

void Sentinel::onLinkedComboChanged(
    const ComboType combo_type, const int id, const QStringList& list)
{
    emit linkedComboChanged(combo_type, id, list);
}

void Sentinel::addLinkedComboItem(const ComboType combo_type, const QString& item)
{
    emit linkedComboItemAdded(combo_type, item);
}

void Sentinel::setLinkedComboItems(const ComboType combo_type, const QStringList& list)
{
    emit setLinkedComboList(combo_type, list);
}
