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
#include "gui/widgets/PlotPanel.h"

#include <QComboBox>
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
#include <QScrollBar>
#include <stdexcept>

#include "gui/utility/Spoiler.h"
#include "gui/utility/GridFiller.h"

SubframeExperiment::SubframeExperiment()
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    QSplitter* splitter = new QSplitter(this);

    QWidget* left_widget = new QWidget();
    left_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    QVBoxLayout* left_layout = new QVBoxLayout;

    intensity_plot_box = new Spoiler("Per-pixel detector count histogram");
    lineplot_box = new Spoiler("Lineplot");

    GridFiller gfiller(intensity_plot_box, true);
    GridFiller gfiller2(lineplot_box, true);

    _npoints_intensity = gfiller.addSpinBox(QString("Number of datapoints:"));
    _npoints_lineplot = gfiller2.addSpinBox(QString("Number of datapoints:"));

    _calc_intensity = gfiller.addButton("Calculate intensity");

    _totalHistogram = gfiller.addCheckBox("Show total histogram", 1);
    _yLog = gfiller.addCheckBox("Use logarithmic y scale", 1);
    _xZoom = gfiller.addCheckBox("Range on x axis", 1);
    _yZoom = gfiller.addCheckBox("Range on y axis", 1);

    left_layout->addWidget(intensity_plot_box);
    left_layout->addWidget(lineplot_box);
    left_layout->addStretch();

    intensity_plot_box->setMaximumWidth(400);
    lineplot_box->setMaximumWidth(400);

    _npoints_intensity->setMaximumWidth(100);
    _npoints_intensity->setMaximum(65535);
    _npoints_intensity->setMinimum(100);    
    _npoints_intensity->setValue(100); 

    _npoints_lineplot->setMaximum(1000);
    _npoints_lineplot->setMinimum(10); 
    _npoints_lineplot->setMaximumWidth(250); 
    _npoints_lineplot->setValue(10);
 
    _minX = gfiller.addSpinBox("Minimal x value:");
    _maxX = gfiller.addSpinBox("Maximum x value:");
    _minY = gfiller.addSpinBox("Minimal y value:");
    _maxY = gfiller.addSpinBox("Maximal y value:");

    _update_plot = gfiller.addButton("Update plot"); 

    _plot = new PlotPanel;

    QGroupBox* figure_group = new QGroupBox("Detector image");
    figure_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _detector_widget = new DetectorWidget(true, false, true, figure_group);
    _detector_widget->modeCombo()->addItems(QStringList{
            "Zoom", "Selection box", "Rectangular mask", "Elliptical mask",
            "Line plot", "Horizontal slice", "Vertical slice", "Intensity Histograms"});

    left_widget->setLayout(left_layout);
    left_widget->setFixedWidth(400);

    QSplitter* right_splitter = new QSplitter();
    right_splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    right_splitter->setOrientation(Qt::Orientation::Vertical);
    right_splitter->setChildrenCollapsible(false);
    right_splitter->addWidget(figure_group);
    right_splitter->addWidget(_plot);

    splitter->addWidget(left_widget);
    splitter->addWidget(right_splitter);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 4);
    splitter->setChildrenCollapsible(false);

    layout->addWidget(splitter);

    lineplot_box->setVisible(false);
    intensity_plot_box->setVisible(false);
    
    connect(
        _detector_widget->modeCombo(), QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
            [=](){
                lineplot_box->setVisible(_detector_widget->modeCombo()->currentIndex() == 4);
                intensity_plot_box->setVisible(_detector_widget->modeCombo()->currentIndex() == 7);
            }
    );

    connect(
        _detector_widget->dataCombo(), QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &SubframeExperiment::calculateIntensities);

    connect(_totalHistogram, &QCheckBox::clicked, this,
        &SubframeExperiment::refreshAll);

    connect(_xZoom, &QCheckBox::clicked, this,
        &SubframeExperiment::refreshAll);

    connect(_yZoom, &QCheckBox::clicked, this,
        &SubframeExperiment::refreshAll);

    connect(_yLog, &QCheckBox::clicked, this,
        &SubframeExperiment::setLogarithmicScale);

    connect(_calc_intensity, &QPushButton::clicked, this,
        &SubframeExperiment::calculateIntensities);

    connect(_detector_widget->scroll(), &QScrollBar::valueChanged,
    this, &SubframeExperiment::refreshAll);

    connect(_update_plot, &QPushButton::clicked, this,
    &SubframeExperiment::refreshAll);

    toggleUnsafeWidgets();
}

