//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/views/ExperimentTableView.h
//! @brief     Defines classes PeaksTableModel and PeaksTableView
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_VIEWS_EXPERIMENTTABLEVIEW_H
#define OHKL_GUI_VIEWS_EXPERIMENTTABLEVIEW_H

#include <QTableView>

class ExperimentTableView : public QTableView {
    Q_OBJECT
 public:
    explicit ExperimentTableView(QWidget* parent = 0);
};

#endif // OHKL_GUI_VIEWS_EXPERIMENTTABLEVIEW_H
