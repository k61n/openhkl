//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/ShortTable.h
//! @brief     Defines class ShortTable
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_VIEWS_SHORTTABLE_H
#define OHKL_GUI_VIEWS_SHORTTABLE_H

#include <QTableView>

#include "gui/views/PeakTableView.h"

class ShortTable : public PeakTableView {
    Q_OBJECT

 public:
    ShortTable(QWidget* parent = 0, int height = -1);

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

 private:
    int _default_height;
};

#endif // OHKL_GUI_VIEWS_SHORTTABLE_H
