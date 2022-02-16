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
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
#include <QSplitter>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

class DetectorWidget;
class LinkedComboBox;
class PeakItemGraphic;
class PeakTableView;
class PeakViewWidget;
class Spoiler;
class SafeSpinBox;
class SafeDoubleSpinBox;

namespace nsx {
struct PredictionParameters;
struct ShapeCollectionParameters;
}

//! Frame containing interface for predicting peaks from unit cell
class SubframePredictPeaks : public QWidget {
 public:
    SubframePredictPeaks();
    //! Show direct beam position computed from unit cell in DetectorScene
    void showDirectBeamEvents();
    //! Refresh all the panels
    void refreshAll();
    //! Get a pointer to the detector widget
    DetectorWidget* detectorWidget();

 private:
    //! Set the parameters values up
    void setParametersUp();
    //! Set the shape collection construction update
    void setShapeCollectionUp();
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

    //! Set the experiments
    void setExperiments();
    //! Update the peak list
    void updatePeakList();
    //! Update the peak list
    void updateUnitCellList();
    //! Update the peak list
    void updateDatasetList();
    //! Get the parameters of the indexer
    void grabPredictorParameters();
    //! Set the parameters of the indexer
    void setPredictorParameters();
    //! Get shape collection parameters
    void grabShapeCollectionParameters();
    //! Set shape collection parameters
    void setShapeCollectionParameters();
    //! Refresh the peak combo
    void refreshPeakCombo();
    //! Refine the incident wavevector
    void refineKi();
    //! Refresh the found peaks list
    void refreshPeakTable();
    //! Refresh the found peaks visual properties
    void refreshPeakVisual();
    //! Change the peak selected in the table
    void changeSelected(PeakItemGraphic* peak_graphic);
    //! run the prediction
    void runPrediction();
    //! Build the shapes to assign to predicted peaks
    void assignPeakShapes();
    //! Accept and save current list
    void accept();
    //! Disable unsafe widgets if no data loaded
    void toggleUnsafeWidgets();
    //! Compute beam divergence and mosaicity sigmas
    void computeSigmas();

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
    //! Flag to check whether peaks have been predicted
    bool _peaks_predicted;
    //! Flag to check whether shapes have been assigned to predicted peaks
    bool _shapes_assigned;
    //! Shape collection paramters
    std::shared_ptr<nsx::ShapeCollectionParameters> _shape_params;

    QVBoxLayout* _left_layout;
    QSplitter* _right_element;

    Spoiler* _para_box;
    Spoiler* _shapes_box;
    Spoiler* _preview_box;

    LinkedComboBox* _cell_combo;
    QComboBox* _integrator;
    SafeDoubleSpinBox* _d_min;
    SafeDoubleSpinBox* _d_max;
    QCheckBox* _direct_beam;
    QPushButton* _refine_ki_button;

    QPushButton* _save_button;
    QPushButton* _predict_button;

    LinkedComboBox* _exp_combo;

    PeakViewWidget* _peak_view_widget;
    DetectorWidget* _detector_widget;
    PeakTableView* _peak_table;

    LinkedComboBox* _found_peaks_combo;
    SafeSpinBox* _nx;
    SafeSpinBox* _ny;
    SafeSpinBox* _nz;
    QCheckBox* _kabsch;
    SafeDoubleSpinBox* _sigma_m;
    SafeDoubleSpinBox* _sigma_d;
    SafeDoubleSpinBox* _min_strength;
    SafeDoubleSpinBox* _min_d;
    SafeDoubleSpinBox* _max_d;
    SafeDoubleSpinBox* _peak_end;
    SafeDoubleSpinBox* _bkg_begin;
    SafeDoubleSpinBox* _bkg_end;
    SafeDoubleSpinBox* _radius_pix;
    SafeDoubleSpinBox* _radius_frames;
    SafeSpinBox* _min_neighbours;
    QComboBox* _interpolation_combo;
    QPushButton* _assign_peak_shapes; 
};

#endif // NSX_GUI_SUBFRAME_PREDICT_SUBFRAMEPREDICTPEAKS_H
