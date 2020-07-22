//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/views/UnitCellTableView.h
//! @brief     Defines classes PeaksTableModel and PeaksTableView
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_VIEWS_UNITCELLTABLEVIEW_H
#define NSX_GUI_VIEWS_UNITCELLTABLEVIEW_H

#include <QTableView>

class UnitCellTableView : public QTableView {
    Q_OBJECT
 public:
    explicit UnitCellTableView(QWidget* parent = 0);
};

#endif // NSX_GUI_VIEWS_UNITCELLTABLEVIEW_H
