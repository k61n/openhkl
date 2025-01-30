//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/widgets/PlotCheckBox.h
//! @brief     Defines class PlotCheckBox
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_WIDGETS_PLOTCHECKBOX_H
#define OHKL_GUI_WIDGETS_PLOTCHECKBOX_H

#include <QCheckBox>
#include <qobjectdefs.h>

enum class TableType { Lattice, SamplePos, SampleOrn, DetectorPos, Ki, Count };

class PlotCheckBox : public QCheckBox {
    Q_OBJECT
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

#endif // OHKL_GUI_WIDGETS_PLOTCHECKBOX_H
