//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
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

enum class TableType { Lattice, SamplePos, SampleOrn, DetectorPos, Ki, Count };

class PlotCheckBox : public QCheckBox {
 public:
    PlotCheckBox() = delete;
    PlotCheckBox(const QString& text, TableType table, int column);

    //! Get the table type
    TableType getTable() const;
    //! Get the column index
    int getColumn() const;
    //! Get the column header
    QString getHeader() const;
    //! Get the axis label
    QString getLabel() const;

 private:
    TableType _table;
    int _column;
    QString _header;
};

#endif // NSX_GUI_WIDGETS_PLOTCHECKBOX_H
