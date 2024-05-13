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

#include <QSortFilterProxyModel>
#include <QTableView>

class QAbstractItemModel;

class PeakTableView : public QTableView {
    Q_OBJECT
 public:
    explicit PeakTableView(QWidget* parent = nullptr);

    void setModel(QAbstractItemModel* model) override;
    void selectAll() override{};

 private:
    void resetSort();

    QSortFilterProxyModel _sort_model;
};

#endif // OHKL_GUI_VIEWS_PEAKTABLEVIEW_H
