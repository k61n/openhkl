//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/widgets/PlotCheckBox.cpp
//! @brief     QCheckBox with table metadata
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/widgets/PlotCheckBox.h"

PlotCheckBox::PlotCheckBox() : QCheckBox()
{
}

PlotCheckBox::PlotCheckBox(const QString& text, TableType table, int column) :
    QCheckBox(text), _table(table), _column(column)
{
    _header = text;
}

TableType PlotCheckBox::getTable() const
{
    return _table;
}


int PlotCheckBox::getColumn() const
{
    return _column;
}

QString PlotCheckBox::getHeader() const
{
    return _header;
}
