//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_refiner/tables/RefinerTables.cpp
//! @brief     Implements tables for SubframeRefiner
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/data/DataTypes.h"

#include <QTabWidget>

#ifndef OHKL_GUI_SUBFRAME_REFINER_REFINERTABLES_H
#define OHKL_GUI_SUBFRAME_REFINER_TABLES_REFINERTABLES_H

class QStandardItemModel;
class QTableView;
enum class TableType;
namespace ohkl {
class Refiner;
}

class RefinerTables : public QTabWidget {

 public:
    RefinerTables();
    void refreshTables(ohkl::Refiner* refiner, ohkl::DataSet* data);

    QVector<double> getXVals() const;
    QVector<double> getYVals(TableType table, int column) const;

 private:
    //! construct the tables
    void setLatticeTableUp();
    //! Refresh table contents
    void refreshLatticeTable(ohkl::Refiner* refiner);

    void setSamplePosTableUp();
    void refreshSamplePosTable(ohkl::Refiner* refiner, ohkl::DataSet* data);

    void setSampleOrnTableUp();
    void refreshSampleOrnTable(ohkl::Refiner* refiner, ohkl::DataSet* data);

    void setDetectorPosTableUp();
    void refreshDetectorPosTable(ohkl::Refiner* refiner, ohkl::DataSet* data);

    void setKiTableUp();
    void refreshKiTable(ohkl::Refiner* refiner, ohkl::DataSet* data);

 private:
    QWidget* _lattice_tab;
    QWidget* _sample_pos_tab;
    QWidget* _sample_orn_tab;
    QWidget* _detector_pos_tab;
    QWidget* _ki_tab;

    QTableView* _original_lattice_view;
    QTableView* _lattice_view;
    QStandardItemModel* _original_lattice_model;
    QStandardItemModel* _lattice_model;

    QTableView* _original_sample_pos_view;
    QTableView* _sample_pos_view;
    QStandardItemModel* _original_sample_pos_model;
    QStandardItemModel* _sample_pos_model;

    QTableView* _original_sample_orn_view;
    QTableView* _sample_orn_view;
    QStandardItemModel* _original_sample_orn_model;
    QStandardItemModel* _sample_orn_model;

    QTableView* _original_detector_pos_view;
    QTableView* _detector_pos_view;
    QStandardItemModel* _original_detector_pos_model;
    QStandardItemModel* _detector_pos_model;

    QTableView* _original_ki_view;
    QTableView* _ki_view;
    QStandardItemModel* _original_ki_model;
    QStandardItemModel* _ki_model;

    int _nframes;
    QVector<double> _x_vals;
};

#endif // OHKL_GUI_SUBFRAME_REFINER_TABLES_REFINERTABLES_H
