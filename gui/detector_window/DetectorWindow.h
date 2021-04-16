//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/detector_window/DetectorWindow.cpp
//! @brief     Implements class DetectorWindow
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_DETECTOR_WINDOW_DETECTORWINDOW_H
#define NSX_GUI_DETECTOR_WINDOW_DETECTORWINDOW_H

#include "core/shape/PeakCollection.h"
#include "gui/items/PeakCollectionItem.h"
#include "gui/models/PeakCollectionModel.h"

#include <QComboBox>
#include <QDialog>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QSpinBox>
#include <QSplitter>
#include <QVBoxLayout>

class DetectorScene;
class DetectorView;
class PeakViewWidget;
class ShortTable;

//! Modeless dialog containing a non-contextual detector scene (open via Menu)
class DetectorWindow : public QDialog {
 public:
    DetectorWindow(QWidget* parent = nullptr);

    //! Overload QDialog::showEvent to resize window at runtime
    void showEvent(QShowEvent* event);

    //! Set up control panel widgets
    void setDetectorViewUp();
    void setPeakTableUp();
    void setInputUp();
    void setPlotUp(PeakViewWidget* peak_widget, QString name);

    void refreshDetectorView();
    void refreshPeakTable();
    //! Refresh the whole dialog
    void refreshAll();

    //! Update experiment QComboBoxes
    void updateExptList();
    //! Update data set QComboBoxes
    void updateDatasetList();
    //! Update metadata extracted from selected data set
    void updateDatasetParameters(int idx);
    //! Update peak collection QComboBoxes
    void updatePeakList();
    //! Update unit cell QComboBox
    void updateUnitCellList();

    //! Get a pointer to the DetectorView
    DetectorView* getDetectorView() { return _detector_view; };

 public slots:
     void setUnitCell();

 private:
    void changeSelected(PeakItemGraphic* peak_graphic);

 private:
    QVBoxLayout* _control_layout;
    QSplitter* _right_element;

    // Control panel elements
    QComboBox* _exp_combo;
    QComboBox* _data_combo;
    QComboBox* _peak_combo_1;
    QComboBox* _peak_combo_2;
    QComboBox* _unit_cell_combo;

    // the widget for plotting elements on the detector scene
    PeakViewWidget* _peak_view_widget_1;
    PeakViewWidget* _peak_view_widget_2;

    // Detector elements
    DetectorView* _detector_view;
    QScrollBar* _detector_scroll;
    QSpinBox* _detector_spin;
    QComboBox* _cursor_mode;

    // Peak table elements
    ShortTable* _peak_table_1;
    ShortTable* _peak_table_2;

    // Data model
    nsx::PeakCollection* _peak_collection_1;
    PeakCollectionItem _peak_collection_item_1;
    PeakCollectionModel _peak_collection_model_1;
    nsx::PeakCollection* _peak_collection_2 = nullptr;
    PeakCollectionItem _peak_collection_item_2;
    PeakCollectionModel _peak_collection_model_2;

    std::vector<nsx::sptrDataSet> _data_list;
    QStringList _peak_list;
    QStringList _cell_list;
};

#endif // NSX_GUI_DETECTOR_WINDOW_DETECTORWINDOW_H
