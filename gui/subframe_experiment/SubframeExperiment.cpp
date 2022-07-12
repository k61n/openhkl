//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_experiment/SubframeExperiment.cpp
//! @brief     Implements class SubframeExperiment
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_experiment/SubframeExperiment.h"

#include "core/data/DataSet.h"
#include "gui/models/Project.h"
#include "core/experiment/Experiment.h"
#include "gui/models/Session.h"
#include "gui/utility/Spoiler.h"
#include "gui/utility/SafeSpinBox.h"
#include "gui/widgets/PlotPanel.h"
#include "gui/widgets/DetectorWidget.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <cstring>
#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <gsl/gsl_histogram.h>
#include <qnamespace.h>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QVector>
#include <QWidget>

#include "gui/utility/Spoiler.h"
#include "gui/utility/GridFiller.h"

SubframeExperiment::SubframeExperiment()
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    QSplitter* splitter = new QSplitter(this);

    QWidget* left_widget = new QWidget();
    left_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    QVBoxLayout* left_layout = new QVBoxLayout;

    Spoiler* intensity_plot_box = new Spoiler("Intensity Plot");
    GridFiller f(intensity_plot_box, true);

    int nMaxBins = 100000;

    _data_combo = f.addDataCombo("Dataset: ","Select dataset");

    _number_bins = new QSlider(Qt::Horizontal);
    QLabel* label = new QLabel("Number of bins: ");
    _number_bins_current = new QDoubleSpinBox();
    _number_bins_current->setMaximumWidth(100);
    _number_bins_current->setMinimum(1);
    _number_bins_current->setMaximum(nMaxBins);
    _number_bins->setMaximumWidth(250);
    _number_bins_current->setDecimals(0);

    _number_bins_current->setValue(1000);
    _number_bins->setValue(1000);

    QWidget* bin_selector_widget = new QWidget;
    QHBoxLayout* hbox = new QHBoxLayout(bin_selector_widget);

    hbox->addWidget(label);
    hbox->addWidget(_number_bins);
    hbox->addWidget(_number_bins_current);

    f.addWidget(bin_selector_widget,0);

    _calc_intensity = f.addButton("Calculate intensity");

    _frame_selector = new QSlider(Qt::Horizontal);
    label = new QLabel("Frame: ");
    _frame_selector_curent = new QDoubleSpinBox();
    _frame_selector->setMaximumWidth(100);
    _frame_selector->setMaximumWidth(250);
    _frame_selector_curent->setDecimals(0);

    QWidget* frame_selector_widget = new QWidget;
    hbox = new QHBoxLayout(frame_selector_widget);

    hbox->addWidget(label);
    hbox->addWidget(_frame_selector);
    hbox->addWidget(_frame_selector_curent);

    f.addWidget(frame_selector_widget,0);
    _totalHistogram = f.addCheckBox("Show total histogram", 1);
    _yLog = f.addCheckBox("yLog", 1);
    _xZoom = f.addCheckBox("Zoom on X axis", 1);
    _yZoom = f.addCheckBox("Zoom on Y axis", 1);

    left_layout->addWidget(intensity_plot_box);
    left_layout->addStretch();
    intensity_plot_box->setMaximumWidth(350);

    _number_bins->setMinimumWidth(1);
    _number_bins->setMaximum(nMaxBins);
    _number_bins->setValue(100);

    _minX = f.addDoubleSpinBox("minX:");
    _maxX = f.addDoubleSpinBox("maxX:");
    _minY = f.addDoubleSpinBox("minY:");
    _maxY = f.addDoubleSpinBox("maxY:");

    _update_plot = f.addButton("Update plot");

    _minY->setDecimals(0);
    _minX->setDecimals(0);
    _maxY->setDecimals(0);
    _maxX->setDecimals(0);

    _plot = new PlotPanel;

    QGroupBox* figure_group = new QGroupBox("Preview");
    figure_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _detector_widget = new DetectorWidget(true, false, true, figure_group);
    _detector_widget->modeCombo()->addItems(QStringList{
            "Zoom", "Selection box", "Rectangular mask", "Elliptical mask",
            "Line plot", "Horizontal slice", "Vertical slice"});

    QWidget* right_widget = new QWidget(this);
    right_widget->setLayout(_detector_widget);
    left_widget->setLayout(left_layout);

    QSplitter* right_splitter = new QSplitter();
    right_splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    right_splitter->setOrientation(Qt::Orientation::Vertical);
    right_splitter->setChildrenCollapsible(false);
    right_splitter->addWidget(right_widget);
    right_splitter->addWidget(_plot);

    splitter->addWidget(left_widget);
    splitter->addWidget(right_splitter);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 4);
    splitter->setChildrenCollapsible(false);

    layout->addWidget(splitter);

    connect(
        _data_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
            [=](){
                toggleUnsafeWidgets();
                plotIntensities();
            }
        );

    connect(_totalHistogram, &QCheckBox::clicked, this,
        [=](){
            toggleUnsafeWidgets();
            plotIntensities();
        }
    );

    connect(_xZoom, &QCheckBox::clicked, this,
        [=](){
            toggleUnsafeWidgets();
        }
    );

    connect(_yZoom, &QCheckBox::clicked, this,
        [=](){
             toggleUnsafeWidgets();
        }
    );

    connect(_yLog, &QCheckBox::clicked, this,
        [=](){
            _plot->setYLog(_yLog->isChecked());
            plotIntensities();
        }
    );

    connect(_calc_intensity, &QPushButton::clicked, this,
        [=](){
            nsx::Experiment* expt = gSession->currentProject()->experiment();
            auto data = expt->getDataMap()->at(_data_combo->currentText().toStdString());
            bool hasHistograms = data->getNumberHistograms() > 0;

            if (!data) return;
            if (hasHistograms) data->clearHistograms();
            data->getIntensityHistogram(_number_bins->value());
            _frame_selector->setMaximum(data->getNumberHistograms());
            _frame_selector_curent->setMaximum(data->getNumberHistograms());

            _maxX->setMaximum(data->nCols()*data->nRows());
            _minX->setMaximum(data->nCols()*data->nRows()-1);
            _maxY->setMaximum(10000000);
            _minY->setMaximum(10000000);

            toggleUnsafeWidgets();
        }
    );

    connect(_number_bins, &QSlider::valueChanged, this,
        [=](){
            _number_bins->setMaximum(10000);
            _number_bins_current->setValue(_number_bins->value());
        }
     );

    connect(_frame_selector, &QSlider::valueChanged, this,
        [=](){
            plotIntensities();
        }
    );

    connect(_frame_selector_curent, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
        [=](){
            _frame_selector->setValue(_frame_selector_curent->value());
            SubframeExperiment::plotIntensities();
        }
    );

    connect(_number_bins_current, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
        [=](){
           _number_bins->setValue(_number_bins_current->value());
        }
    );


    connect(_update_plot, &QPushButton::clicked, this,
        [=](){
            plotIntensities();
        }
    );

    toggleUnsafeWidgets();
}

