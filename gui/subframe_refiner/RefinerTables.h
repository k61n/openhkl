//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_refiner/tables/RefinerTables.cpp
//! @brief     Implements tables for SubframeRefiner
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/algo/Refiner.h"

#include <QTableWidgetItem>
#include <QStandardItemModel>

#ifndef NSX_GUI_SUBFRAME_REFINER_REFINERTABLES_H
#define NSX_GUI_SUBFRAME_REFINER_TABLES_REFINERTABLES_H

class RefinerTables : public QTabWidget {

 public:
    RefinerTables();
    void refreshTables(nsx::Refiner* refiner);

 private:
    void setSizePolicies();
    //! construct the tables
    void setLatticeTableUp();
    //! Refresh table contents
    void refreshLatticeTable(nsx::Refiner* refiner);


 private:
    QWidget* _lattice_tab;
    QWidget* _sample_position_tab;
    QWidget* _sample_orientation_tab;
    QWidget* _detector_orientation_tab;
    QWidget* _ki_tab;

    QTableView* _lattice_view;
    QStandardItemModel* _lattice_model;

};

#endif  // NSX_GUI_SUBFRAME_REFINER_TABLES_REFINERTABLES_H
