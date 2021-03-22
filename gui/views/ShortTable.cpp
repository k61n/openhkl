//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/detector_window/ShortTable.cpp
//! @brief     Defines class ShortTable
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/views/ShortTable.h"

ShortTable::ShortTable(QWidget* parent, int height) : PeakTableView(parent), _default_height(height)
{
}

QSize ShortTable::sizeHint() const
{
    return minimumSizeHint();
}

QSize ShortTable::minimumSizeHint() const
{
    int nrows = 5;
    int w = PeakTableView::sizeHint().width();
    int h = PeakTableView::rowHeight(0) * nrows;
    if (_default_height > 0)
        h = _default_height;
    return QSize(w, h);
}
