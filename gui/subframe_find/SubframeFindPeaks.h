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
#include <QHBoxLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QSplitter>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

class DetectorScene;
class DetectorWidget;
class LinkedComboBox;
class PeakItemGraphic;
class PeakTableView;
class PeakViewWidget;
class SafeSpinBox;
class SafeDoubleSpinBox;

//! Frame containing interface to find peaks from detector images
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
    //! Get the detector widget
    DetectorWidget* detectorWidget();
    //! Grab the finder parameters
    void grabFinderParameters();
    //! Grab the integration parameters
    void grabIntegrationParameters();

 private:
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
    //! Disable widgets that are unsafe without relevant data
    void toggleUnsafeWidgets();

    //! Update the datalist as an experiment was changed
    void updateDatasetList();
    //! Update the dataset related parameters
    void updateDatasetParameters(const QString& dataname);
    //! Set the finder parameters
    void setFinderParameters();

    //! Set the integration parameters
    void setIntegrationParameters();

    //! Convolution parameter map
    std::map<std::string, double> convolutionParameters();

    //! The model for the found peaks
    nsx::PeakCollection _peak_collection;
    //! The temporary collection
    PeakCollectionItem _peak_collection_item;
    //! The temporary collection
    PeakCollectionModel _peak_collection_model;

    //! Flag to check whether the peaks have been integrated
    bool _peaks_integrated;

    QVBoxLayout* _left_layout;
    QSplitter* _right_element;

    LinkedComboBox* _exp_combo;
    LinkedComboBox* _data_combo;
    QCheckBox* _all_data;

    SafeSpinBox* _threshold_spin;
    SafeDoubleSpinBox* _scale_spin;
    SafeSpinBox* _min_size_spin;
    SafeSpinBox* _max_size_spin;
    SafeSpinBox* _max_width_spin;
    QComboBox* _kernel_combo;
    QTableWidget* _kernel_para_table;
    SafeSpinBox* _start_frame_spin;
    SafeSpinBox* _end_frame_spin;

    PeakViewWidget* _peak_view_widget;
    QCheckBox* _live_check;

    SafeDoubleSpinBox* _peak_area;
    SafeDoubleSpinBox* _bkg_lower;
    SafeDoubleSpinBox* _bkg_upper;

    QPushButton* _find_button;
    QPushButton* _integrate_button;
    QPushButton* _save_button;

    DetectorWidget* _detector_widget;
    QGraphicsPixmapItem* _pixmap;

    PeakTableView* _peak_table;
};

#endif // NSX_GUI_SUBFRAME_FIND_SUBFRAMEFINDPEAKS_H
