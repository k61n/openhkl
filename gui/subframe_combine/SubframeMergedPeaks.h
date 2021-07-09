//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_combine/SubframeMergedPeaks.h
//! @brief     Defines class MergedPeakInformationFrame
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBFRAME_COMBINE_SUBFRAMEMERGEDPEAKS_H
#define NSX_GUI_SUBFRAME_COMBINE_SUBFRAMEMERGEDPEAKS_H

#include "core/data/DataTypes.h"
#include "core/statistics/PeakExporter.h"

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

class MergedData;
class SXPlot;

class SubframeMergedPeaks : public QWidget {
 public:
    SubframeMergedPeaks();
    //! Refresh all the panels
    void refreshAll();

 private:
    //! Get the merge parameters
    void grabMergeParameters();
    //! Set the merge parameters
    void setMergeParameters();
    //! Set up the GUI size policies
    void setSizePolicies();
    //! Set the experiments
    void setDShellUp();
    //! Update the peak list
    void setMergedUp();
    //! Update the peak list
    void setUnmergedUp();

 private:
    //! Refresh the QComboBox for the experiments
    void refreshExperimentList();
    //! Refresh the QComboBox for the found peaks
    void refreshPeakLists();
    //! Refresh the QComboBoxes for peak collections
    void refreshPeakCombos();
    //! Refresh the QComboBox for the found peaks
    void refreshFoundPeakList();
    //! Refresh the QComboBox for the predicted peaks
    void refreshPredictedPeakList();
    //! Process the two inputs to create the merged dataset
    void processMerge();
    //! Disable unsafe widgets if no data loaded
    void toggleUnsafeWidgets();

 private:
    //! Refresh the tables of the visual
    void refreshTables();
    //! Refresh the tables of the visual
    void refreshDShellTable();
    //! Refresh the tables of the visual
    void refreshMergedTable();
    //! Refresh the tables of the visual
    void refreshUnmergedTable();
    //! Refresh the tables of the visual
    void refreshGraphTable(int column);

    //! Save the statistics seen in the Dshell tab
    void saveStatistics();
    //! Save the merged peaks
    void saveMergedPeaks();
    //! Save the unmerged peaks
    void saveUnmergedPeaks();

 private:
    //! The loaded data list
    std::vector<nsx::sptrDataSet> _data_list;
    //! The loaded peak list
    QStringList _peaks1_list;
    //! The loaded peak list
    QStringList _peaks2_list;
    //! The merged peak list
    nsx::MergedData* _merged_data;
    //! The spacegroup
    nsx::PeakExporter exporter;

 private:
    QSizePolicy* _size_policy_widgets;
    QSizePolicy* _size_policy_box;
    QSizePolicy* _size_policy_right;
    QSizePolicy* _size_policy_fixed;

    QTabWidget* _main_tab_widget;
    QWidget* _shell_tab;
    QWidget* _merged_tab;
    QWidget* _unmerged_tab;

    QComboBox* _exp_drop;
    QComboBox* _peaks1_drop;
    QComboBox* _peaks2_drop;

    QTableView* _d_shell_view;
    QDoubleSpinBox* _d_min;
    QDoubleSpinBox* _d_max;
    QSpinBox* _d_shells;
    QCheckBox* _friedel;
    QComboBox* _plottable_statistics;
    SXPlot* _statistics_plot;
    QStandardItemModel* _shell_model;
    QPushButton* _save_shell;

    QTableView* _merged_view;
    QComboBox* _merged_save_type;
    QPushButton* _save_merged;
    QStandardItemModel* _merged_model;

    QTableView* _unmerged_view;
    QComboBox* _unmerged_save_type;
    QPushButton* _save_unmerged;
    QStandardItemModel* _unmerged_model;
};

#endif // NSX_GUI_SUBFRAME_COMBINE_SUBFRAMEMERGEDPEAKS_H
