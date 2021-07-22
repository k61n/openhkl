//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/utility/LinkedComboBox.h
//! @brief     Defines class LinkedComboBox
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/utility/LinkedComboBox.h"

#include "gui/connect/Sentinel.h"

#include <QSignalBlocker>
#include <QDebug>

int LinkedComboBox::_count = 0;

LinkedComboBox::LinkedComboBox(ComboType combo_type, Sentinel* sentinel, QWidget* parent)
    : QComboBox(parent), _combo_type(combo_type), _id(++_count)
{
    connect(sentinel, &Sentinel::linkedComboChanged, this, &LinkedComboBox::onComboChanged);
    connect(this, &LinkedComboBox::comboChanged, sentinel, &Sentinel::onLinkedComboChanged);
}

void LinkedComboBox::updateList(const QStringList& list)
{
    QString current_text = currentText();
    clear();
    addItems(list);
    setCurrentText(current_text);
    emit comboChanged(_combo_type, _id, list);
}

void LinkedComboBox::onComboChanged(
    const ComboType combo_type, const int id, const QStringList& list)
{
    qDebug() << "onComboChanged " << static_cast<int>(combo_type) << id << list[0];
    const QSignalBlocker blocker(this);
    if (_combo_type == combo_type && !(_id == id)){
        QString current_text = currentText();
        clear();
        addItems(list);
        setCurrentText(current_text);
    }
}
