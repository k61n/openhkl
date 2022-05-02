//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_filter/SubframeFilterPeaks.h
//! @brief     Defines class SubframeFilterPeaks
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBFRAME_FILTER_SUBFRAMEFILTERPEAKS_H
#define NSX_GUI_SUBFRAME_FILTER_SUBFRAMEFILTERPEAKS_H

#include "gui/items/PeakCollectionItem.h"
#include "gui/models/PeakCollectionModel.h"
#include "gui/utility/ColorButton.h"
#include "gui/views/PeakTableView.h"

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QSplitter>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

class DetectorWidget;
class LinkedComboBox;
class PeakComboBox;
class PeakItemGraphic;
class SpoilerCheck;
class PeakViewWidget;
class SafeSpinBox;
class SafeDoubleSpinBox;

//! Frame containing interface for filtering peak collections
class SubframeFilterPeaks : public QWidget {
 public:
    SubframeFilterPeaks();

 public:
    //! Run the filtering method
    void filterPeaks();
    //! Refresh all the inputs
    void refreshAll();
    //! Get a pointer to the detector wiget
    DetectorWidget* detectorWidget();
    //! Grab the finder parameters
    void grabFilterParameters();

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

    //! Set the finder parameters
    void setFilterParameters();

    //! Refresh all th einputs
    void updatePeakList();
    //! Update the datalist as an experiment was changed
    void updateDatasetList();

    //! Refresh the found peaks list
    void refreshPeakTable();
    //! Refresh the found peaks visual properties
    void refreshPeakVisual();

    //! The model for the found peaks
    nsx::PeakCollection _peak_collection;
    //! The temporary collection
    PeakCollectionItem _peak_collection_item;
    //! The temporary collection
    PeakCollectionModel _peak_collection_model;
    //! The loaded data list
    std::vector<nsx::sptrDataSet> _data_list;

    QHBoxLayout* _main_layout;
    QVBoxLayout* _left_layout;
    QSplitter* _right_element;

    QCheckBox* _selected;
    QCheckBox* _masked;
    QCheckBox* _predicted;
    QCheckBox* _indexed_peaks;
    QCheckBox* _extinct_spacegroup;
    QCheckBox* _keep_complementary;
    LinkedComboBox* _unit_cell;

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

    PeakComboBox* _peak_combo;

    QPushButton* _filter_button;
    QPushButton* _save_button;

    PeakViewWidget* _peak_view_widget;
    DetectorWidget* _detector_widget;
    PeakTableView* _peak_table;
};

#endif // NSX_GUI_SUBFRAME_FILTER_SUBFRAMEFILTERPEAKS_H
