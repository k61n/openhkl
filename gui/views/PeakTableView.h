//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/views/PeakTableView.h
//! @brief     Defines classes PeaksTableModel and PeakTableView
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_VIEWS_PEAKTABLEVIEW_H
#define OHKL_GUI_VIEWS_PEAKTABLEVIEW_H

#include <QTableView>

class PeakTableView : public QTableView {
    Q_OBJECT
 public:
    explicit PeakTableView(QWidget* parent = nullptr);
};

#endif // OHKL_GUI_VIEWS_PEAKTABLEVIEW_H
