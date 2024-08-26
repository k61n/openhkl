//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_predict/SubframePredict.h
//! @brief     Defines classes FoundPeaks, PeakFinderFrame
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_SUBFRAME_PREDICT_SUBFRAMEPREDICT_H
#define OHKL_GUI_SUBFRAME_PREDICT_SUBFRAMEPREDICT_H

#include "core/algo/Refiner.h"
#include "core/data/DataTypes.h"
#include "core/detector/DetectorEvent.h"
#include "core/shape/PeakCollection.h"
#include "gui/items/PeakCollectionItem.h"
#include "gui/models/PeakCollectionModel.h"

#include <QWidget>

class CellComboBox;
class DataComboBox;
class DetectorWidget;
class DirectBeamWidget;
class PeakComboBox;
class PeakItemGraphic;
class PeakTableView;
class PeakViewWidget;
class QCheckBox;
class QComboBox;
class QPushButton;
class QSplitter;
class QVBoxLayout;
class SafeSpinBox;
class SafeDoubleSpinBox;
class Spoiler;
class ShapeComboBox;
enum class ComboType;

namespace ohkl {
struct ShapeModelParameters;
}

//! Frame containing interface for predicting peaks from unit cell
class SubframePredict : public QWidget {
    Q_OBJECT
 public:
    SubframePredict();
    //! Show direct beam position computed from unit cell in DetectorScene
    void showDirectBeamEvents();
    //! Refresh all the panels
    void refreshAll();
    //! Get a pointer to the detector widget
    DetectorWidget* detectorWidget();
    //! Get prediction parameters
    void grabPredictorParameters();
    //! Get refiner parameters
    void grabRefinerParameters();
    //! Get shape collection parameters
    void grabShapeModelParameters();
    //! Set refiner parameters
    void setRefinerParameters();
    //! Set prediction parameters
    void setPredictorParameters();
    //! Set shape collection parameters
    void setShapeModelParameters();
    //! Refine the incident wavevector

 private:
    //! Manually set the incident wavevector
    void setAdjustBeamUp();
    //! Set the incident wavevector refinement up
    void setRefineKiUp();
    //! Set the parameters up
    void setParametersUp();
    //! Set the shape collection construction update
    void setShapeModelUp();
    //! Build the buttons
    void setProceedUp();
    //! Set the peak table view up
    void setPeakTableUp();
    //! Set the controls to show/hide peaks in detector scene
    void setPreviewUp();
    //! Set the save button up
    void setSaveUp();

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
    void applyShapeModel();
    //! Accept and save current list
    void accept();
    //! Disable unsafe widgets if no data loaded
    void toggleUnsafeWidgets();
    //! Toggle cursor mode
    void toggleCursorMode();
    //! Transmit crosshair changes to DetectorScene
    void changeCrosshair();

    //! The model for the found peaks
    ohkl::PeakCollection _peak_collection;
    //! The temporary collection
    PeakCollectionItem _peak_collection_item;
    //! The temporary collection
    PeakCollectionModel _peak_collection_model;
    //! Flag to check whether peaks have been predicted
    bool _peaks_predicted;
    //! Flag to check whether shapes have been assigned to predicted peaks
    bool _shapes_assigned;
    //! Saved direct beam positions
    std::vector<ohkl::DetectorEvent> _direct_beam_events;
    //! Current direct beam positions
    std::vector<ohkl::DetectorEvent> _old_direct_beam_events;


    QVBoxLayout* _left_layout;
    QSplitter* _right_element;

    Spoiler* _set_initial_ki;
    DirectBeamWidget* _beam_setter_widget;

    DataComboBox* _data_combo;
    PeakComboBox* _peak_combo;
    SafeSpinBox* _n_batches_spin;
    SafeSpinBox* _max_iter_spin;
    QComboBox* _residual_combo;
    QCheckBox* _direct_beam;
    QPushButton* _refine_ki_button;

    CellComboBox* _cell_combo;
    QComboBox* _integrator;
    SafeDoubleSpinBox* _d_min;
    SafeDoubleSpinBox* _d_max;

    QPushButton* _save_button;
    QPushButton* _predict_button;

    PeakViewWidget* _peak_view_widget;
    DetectorWidget* _detector_widget;
    PeakTableView* _peak_table;

    ShapeComboBox* _shape_combo;
    SafeDoubleSpinBox* _radius_pix;
    SafeDoubleSpinBox* _radius_frames;
    QComboBox* _interpolation_combo;
    QPushButton* _apply_shape_model;

    int _stored_cursor_mode;

    // Convert enum class ResidualType to a string
    const std::map<std::string, ohkl::ResidualType> _residual_strings{
        {"Reciprocal space", ohkl::ResidualType::QSpace},
        {"Real space", ohkl::ResidualType::RealSpace}};
};

#endif // OHKL_GUI_SUBFRAME_PREDICT_SUBFRAMEPREDICT_H
