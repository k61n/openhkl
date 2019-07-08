//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/SessionModelDelegate.h
//! @brief     Defines class SessionModelDelegate
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <QStyledItemDelegate>

class SessionModelDelegate : public QStyledItemDelegate {
    Q_OBJECT

 public:
    SessionModelDelegate();

    virtual void paint(
        QPainter* painter, const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

    virtual bool editorEvent(
        QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option,
        const QModelIndex& index) override;
};
