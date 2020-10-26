//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_find/SubframeFindPeaks.h
//! @brief     Defines classes FoundPeaks, SubframeFindPeaks
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBFRAME_FIND_SUBFRAMEFINDPEAKS_H
#define NSX_GUI_SUBFRAME_FIND_SUBFRAMEFINDPEAKS_H

#include "core/shape/PeakCollection.h"
#include "gui/items/PeakCollectionItem.h"
#include "gui/models/PeakCollectionModel.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QSpinBox>
#include <QSplitter>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

class DetectorView;
class PeakItemGraphic;
class PeakTableView;
class PeakViewWidget;

//! Frame which shows the settings to find peaks
class SubframeFindPeaks : public QWidget {
 public:
    SubframeFindPeaks();
    //! Change the convolution parameters
    void updateConvolutionParameters();
    //! Find peaks
    void find();
    //! integrate found peaks
    void integrate();
    //! Refresh all the panels
    void refreshAll();
    //! detector view
    DetectorView* getDetectorView() { return _figure_view; }

 private:
    //! Set up the GUI size policies
    void setSizePolicies();
    //! Set up the data selection GUI
    void setDataUp();
    //! Set up the blob finding GUI
    void setBlobUp();
    //! Set up the Preview GUI
    void setPreviewUp();
    //! Set up the save button
    void setSaveUp();
    //! Set up the Preview GUI
    void setIntegrateUp();
    //! Set up the detector figure up
    void setFigureUp();
    //! Set the peak table view up
    void setPeakTableUp();
    //! Set the parameters values up
    void setParametersUp();

    //! Refresh the preview
    void refreshPreview();
    //! Refresh the found peaks list
    void refreshPeakTable();
    //! Refresh the found peaks visual properties
    void refreshPeakVisual();
    //! Change the peak selected in the table
    void changeSelected(PeakItemGraphic* peak_graphic);
    //! Accept and save current list
    void accept();

 private:
    //! Update the datalist as an experiment was changed
    void setExperimentsUp();

 private:
    //! Update the datalist as an experiment was changed
    void updateDatasetList();
    //! Update the dataset related parameters
    void updateDatasetParameters(int idx);

    //! Grab the finder parameters
    void grabFinderParameters();
    //! Set the finder parameters
    void setFinderParameters();

    //! Grab the Integration parameters
    void grabIntegrationParameters();

 private:
    //! Convolution parameter map
    std::map<std::string, double> convolutionParameters();

 private:
    //! The model for the found peaks
    nsx::PeakCollection _peak_collection;
    //! The temporary collection
    PeakCollectionItem _peak_collection_item;
    //! The temporary collection
    PeakCollectionModel _peak_collection_model;
    //! The loaded data list
    std::vector<nsx::sptrDataSet> _data_list;

 private:
    QHBoxLayout* _main_layout;

    QVBoxLayout* _left_layout;
    QSplitter* _right_element;

    QComboBox* _exp_combo;
    QComboBox* _data_combo;
    QCheckBox* _all_data;

    QSpinBox* _threshold_spin;
    QDoubleSpinBox* _scale_spin;
    QSpinBox* _min_size_spin;
    QSpinBox* _max_size_spin;
    QSpinBox* _max_width_spin;
    QComboBox* _kernel_combo;
    QTableWidget* _kernel_para_table;
    QSpinBox* _start_frame_spin;
    QSpinBox* _end_frame_spin;

    PeakViewWidget* _peak_view_widget;
    QCheckBox* _live_check;

    QDoubleSpinBox* _peak_area;
    QDoubleSpinBox* _bkg_lower;
    QDoubleSpinBox* _bkg_upper;

    DetectorView* _figure_view;
    QGraphicsPixmapItem* _pixmap;
    QSpinBox* _figure_spin;
    QScrollBar* _figure_scroll;

    PeakTableView* _peak_table;

    QPushButton* _find_button;
    QPushButton* _integrate_button;
    QPushButton* _save_button;

    QSizePolicy* _size_policy_widgets;
    QSizePolicy* _size_policy_box;
    QSizePolicy* _size_policy_right;
    QSizePolicy* _size_policy_fixed;
};

#endif // NSX_GUI_SUBFRAME_FIND_SUBFRAMEFINDPEAKS_H
