//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/widgets/PlotCheckBox.h
//! @brief     Defines class PlotCheckBox
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_WIDGETS_PLOTCHECKBOX_H
#define NSX_GUI_WIDGETS_PLOTCHECKBOX_H

#include <QCheckBox>

enum class TableType { Lattice, SamplePos, SampleOrn, DetectorPos, Ki, Count};

class PlotCheckBox : public QCheckBox {
 public:
    PlotCheckBox();
    PlotCheckBox(const QString& text, TableType table, int column);

    TableType getTable() const;
    int getColumn() const;
    QString getHeader() const;


 private:
    TableType _table;
    int _column;
    QString _header;
};

#endif // NSX_GUI_WIDGETS_PLOTCHECKBOX_H
