//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/utility/ShapeComboBox.h
//! @brief     Defines class ShapeComboBox
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/utility/ShapeComboBox.h"
#include "core/shape/IPeakIntegrator.h"

#include <QSignalBlocker>

ShapesList ShapeComboBox::_shape_collections;
QVector<ShapeComboBox*> ShapeComboBox::_all_combos;

ShapeComboBox::ShapeComboBox(QWidget* parent) : QComboBox(parent)
{
    _all_combos.push_back(this);
}

void ShapeComboBox::addShapeCollection(nsx::ShapeCollection* shapes)
{
    QSignalBlocker blocker(this);
    addItem(QString::fromStdString(shapes->name()));
    _shape_collections.push_back(shapes);
    refresh();
}

void ShapeComboBox::addShapeCollections(const ShapesList& shape_list)
{
    for (auto shapes : shape_list)
        addShapeCollection(shapes);
}

//! Clear all elements
void ShapeComboBox::clearAll()
{
    QSignalBlocker blocker(this);
    _current = currentText();
    clear();
    _shape_collections.clear();
}

//! Return a pointer to the current unit shapes
nsx::ShapeCollection* ShapeComboBox::currentShapes() const
{
    if (count() != _shape_collections.size())
        throw std::runtime_error("ShapeComboBox needs refreshing");
    return _shape_collections.at(currentIndex());
}

void ShapeComboBox::refresh()
{
    QSignalBlocker blocker(this);
    _current = currentText();
    clear();
    for (nsx::ShapeCollection* shapes : _shape_collections)
        addItem(QString::fromStdString(shapes->name()));
    setCurrentText(_current);
}

void ShapeComboBox::refreshAll()
{
    for (auto* combo : _all_combos)
        combo->refresh();
}
