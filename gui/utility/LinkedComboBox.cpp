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

int LinkedComboBox::_count = 0;

LinkedComboBox::LinkedComboBox(ComboType combo_type, Sentinel* sentinel, QWidget* parent)
    : QComboBox(parent), _combo_type(combo_type), _id(++_count)
{
    connect(sentinel, &Sentinel::linkedComboChanged, this, &LinkedComboBox::onComboChanged);
    connect(sentinel, &Sentinel::linkedComboItemAdded, this, &LinkedComboBox::onItemAdded);
    connect(sentinel, &Sentinel::setLinkedComboList, this, &LinkedComboBox::onSetItems);
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
    const QSignalBlocker blocker(this);
    if (_combo_type == combo_type && !(_id == id)) {
        QString current_text = currentText();
        if (itemText(0) == QString()) { // For combos with a blank entry at the start
            clear();
            addItems(list);
            insertItem(0, QString());
        } else {
            clear();
            addItems(list);
        }
        setCurrentText(current_text);
    }
}

void LinkedComboBox::onItemAdded(ComboType combo_type, const QString& item)
{
    const QSignalBlocker blocker(this);
    if (_combo_type == combo_type)
        addItem(item);
}

void LinkedComboBox::onSetItems(ComboType combo_type, const QStringList& list)
{
    const QSignalBlocker blocker(this);
    if (_combo_type == combo_type) {
        QString current_text = currentText();
        if (itemText(0) == QString()) { // For combos with a blank entry at the start
            clear();
            addItems(list);
            insertItem(0, QString());
        } else {
            clear();
            addItems(list);
        }
        setCurrentText(current_text);
    }
}