void SubframeExperiment::setLogarithmicScale()
{
    _plot->setYLog(_yLog->isChecked());
    updateRanges();
    plotIntensities();
}
 
void SubframeExperiment::calculateIntensities()
{
    ohkl::Experiment* expt = gSession->currentProject()->experiment();
    auto data = expt->getDataMap()->at(_detector_widget->dataCombo()->currentText().toStdString());
    bool hasHistograms = data->getNumberHistograms() > 0;

    if (!data) return;
    if (hasHistograms) data->clearHistograms();
    data->getIntensityHistogram(_npoints_intensity->value());

    _maxX->setMaximum(data->nCols()*data->nRows());
    _minX->setMaximum(data->nCols()*data->nRows()-1);
    _maxY->setMaximum(1e+100);
    _minY->setMaximum(1e+100-1);

    updateRanges();
    toggleUnsafeWidgets();
    plotIntensities();
}

void SubframeExperiment::updateRanges()
{
    ohkl::Experiment* expt = gSession->currentProject()->experiment();
    auto data = expt->getDataMap()->at(_detector_widget->dataCombo()->currentText().toStdString());

    gsl_histogram* histo = nullptr;

    if (!_totalHistogram->isChecked())
        histo = data->getHistogram(_detector_widget->scroll()->value());
    else
        histo = data->getTotalHistogram();

    if (!histo) return;

    if (!_xZoom->isChecked()){
        _minX->setValue(0);
        _maxX->setValue(data->maxCount());
    }
    if (!_yZoom->isChecked()){
        double max_element = *(std::max_element(histo->bin, histo->bin + histo->n*8));

        _minY->setValue(0);
        _maxY->setValue(max_element);
    }
}

void SubframeExperiment::plotIntensities()
{
    ohkl::Experiment* expt = gSession->currentProject()->experiment();
    auto data = expt->getDataMap()->at(_detector_widget->dataCombo()->currentText().toStdString());

    if (!data) return;

    gsl_histogram* histo = nullptr;

    if (!_totalHistogram->isChecked())
        histo = data->getHistogram(_detector_widget->scroll()->value());
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

    if (histo->range == nullptr || histo->bin == nullptr)
        throw std::runtime_error("SubframeExperiment::plotIntensities received invalid arrays for gsl_histogram");

    QVector<double> x;
    QVector<double> y;
    QVector<double> e;

    x.resize(histo->n);
    y.resize(histo->n);

    memcpy(x.data(), histo->range, histo->n * sizeof(double));
    memcpy(y.data(), histo->bin, histo->n * sizeof(double));

    auto plot = getPlot();
    if (plot != nullptr)
        plot->plotData(x, y, e, QString("Pixels"), QString("Counts"), xmin, xmax, ymin, ymax);
}

void SubframeExperiment::refreshAll()
{
    if (!gSession->hasProject())
        return;

    _detector_widget->refresh();
    updateRanges();
    plotIntensities();
    toggleUnsafeWidgets();
}

DetectorWidget* SubframeExperiment::detectorWidget()
{
    return _detector_widget;
}

void SubframeExperiment::toggleUnsafeWidgets()
{
    _calc_intensity->setEnabled(false);
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

    _calc_intensity->setEnabled(hasData);

    ohkl::Experiment* expt = gSession->currentProject()->experiment();
    auto data = expt->getDataMap()->at(_detector_widget->dataCombo()->currentText().toStdString());

    bool hasHistograms = data->getNumberHistograms() > 0;

    _yLog->setEnabled(hasHistograms);
    _yZoom->setEnabled(hasHistograms);
    _xZoom->setEnabled(hasHistograms);
    _totalHistogram->setEnabled(hasHistograms);

    _minX->setEnabled(_xZoom->isChecked() && _xZoom->isEnabled());
    _minY->setEnabled(_yZoom->isChecked() && _yZoom->isEnabled());
    _maxX->setEnabled(_xZoom->isChecked() && _xZoom->isEnabled());
    _maxY->setEnabled(_yZoom->isChecked() && _yZoom->isEnabled());
    _update_plot->setEnabled(hasHistograms);
}
