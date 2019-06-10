//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/delegates/DoubleItemDelegate.cpp
//! @brief     Implements class DoubleItemDelegate
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <QDoubleValidator>
#include <QLineEdit>

#include "apps/delegates/DoubleItemDelegate.h"

QWidget* DoubleItemDelegate::createEditor(
    QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    QLineEdit* lineEdit = new QLineEdit(parent);

    // Set validator
    QDoubleValidator* validator = new QDoubleValidator(lineEdit);
    lineEdit->setValidator(validator);

    return lineEdit;
}
