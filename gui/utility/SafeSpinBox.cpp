//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/SafeSpinBox.h
//! @brief     Defines classes SafeSpinBox and SafeDoubleSpinBox
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/utility/SafeSpinBox.h"

#include <QWheelEvent>

SafeSpinBox::SafeSpinBox(QWidget* parent) : QSpinBox(parent)
{
    setFocusPolicy(Qt::StrongFocus);
}

void SafeSpinBox::wheelEvent(QWheelEvent* event)
{
    if (!hasFocus()) {
        event->ignore();
    } else {
        QSpinBox::wheelEvent(event);
    }
}

SafeDoubleSpinBox::SafeDoubleSpinBox(QWidget* parent) : QDoubleSpinBox(parent)
{
    setFocusPolicy(Qt::StrongFocus);
}

void SafeDoubleSpinBox::wheelEvent(QWheelEvent* event)
{
    if (!hasFocus()) {
        event->ignore();
    } else {
        QDoubleSpinBox::wheelEvent(event);
    }
}
