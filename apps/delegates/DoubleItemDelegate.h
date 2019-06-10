//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/delegates/DoubleItemDelegate.h
//! @brief     Defines class DoubleItemDelegate
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <QItemDelegate>
#include <QModelIndex>
#include <QStyleOptionViewItem>

class DoubleItemDelegate : public QItemDelegate {
public:
    virtual QWidget* createEditor(
        QWidget* parent, const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;
};
