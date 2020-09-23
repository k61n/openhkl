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

#include "core/shape/PeakCollection.h"

#include "gui/graphics/DetectorView.h"
#include "gui/models/PeakCollectionModel.h"
#include "gui/utility/ColorButton.h"
#include "gui/views/PeakTableView.h"

#include "gui/utility/Spoiler.h"
#include "gui/utility/SpoilerCheck.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QSpinBox>
#include <QSplitter>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

//! Dialog to filter a peak list
class SubframeFilterPeaks : public QWidget {
 public:
    SubframeFilterPeaks();
    ~SubframeFilterPeaks();

 public:
    //! Run the filtering method
    void filterPeaks();
    //! Refresh all th einputs
    void refreshAll();

 private:
    //! Set up the GUI size policies
    void setSizePolicies();
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
    //! Build the merge
    void setProceedUp();
    //! Set up the detector figure up
    void setFigureUp();
    //! Set the peak table view up
    void setPeakTableUp();
    //! Change the peak selected in the table
    void changeSelected(PeakItemGraphic* peak_graphic);
    //! Accept and save current list
    void accept();

 private:
    //! Grab the finder parameters
    void grabFilterParameters();
    //! Set the finder parameters
    void setFilterParameters() const;

    //! Refresh all th einputs
    void setParametersUp();
    //! Refresh all th einputs
    void setExperimentsUp();
    //! Refresh all th einputs
    void updatePeakList();
    //! Update the datalist as an experiment was changed
    void updateDatasetList();
    //! Update the dataset related parameters
    void updateDatasetParameters(int idx);

    //! Refresh the found peaks list
    void refreshPeakTable();
    //! Refresh the found peaks visual properties
    void refreshPeakVisual();

 private:
    //! The model for the found peaks
    nsx::PeakCollection _peak_collection;
    //! The temporary collection
    PeakCollectionItem _peak_collection_item;
    //! The temporary collection
    PeakCollectionModel _peak_collection_model;
    //! The loaded data list
    QStringList _peak_list;
    //! The loaded data list
    std::vector<nsx::sptrDataSet> _data_list;

 private:
    QHBoxLayout* _main_layout;
    QVBoxLayout* _left_layout;
    QSplitter* _right_element;

    QCheckBox* _selected;
    QCheckBox* _masked;
    QCheckBox* _predicted;
    QCheckBox* _indexed_peaks;
    QCheckBox* _extinct_spacegroup;
    QCheckBox* _remove_overlaping;
    QCheckBox* _keep_complementary;
    QComboBox* _unit_cell;

    QDoubleSpinBox* _tolerance;
    QDoubleSpinBox* _strength_min;
    QDoubleSpinBox* _strength_max;
    QDoubleSpinBox* _d_range_min;
    QDoubleSpinBox* _d_range_max;
    QDoubleSpinBox* _frame_min;
    QDoubleSpinBox* _frame_max;
    QDoubleSpinBox* _significance_level;

    QSpinBox* _min_number_peaks;

    Spoiler* _input_box;
    SpoilerCheck* _state_box;
    SpoilerCheck* _unit_cell_box;
    SpoilerCheck* _strength_box;
    SpoilerCheck* _d_range_box;
    SpoilerCheck* _frame_range_box;
    SpoilerCheck* _sparse_box;
    SpoilerCheck* _merge_box;

    QComboBox* _exp_combo;
    QComboBox* _peak_combo;
    QComboBox* _data_combo;

    QPushButton* _filter_button;
    QPushButton* _save_button;

    QSizePolicy* _size_policy_widgets;
    QSizePolicy* _size_policy_box;
    QSizePolicy* _size_policy_right;
    QSizePolicy* _size_policy_fixed;

    DetectorView* _figure_view;
    // QGraphicsPixmapItem* _pixmap;
    QSpinBox* _figure_spin;
    QScrollBar* _figure_scroll;
    PeaksTableView* _peak_table;

    // For modifying the title of peak_group
    QGroupBox* _preview_panel;
};


#endif // NSX_GUI_SUBFRAME_FILTER_SUBFRAMEFILTERPEAKS_H
