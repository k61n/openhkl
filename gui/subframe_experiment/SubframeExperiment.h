//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_experiment/SubframeExperiment.h
//! @brief     Defines class SubframeExperiment
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_SUBFRAME_EXPERIMENT_SUBFRAMEEXPERIMENT_H
#define OHKL_GUI_SUBFRAME_EXPERIMENT_SUBFRAMEEXPERIMENT_H

#include "core/data/DataTypes.h"
#include "core/detector/DetectorEvent.h"
#include "gui/utility/SafeSpinBox.h"
#include "core/convolve/Convolver.h"

#include <QWidget>

class DataComboBox;
class DetectorWidget;
class PlotPanel;
class PropertyPanel;
class QPushButton;
class QCheckBox;
class QComboBox;
class QSlider;
class QVBoxLayout;
class Spoiler;
class SpoilerCheck;

//! Frame containing information on all aspects of the experiment
class SubframeExperiment : public QWidget {
    Q_OBJECT
 public:
    SubframeExperiment();

    void setAdjustBeamUp();
    void setPeakFinder2DUp();

    DetectorWidget* detectorWidget();

    void refreshVisual();
    void refreshAll();

    PlotPanel* getPlot() { return _plot; };
    PropertyPanel* getProperty() { return _properties; };

    void find_2d();
    void plotIntensities();
    void toggleUnsafeWidgets();
    void updateRanges();

    int getNumberDataPoints() {return _npoints_lineplot->value();};
    void calculateIntensities();
    void setLogarithmicScale();

    void grabFinderParameters();
    void setFinderParameters();

    //! Transmit crosshair changes to DetectorScene
    void changeCrosshair();
    //! Toggle cursor mode
    void toggleCursorMode();
    //! Set the initial value of ki from the crosshair position
    void setInitialKi(ohkl::sptrDataSet data);
    //! Show direct beam position computed from unit cell in DetectorScene
    void showDirectBeamEvents();

 public slots:
    void onBeamPosChanged(QPointF pos);
    void onBeamPosSpinChanged();

 signals:
    void beamPosChanged(QPointF pos);
    void crosshairChanged(int size, int linewidth);

 private:
    DetectorWidget* _detector_widget;
    PlotPanel* _plot;
    PropertyPanel* _properties;

    QVBoxLayout* _left_layout;

    QPushButton* _calc_intensity;
    QPushButton* _update_plot;
    QCheckBox* _totalHistogram;
    QCheckBox* _yLog;
    QCheckBox* _xZoom;
    QCheckBox* _yZoom;
    QSpinBox*  _npoints_intensity;
    QSpinBox* _npoints_lineplot;
    QSpinBox* _minX;
    QSpinBox* _maxX;
    QSpinBox* _minY;
    QSpinBox* _maxY;

    SpoilerCheck* _set_initial_ki;
    QCheckBox* _direct_beam;
    SafeDoubleSpinBox* _beam_offset_x;
    SafeDoubleSpinBox* _beam_offset_y;
    QSlider* _crosshair_size;
    SafeSpinBox* _crosshair_linewidth;

    DataComboBox* _data_combo;
    QComboBox* _convolver_combo;
    QPushButton* _find_peaks_2d;
    SafeSpinBox* _threshold;
    SafeSpinBox* _blob_min_thresh;
    SafeSpinBox* _blob_max_thresh;

    Spoiler* lineplot_box;
    Spoiler* intensity_plot_box;

    bool _show_direct_beam;
    int _stored_cursor_mode;

    //! Saved direct beam positions
    std::vector<ohkl::DetectorEvent> _direct_beam_events;
    //! Current direct beam positions
    std::vector<ohkl::DetectorEvent> _old_direct_beam_events;

    static const std::map<ohkl::ConvolutionKernelType, std::string> _kernel_types;
};

#endif // OHKL_GUI_SUBFRAME_EXPERIMENT_SUBFRAMEEXPERIMENT_H
