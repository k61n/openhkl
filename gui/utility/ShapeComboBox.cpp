//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/ShapeComboBox.h
//! @brief     Defines class ShapeComboBox
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/utility/ShapeComboBox.h"
#include "core/integration/IIntegrator.h"

#include <QSignalBlocker>

ShapesList ShapeComboBox::_shape_models;
QVector<ShapeComboBox*> ShapeComboBox::_all_combos;

ShapeComboBox::ShapeComboBox(QWidget* parent) : QComboBox(parent)
{
    _all_combos.push_back(this);
}

void ShapeComboBox::addShapeModel(ohkl::ShapeModel* shapes)
{
    QSignalBlocker blocker(this);
    addItem(QString::fromStdString(shapes->name()));
    _shape_models.push_back(shapes);
    refresh();
}

void ShapeComboBox::addShapeModels(const ShapesList& shape_list)
{
    for (auto shapes : shape_list)
        addShapeModel(shapes);
}

//! Clear all elements
void ShapeComboBox::clearAll()
{
    QSignalBlocker blocker(this);
    _current = currentText();
    clear();
    _shape_models.clear();
}

//! Return a pointer to the current unit shapes
ohkl::ShapeModel* ShapeComboBox::currentShapes() const
{
    if (count() != _shape_models.size())
        throw std::runtime_error("ShapeComboBox needs refreshing");
    return _shape_models.at(currentIndex());
}

void ShapeComboBox::refresh()
{
    QSignalBlocker blocker(this);
    _current = currentText();
    clear();
    for (ohkl::ShapeModel* shapes : _shape_models)
        addItem(QString::fromStdString(shapes->name()));
    setCurrentText(_current);
}

void ShapeComboBox::refreshAll()
{
    for (auto* combo : _all_combos)
        combo->refresh();
}
