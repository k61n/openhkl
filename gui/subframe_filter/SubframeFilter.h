//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_filter/SubframeFilter.h
//! @brief     Defines class SubframeFilter
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_SUBFRAME_FILTER_SUBFRAMEFILTER_H
#define OHKL_GUI_SUBFRAME_FILTER_SUBFRAMEFILTER_H

#include "core/shape/PeakCollection.h"
#include "gui/items/PeakCollectionItem.h"
#include "gui/models/PeakCollectionModel.h"

#include <QWidget>

class CellComboBox;
class DataComboBox;
class DetectorWidget;
class PeakComboBox;
class PeakItemGraphic;
class PeakTableView;
class PeakViewWidget;
class QComboBox;
class QCheckBox;
class QHBoxLayout;
class QPushButton;
class QRadioButton;
class QSplitter;
class QVBoxLayout;
class SafeDoubleSpinBox;
class SpoilerCheck;

//! Frame containing interface for filtering peak collections
class SubframeFilter : public QWidget {
    Q_OBJECT
 public:
    SubframeFilter();

 public:
    //! Run the filtering method
    void filterPeaks();
    //! Refresh all the inputs
    void refreshAll();
    //! Get a pointer to the detector wiget
    DetectorWidget* detectorWidget();
    //! Grab the finder parameters
    void grabFilterParameters();
    //! Set the filter parameters
    void setFilterParameters();

 public slots:

 private:
    //! Build the input
    void setInputUp();
    //! Build the state
    void setStateUp();
    //! Build the unit-call
    void setUnitCellUp();
    //! Build the strength
    void setStrengthUp();
    //! Build the range
    void setRangeUp();
    //! Build the frame range
    void setFrameRangeUp();
    //! Build the sparse
    void setSparseUp();
    //! Build the merge
    void setMergeUp();
    //! Build overlap
    void setOverlapUp();
    //! Build rejection flag
    void setRejectionFlagsUp();
    //! Build the others
    void setProceedUp();
    //! Set up the detector figure up
    void setFigureUp();
    //! Set the peak table view up
    void setPeakTableUp();
    //! Change the peak selected in the table
    void changeSelected(PeakItemGraphic* peak_graphic);
    //! Accept and save current list
    void accept();
    //! Disable unsafe widgets if no data loaded
    void toggleUnsafeWidgets();

    //! Refresh the found peaks list
    void refreshPeakTable();

    //! The model for the found peaks
    ohkl::PeakCollection _peak_collection;
    //! The temporary collection
    PeakCollectionItem _peak_collection_item;
    //! The temporary collection
    PeakCollectionModel _peak_collection_model;
    //! The loaded data list
    std::vector<ohkl::sptrDataSet> _data_list;

    QHBoxLayout* _main_layout;
    QVBoxLayout* _left_layout;
    QSplitter* _right_element;

    QCheckBox* _enabled;
    QCheckBox* _masked;
    QCheckBox* _indexed_peaks;
    QCheckBox* _extinct_spacegroup;
    CellComboBox* _unit_cell;

    SafeDoubleSpinBox* _tolerance;
    SafeDoubleSpinBox* _strength_min;
    SafeDoubleSpinBox* _strength_max;
    SafeDoubleSpinBox* _d_range_min;
    SafeDoubleSpinBox* _d_range_max;
    SafeDoubleSpinBox* _frame_min;
    SafeDoubleSpinBox* _frame_max;
    SafeDoubleSpinBox* _significance_level;
    SafeDoubleSpinBox* _peak_end;
    SafeDoubleSpinBox* _bkg_end;

    QSpinBox* _min_number_peaks;

    SpoilerCheck* _state_box;
    SpoilerCheck* _unit_cell_box;
    SpoilerCheck* _strength_box;
    SpoilerCheck* _d_range_box;
    SpoilerCheck* _frame_range_box;
    SpoilerCheck* _sparse_box;
    SpoilerCheck* _merge_box;
    SpoilerCheck* _overlap_box;
    SpoilerCheck* _rejection_flag_box;

    QRadioButton* _sum_radio_1;
    QRadioButton* _sum_radio_2;
    QRadioButton* _profile_radio_1;
    QRadioButton* _profile_radio_2;

    QComboBox* _rejection_flag_combo;

    DataComboBox* _data_combo;
    PeakComboBox* _peak_combo;

    QPushButton* _filter_button;
    QPushButton* _save_button;

    PeakViewWidget* _peak_view_widget;
    DetectorWidget* _detector_widget;
    PeakTableView* _peak_table;
};

#endif // OHKL_GUI_SUBFRAME_FILTER_SUBFRAMEFILTER_H
