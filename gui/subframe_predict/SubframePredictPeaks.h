//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_predict/SubframePredictPeaks.h
//! @brief     Defines classes FoundPeaks, PeakFinderFrame
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBFRAME_PREDICT_SUBFRAMEPREDICTPEAKS_H
#define NSX_GUI_SUBFRAME_PREDICT_SUBFRAMEPREDICTPEAKS_H

#include "core/shape/PeakCollection.h"
#include "gui/items/PeakCollectionItem.h"
#include "gui/models/PeakCollectionModel.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
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
class Spoiler;

namespace nsx {
struct PredictionParameters;
struct ShapeCollectionParameters;
}

//! Frame which shows the settings to find peaks
class SubframePredictPeaks : public QWidget {
 public:
    SubframePredictPeaks();
    //! run the prediction
    void runPrediction();
    //! Refresh all the panels
    void refreshAll();
    //! detector view
    DetectorView* getDetectorView() { return _figure_view; }


 private:
    //! Set the parameters values up
    void setParametersUp();
    //! Build the buttons
    void setProceedUp();
    //! Set the peak table view up
    void setPeakTableUp();
    //! Set up the detector figure up
    void setFigureUp();
    //! Set the controls to show/hide peaks in detector scene
    void setPreviewUp();
    //! Set the save button up
    void setSaveUp();

 private:
    //! Set the experiments
    void setExperiments();
    //! Update the peak list
    void updatePeakList();
    //! Update the peak list
    void updateUnitCellList();
    //! Update the peak list
    void updateDatasetList();
    //! Update the peak list
    void updateDatasetParameters(int idx);
    //! Get the parameters of the indexer
    void grabPredictorParameters();
    //! Get the parameters of the indexer
    void setPredictorParameters();
    //! Refresh the found peaks list
    void refreshPeakTable();
    //! Refresh the found peaks visual properties
    void refreshPeakVisual();
    //! Change the peak selected in the table
    void changeSelected(PeakItemGraphic* peak_graphic);
    //! Accept and save current list
    void accept();
    //! Disable unsafe widgets if no data loaded
    void toggleUnsafeWidgets();

 private:
    //! The model for the found peaks
    nsx::PeakCollection _peak_collection;
    //! The temporary collection
    PeakCollectionItem _peak_collection_item;
    //! The temporary collection
    PeakCollectionModel _peak_collection_model;
    //! The loaded data list
    std::vector<nsx::sptrDataSet> _data_list;
    //! The loaded peak list
    QStringList _unit_cell_list;
    //! Parameters for shape collection
    nsx::ShapeCollectionParameters _shape_params;

 private:
    QVBoxLayout* _left_layout;
    QSplitter* _right_element;

    Spoiler* _para_box;
    Spoiler* _integrate_box;
    Spoiler* _preview_box;

    QComboBox* _unit_cells;
    QComboBox* _integrator;
    QDoubleSpinBox* _d_min;
    QDoubleSpinBox* _d_max;

    QPushButton* _save_button;
    QPushButton* _predict_button;

    QComboBox* _exp_combo;
    QComboBox* _data_combo;

    PeakViewWidget* _peak_view_widget;

    DetectorView* _figure_view;
    QSpinBox* _figure_spin;
    QScrollBar* _figure_scroll;

    PeakTableView* _peak_table;

    QSizePolicy _size_policy_right;
};

#endif // NSX_GUI_SUBFRAME_PREDICT_SUBFRAMEPREDICTPEAKS_H
