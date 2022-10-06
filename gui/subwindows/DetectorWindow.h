//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subwindows/DetectorWindow.cpp
//! @brief     Implements class DetectorWindow
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_SUBWINDOWS_DETECTORWINDOW_H
#define OHKL_GUI_SUBWINDOWS_DETECTORWINDOW_H

#include "core/loader/XFileHandler.h"
#include "core/shape/PeakCollection.h"
#include "gui/items/PeakCollectionItem.h"
#include "gui/models/PeakCollectionModel.h"

#include <QComboBox>
#include <QDialog>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QSplitter>
#include <QVBoxLayout>

class ColorButton;
class DetectorWidget;
class PeakViewWidget;
class SafeSpinBox;
class ShortTable;
class CellComboBox;
class PeakComboBox;

//! Modeless dialog containing a non-contextual detector scene (open via Menu)
class DetectorWindow : public QDialog {
 public:
    DetectorWindow(QWidget* parent = nullptr);

    //! Overload QDialog::showEvent to resize window at runtime
    void showEvent(QShowEvent* event);
    //! Refresh the whole dialog
    void refreshAll();
    //! Get a pointer to the DetectorView
    DetectorWidget* detectorWidget();

 public slots:
    void setUnitCell();

 private:
    //! Set up the detector image
    void setDetectorViewUp();
    //! Set up the peak tables
    void setPeakTableUp();
    //! Set up control panel widgets
    void setInputUp();
    //! Set up interface peaks read from 3rd party software files
    void set3rdPartyPeaksUp();
    void load3rdPartyPeaks();
    void setPlotUp(PeakViewWidget* peak_widget, QString name);

    //! Update experiment QComboBoxes
    void updateExptList();
    //! Update data set QComboBoxes
    void updateDatasetList();
    //! Update metadata extracted from selected data set
    void updateDatasetParameters(int idx);
    //! Refresh the detector image
    void refreshDetectorView();
    //! Refresh the peak tables
    void refreshPeakTable();
    //! Scroll the table to the peak under the cursor in the detector view
    void changeSelected(PeakItemGraphic* peak_graphic);

    int _nframes;

    QVBoxLayout* _control_layout;
    QSplitter* _right_element;

    // Control panel elements
    QComboBox* _exp_combo;
    PeakComboBox* _peak_combo_1;
    PeakComboBox* _peak_combo_2;
    CellComboBox* _unit_cell_combo;

    // 3rd party peak plotting elements
    QCheckBox* _draw_3rdparty;
    ColorButton* _3rdparty_color;
    SafeSpinBox* _3rdparty_size;
    /* SafeSpinBox* _3rdparty_start_frame; */

    // the widget for plotting elements on the detector scene
    PeakViewWidget* _peak_view_widget_1;
    PeakViewWidget* _peak_view_widget_2;

    // Detector elements
    DetectorWidget* _detector_widget;

    // Peak table elements
    ShortTable* _peak_table_1;
    ShortTable* _peak_table_2;

    // Data model
    ohkl::PeakCollection* _peak_collection_1;
    PeakCollectionItem _peak_collection_item_1;
    PeakCollectionModel _peak_collection_model_1;
    ohkl::PeakCollection* _peak_collection_2 = nullptr;
    PeakCollectionItem _peak_collection_item_2;
    PeakCollectionModel _peak_collection_model_2;

    QStringList _peak_list;
    QStringList _cell_list;

    // 3rd party peaks container
    ohkl::PeakCenterDataSet _peakCenterData;
};

#endif // OHKL_GUI_SUBWINDOWS_DETECTORWINDOW_H
