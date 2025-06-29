//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/widgets/DetectorWidget.h
//! @brief     Defines class DetectorWidget
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_UTILITY_DETECTORWIDGET_H
#define OHKL_GUI_UTILITY_DETECTORWIDGET_H

#include "core/data/DataTypes.h"

#include <QGridLayout>

class DetectorScene;
class DetectorView;
class DataComboBox;
class PeakCollectionModel;
class PeakViewWidget;
class QButtonGroup;
class QPushButton;
class QSpinBox;
class QScrollBar;
class QComboBox;
class QSlider;
class QWidget;
enum class VisualisationType;

class DetectorWidget : public QGridLayout {

 public:
    DetectorWidget(
        std::size_t max_collections, bool cursor, bool slider, QWidget* parent = nullptr);
    //! Synchronise intensity sliders and intensities across instances
    void syncIntensitySliders();
    //! Update the DataSet combo
    void updateDatasetList(const std::vector<ohkl::sptrDataSet>& data_list);
    //! Refresh the DetectorScene
    void refresh();
    //! Link the detector scene to one or two peak models
    void linkPeakModel(PeakCollectionModel* model, PeakViewWidget* widget, std::size_t idx = 0);
    //! Set visualisation type for the subframe
    void setVisualisationMode(VisualisationType vtype);
    //! Return the current DataSet
    ohkl::sptrDataSet currentData();
    //! Switch the Detectorscene between coordinate types
    void changeView(int option);
    //! Set up the toolbar
    void setToolbarUp();

    void datasetChanged();

    //! Return the DetectorScene
    DetectorScene* scene();
    //! Return the frame QSpinBox
    QSpinBox* spin();
    //! Return the frame QScrollBar
    QScrollBar* scroll();
    //! Return the DataSet QComboBox
    DataComboBox* dataCombo();
    //! Return the cursor mode QComboBox
    QComboBox* cursorCombo();
    //! Return the maximum intensity slider
    QSlider* slider();
    //! Whether this instance has an intensity slider
    bool hasSlider();
    //! Return ptr to DetectorView
    DetectorView* getDetectorView();
    //! Save a screenshot via QFileDialog
    void saveScreenshot();
    //! Set the cursor mode
    void toggleCursorMode();
    //! Disable the cursor mode controls
    void enableCursorMode(bool enable);
    //! Toggle between image and image gradient
    void toggleGradient();
    //! Toggle masks
    void toggleMasks();
    //! Toggle Miller index labels
    void toggleLabels();
    //! Toggle resolution contours
    void toggleContours();

 private:
    DetectorView* _detector_view;
    QSpinBox* _spin;
    QScrollBar* _scroll;
    DataComboBox* _data_combo;
    QComboBox* _cursor_combo = nullptr;
    QSlider* _intensity_slider = nullptr;
    QPushButton* _gradient;
    QPushButton* _contours;
    QPushButton* _hide_masks;
    QPushButton* _peak_labels;
    QPushButton* _copy_to_clipboard;
    QPushButton* _save_to_file;
    QPushButton* _zoom;
    QPushButton* _select;
    QButtonGroup* _cursor_mode_buttons;
    QWidget* _toolbar;


    bool _has_slider = false;

    static QList<DetectorWidget*> _detector_widgets;
};

#endif // OHKL_GUI_UTILITY_DETECTORWIDGET_H
