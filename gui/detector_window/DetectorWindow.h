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

#include <QDialog>
#include <QScrollBar>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QComboBox>

class DetectorScene;
class DetectorView;
class PeakViewWidget;
class PeakTableView;

class DetectorWindow : public QDialog {
 public:
    DetectorWindow(QWidget* parent = nullptr);

    void showEvent(QShowEvent* event);

    void setSizePolicies();
    void setDetectorViewUp();
    void setPeakTableUp();
    void setInputUp();
    void setPlotUp();

    void refreshDetectorView();
    void refreshPeakTable();
    void refreshAll();

    void updateExptList();
    void updateDatasetList();
    void updateDatasetParameters(int idx);
    void updatePeakList();

    DetectorView* getDetectorView() { return _detector_view; };

 private:
    QHBoxLayout* _main_layout;
    QVBoxLayout* _control_layout;
    QSplitter* _right_element;

    // Control panel elements
    QComboBox* _exp_combo;
    QComboBox* _data_combo;
    QComboBox* _peak_combo;

    PeakViewWidget* _peak_view_widget;

    // Detector elements
    DetectorScene* _detector_scene;
    DetectorView* _detector_view;
    QScrollBar* _detector_scroll;
    QSpinBox* _detector_spin;

    // Peak table elements
    PeakTableView* _peak_table;

    // Data model
    nsx::PeakCollection* _peak_collection;
    PeakCollectionItem _peak_collection_item;
    PeakCollectionModel _peak_collection_model;

    QSizePolicy* _size_policy_widgets;
    QSizePolicy* _size_policy_box;
    QSizePolicy* _size_policy_right;
    QSizePolicy* _size_policy_fixed;

    std::vector<nsx::sptrDataSet> _data_list;
    QStringList _peak_list;

    void changeSelected(PeakItemGraphic* peak_graphic);
};

#endif // NSX_GUI_DETECTOR_WINDOW_DETECTORWINDOW_H

