//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_merge/SubframeMerge.h
//! @brief     Defines class MergedPeakInformationFrame
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_SUBFRAME_MERGE_SUBFRAMEMERGE_H
#define OHKL_GUI_SUBFRAME_MERGE_SUBFRAMEMERGE_H

#include "core/data/DataTypes.h"
#include "core/experiment/DataQuality.h"
#include "core/statistics/PeakExporter.h"

#include <QWidget>

class IntegratedPeakComboBox;
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QPushButton;
class QSpinBox;
class QStandardItemModel;
class QTableView;
class QTabWidget;
class SXPlot;

//! Frame containing interface to merge peak collections and compute quality staticstics
class SubframeMerge : public QWidget {
 public:
    SubframeMerge();
    //! Refresh all the panels
    void refreshAll();
    //! Get the merge parameters
    void grabMergeParameters();
    //! Set the merge parameters
    void setMergeParameters();
    //! Save unmerged/merged Peaks
    void exportPeaks(bool merged);
    //! Process the two inputs to create the merged dataset
    void processMerge();
    //! Refresh the QComboBox for the space group
    void refreshSpaceGroupCombo();

 private:
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
    void refreshGraph(int column);
    //! Save the resolution shell statistics
    void saveStatistics();
    //! Save the merged peaks
    void saveMergedPeaks();
    //! Save the unmerged peaks
    void saveUnmergedPeaks();
    //! Update the shell models
    void updateShellModel(
        QStandardItemModel* model, ohkl::DataResolution* resolution, ohkl::DataResolution* overall);
    //! Update the merged moddels
    void updateMergedModel(QStandardItemModel* model, ohkl::MergedPeakCollection* merged_data);
    //! Update the unmerged moddels
    void updateUnmergedModel(
        QStandardItemModel* model, ohkl::MergedPeakCollection* merged_data, bool sum_intensity);


    //! Do a single batch refinement to get one unit cell
    ohkl::sptrUnitCell singleBatchRefine();

    //! The merged peak list from pixel sum integration
    ohkl::MergedPeakCollection* _sum_merged_data;
    //! Merged data per resolution shell from pixel sum integration
    std::vector<ohkl::MergedPeakCollection*> _sum_merged_data_per_shell;
    //! The merged peak list from profile integration
    ohkl::MergedPeakCollection* _profile_merged_data;
    //! Merged data per resolution shell from profile integration
    std::vector<ohkl::MergedPeakCollection*> _profile_merged_data_per_shell;
    //! The peak exporter
    ohkl::PeakExporter _exporter;

    QSizePolicy* _size_policy_widgets;
    QSizePolicy* _size_policy_box;
    QSizePolicy* _size_policy_right;
    QSizePolicy* _size_policy_fixed;

    QTabWidget* _main_tab_widget;
    QWidget* _shell_tab;
    QWidget* _merged_tab;
    QWidget* _unmerged_tab;
    QTabWidget* _statistics_tab_widget;
    QTabWidget* _merged_tab_widget;
    QTabWidget* _unmerged_tab_widget;


    IntegratedPeakComboBox* _peak_combo_1;
    IntegratedPeakComboBox* _peak_combo_2;

    QTableView* _sum_shell_view;
    QTableView* _profile_shell_view;
    QStandardItemModel* _sum_shell_model;
    QStandardItemModel* _profile_shell_model;
    QDoubleSpinBox* _d_min;
    QDoubleSpinBox* _d_max;
    QSpinBox* _frame_min;
    QSpinBox* _frame_max;
    QSpinBox* _d_shells;
    QCheckBox* _friedel;
    QComboBox* _space_group;
    QComboBox* _plottable_statistics;
    SXPlot* _statistics_plot;
    QPushButton* _save_shell;
    QPushButton* _save_peaks;

    QTableView* _sum_merged_view;
    QTableView* _profile_merged_view;
    QPushButton* _save_merged;
    QStandardItemModel* _sum_merged_model;
    QStandardItemModel* _profile_merged_model;

    QTableView* _sum_unmerged_view;
    QTableView* _profile_unmerged_view;
    QPushButton* _save_unmerged;
    QStandardItemModel* _sum_unmerged_model;
    QStandardItemModel* _profile_unmerged_model;

    bool _frame_set;
};

#endif // OHKL_GUI_SUBFRAME_MERGE_SUBFRAMEMERGE_H
