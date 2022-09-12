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
#include "gui/utility/SafeSpinBox.h"

#include <QWidget>

class DataComboBox;
class DetectorWidget;
class PlotPanel;
class PropertyPanel;
class QPushButton;
class QCheckBox;
class QVBoxLayout;
class Spoiler;

//! Frame containing information on all aspects of the experiment
class SubframeExperiment : public QWidget {
 public:
    SubframeExperiment();

    void setPeakFinder2DUp();

    DetectorWidget* detectorWidget();

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

    DataComboBox* _data_combo;
    QPushButton* _find_peaks_2d;
    SafeSpinBox* _min_thresh;
    SafeSpinBox* _max_thresh;

    Spoiler* lineplot_box;
    Spoiler* intensity_plot_box;
};

#endif // OHKL_GUI_SUBFRAME_EXPERIMENT_SUBFRAMEEXPERIMENT_H