void SubframeExperiment::plotIntensities()
{
    _frame_selector_curent->setValue(_frame_selector->value());
    nsx::Experiment* expt = gSession->currentProject()->experiment();
    auto data = expt->getDataMap()->at(_data_combo->currentText().toStdString());

    if (!data) return;

    gsl_histogram* histo = nullptr;

    if (!_totalHistogram->isChecked())
        histo = data->getHistogram(_frame_selector->value());
    else
        histo = data->getTotalHistogram();

    if (!histo)
        return;

    int xmin = _minX->value();
    int ymin = _minY->value();
    int xmax = _maxX->value();
    int ymax = _maxY->value();

    if (!_xZoom->isChecked()){
        xmin = -1;
        xmax = -1;
    }
    if (!_yZoom->isChecked()){
        ymin = -1;
        ymax = -1;
    }

    getPlot()->
        plotHistogram(histo->n, histo->range, histo->bin,
        QString(""), QString(""),
        xmin, xmax, ymin, ymax);
}

void SubframeExperiment::refreshAll()
{
    if (!gSession->hasProject())
        return;

    _detector_widget->refresh();
    toggleUnsafeWidgets();
}

DetectorWidget* SubframeExperiment::detectorWidget()
{
    return _detector_widget;
}

void SubframeExperiment::toggleUnsafeWidgets()
{
    _calc_intensity->setEnabled(false);
    _frame_selector->setEnabled(false);
    _frame_selector_curent->setEnabled(false);
    _yLog->setEnabled(false);
    _yZoom->setEnabled(false);
    _xZoom->setEnabled(false);
    _minX->setEnabled(false);
    _minY->setEnabled(false);
    _maxX->setEnabled(false);
    _maxY->setEnabled(false);
    _update_plot->setEnabled(false);
    _totalHistogram->setEnabled(false);

    bool hasProject = gSession->hasProject();

    if (!hasProject) return;
    bool hasData = gSession->currentProject()->hasDataSet();
    bool showTotalHistogram = _totalHistogram->isChecked();

    _calc_intensity->setEnabled(hasData);

    nsx::Experiment* expt = gSession->currentProject()->experiment();
    auto data = expt->getDataMap()->at(_data_combo->currentText().toStdString());
    auto nHistograms = data->getNumberHistograms();

    bool hasHistograms = data->getNumberHistograms() > 0;

    _frame_selector->setEnabled(hasHistograms && !showTotalHistogram);
    _frame_selector_curent->setEnabled(hasHistograms && !showTotalHistogram);
    _yLog->setEnabled(hasHistograms);
    _yZoom->setEnabled(hasHistograms);
    _xZoom->setEnabled(hasHistograms);
    _totalHistogram->setEnabled(hasHistograms);

    _minX->setEnabled(_xZoom->isChecked());
    _minY->setEnabled(_yZoom->isChecked());
    _maxX->setEnabled(_xZoom->isChecked());
    _maxY->setEnabled(_yZoom->isChecked());
    _update_plot->setEnabled(hasHistograms);
}
