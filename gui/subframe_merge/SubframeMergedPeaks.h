//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_merge/SubframeMergedPeaks.h
//! @brief     Defines class MergedPeakInformationFrame
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBFRAME_MERGE_SUBFRAMEMERGEDPEAKS_H
#define NSX_GUI_SUBFRAME_MERGE_SUBFRAMEMERGEDPEAKS_H

#include "core/data/DataTypes.h"
#include "core/statistics/PeakExporter.h"
#include "tables/crystal/UnitCell.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QSpinBox>
#include <QSplitter>
#include <QStandardItemModel>
#include <QTabWidget>
#include <QTableView>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

class IntegratedPeakComboBox;
class PeakComboBox;
class MergedData;
class SXPlot;

//! Frame containing interface to merge peak collections and compute quality staticstics
class SubframeMergedPeaks : public QWidget {
 public:
    SubframeMergedPeaks();
    //! Refresh all the panels
    void refreshAll();
    //! Get the merge parameters
    void grabMergeParameters();

 private:
    //! Set the merge parameters
    void setMergeParameters();
    //! Set up the GUI size policies
    void setSizePolicies();
    //! Set up widget for resolution shells
    void setDShellUp();
    //! Set up the widget for merged represenation
    void setMergedUp();
    //! Set up the widget for the unmerged representation
    void setUnmergedUp();

    //! Refresh the QComboBox for the found peaks
    void refreshPeakLists();
    //! Refresh the QComboBoxes for peak collections
    void refreshPeakCombos();
    //! Refresh the QComboBox for the space group
    void refreshSpaceGroupCombo();
    //! Process the two inputs to create the merged dataset
    void processMerge();
    //! Disable unsafe widgets if no data loaded
    void toggleUnsafeWidgets();

    //! Refresh all tables
    void refreshTables();
    //! Refresh the resolution shell table
    void refreshDShellTable();
    //! Refresh the merged representation table
    void refreshMergedTable();
    //! Refresh the unmerged representation table
    void refreshUnmergedTable();
    //! Refresh the graph
    void refreshGraphTable(int column);

    //! Save the resolution shell statistics
    void saveStatistics();
    //! Save the merged peaks
    void saveMergedPeaks();
    //! Save the unmerged peaks
    void saveUnmergedPeaks();

    //! Do a single batch refinement to get one unit cell
    nsx::sptrUnitCell singleBatchRefine();

    //! The merged peak list
    nsx::MergedData* _merged_data;
    //! Merged data per resolution shell
    std::vector<nsx::MergedData*> _merged_data_per_shell;
    //! The peak exporter
    nsx::PeakExporter exporter;

    QSizePolicy* _size_policy_widgets;
    QSizePolicy* _size_policy_box;
    QSizePolicy* _size_policy_right;
    QSizePolicy* _size_policy_fixed;

    QTabWidget* _main_tab_widget;
    QWidget* _shell_tab;
    QWidget* _merged_tab;
    QWidget* _unmerged_tab;

    IntegratedPeakComboBox* _peak_combo_1;
    IntegratedPeakComboBox* _peak_combo_2;

    QTableView* _d_shell_view;
    QDoubleSpinBox* _d_min;
    QDoubleSpinBox* _d_max;
    QSpinBox* _frame_min;
    QSpinBox* _frame_max;
    QSpinBox* _d_shells;
    QCheckBox* _friedel;
    QComboBox* _space_group;
    QComboBox* _plottable_statistics;
    SXPlot* _statistics_plot;
    QStandardItemModel* _shell_model;
    QDoubleSpinBox* _intensity_rescale_merged;
    QDoubleSpinBox* _intensity_rescale_unmerged;
    QPushButton* _save_shell;

    QTableView* _merged_view;
    QComboBox* _merged_save_type;
    QPushButton* _save_merged;
    QStandardItemModel* _merged_model;

    QTableView* _unmerged_view;
    QComboBox* _unmerged_save_type;
    QPushButton* _save_unmerged;
    QStandardItemModel* _unmerged_model;

    bool _frame_set;
};

#endif // NSX_GUI_SUBFRAME_MERGE_SUBFRAMEMERGEDPEAKS_H
