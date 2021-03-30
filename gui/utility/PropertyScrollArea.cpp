//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/utility/PropertyScrollArea.cpp
//! @brief     Implements class PropertyScrollArea
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/utility/PropertyScrollArea.h"

#include <QScrollBar>
#include <QTimer>

PropertyScrollArea::PropertyScrollArea(QWidget* parent) : QScrollArea(parent)
{
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
}

void PropertyScrollArea::setContentLayout(QLayout* layout)
{
    QWidget* scroll_widget = new QWidget();
    scroll_widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    scroll_widget->setLayout(layout);
    scroll_widget->installEventFilter(this);
    QScrollArea::setWidget(scroll_widget);
}

bool PropertyScrollArea::eventFilter(QObject* obj, QEvent* ev)
{
    const bool widthNeedsUpdate = (obj == widget() && ev->type() == QEvent::Resize)
        || (obj == verticalScrollBar() && ev->type() == QEvent::Show)
        || (obj == verticalScrollBar() && ev->type() == QEvent::Hide);

    if (widthNeedsUpdate)
        // delayed to wait until scrollbar show/hide finished
        QTimer::singleShot(0, this, &PropertyScrollArea::updateFixedWidth);

    return QScrollArea::eventFilter(obj, ev);
}

void PropertyScrollArea::updateFixedWidth()
{
    setFixedWidth(width() - viewport()->width() + widget()->width());
}
