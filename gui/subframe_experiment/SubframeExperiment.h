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
#include "gui/utility/DataComboBox.h"
#include "gui/utility/SafeSpinBox.h"

#include <QWidget>
#include <qcheckbox.h>
#include <qspinbox.h>

class DetectorWidget;
class PlotPanel;
class PropertyPanel;
class QPushButton;
class QCheckBox;
class Spoiler;

//! Frame containing information on all aspects of the experiment
class SubframeExperiment : public QWidget {
 public:
    SubframeExperiment();

    DetectorWidget* detectorWidget();

    void refreshAll();

    PlotPanel* getPlot() { return _plot; };
    PropertyPanel* getProperty() { return _properties; };

    void plotIntensities();
    void toggleUnsafeWidgets();
    void updateRanges();

    int getNumberDataPoints() {return _lineplot_number_current->value();};

 private:
    std::vector<ohkl::sptrDataSet> _data_list;
    DetectorWidget* _detector_widget;
    PlotPanel* _plot;
    PropertyPanel* _properties;
    
    QPushButton* _calc_intensity;
    QPushButton* _update_plot;
    QCheckBox* _totalHistogram;
    QCheckBox* _yLog;
    QCheckBox* _xZoom;
    QCheckBox* _yZoom;
    QDoubleSpinBox* _intensity_number_current;
    QDoubleSpinBox* _lineplot_number_current;
    QDoubleSpinBox* _minX;
    QDoubleSpinBox* _maxX;
    QDoubleSpinBox* _minY;
    QDoubleSpinBox* _maxY;

    Spoiler* lineplot_box;
    Spoiler* intensity_plot_box;
};

#endif // OHKL_GUI_SUBFRAME_EXPERIMENT_SUBFRAMEEXPERIMENT_H
