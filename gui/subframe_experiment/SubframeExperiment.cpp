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

    intensity_plot_box = new Spoiler("Intensity Plot");
    lineplot_box = new Spoiler("Lineplot");

    GridFiller f(intensity_plot_box, true);
    GridFiller f2(lineplot_box, true);

    int nIntensityMaxPoints = 100000;
    int nIntensityMinPoints = 100;

    int nLineplotMaxPoints = 1000;
    int nLineplotMinPoints = 10;

    _intensity_number_datapoints = new QSlider(Qt::Horizontal);
    _intensity_number_current = new QDoubleSpinBox();

    _lineplot_number_datapoints = new QSlider(Qt::Horizontal);
    _lineplot_number_current = new QDoubleSpinBox();

    QWidget* ndatapoint_widget_intensity = new QWidget;
    QWidget* ndatapoint_widget_lineplot = new QWidget;

    QHBoxLayout* hbox = new QHBoxLayout(ndatapoint_widget_intensity);
    QHBoxLayout* hbox2 = new QHBoxLayout(ndatapoint_widget_lineplot);

    hbox->addWidget(new QLabel("Number of datapoints:"));
    hbox->addWidget(_intensity_number_datapoints);
    hbox->addWidget(_intensity_number_current);

    hbox2->addWidget(new QLabel("Number of datapoints:"));
    hbox2->addWidget(_lineplot_number_datapoints);
    hbox2->addWidget(_lineplot_number_current);

    f.addWidget(ndatapoint_widget_intensity,0);
    f2.addWidget(ndatapoint_widget_lineplot,0);

    _calc_intensity = f.addButton("Calculate intensity");

    _totalHistogram = f.addCheckBox("Show total histogram", 1);
    _yLog = f.addCheckBox("yLog", 1);
    _xZoom = f.addCheckBox("Zoom on X axis", 1);
    _yZoom = f.addCheckBox("Zoom on Y axis", 1);

    left_layout->addWidget(intensity_plot_box);
    left_layout->addWidget(lineplot_box);
    left_layout->addStretch();

    intensity_plot_box->setMaximumWidth(400);
    lineplot_box->setMaximumWidth(400);

    _intensity_number_current->setMaximumWidth(100);
    _intensity_number_current->setMaximum(nIntensityMaxPoints);
    _intensity_number_current->setMinimum(nIntensityMinPoints);
    _intensity_number_datapoints->setMaximumWidth(250);
    _intensity_number_current->setDecimals(0);
    _intensity_number_current->setValue(nIntensityMinPoints);
    _intensity_number_datapoints->setValue(nIntensityMinPoints);
    _intensity_number_datapoints->setMinimumWidth(1);
    _intensity_number_datapoints->setMaximum(nIntensityMaxPoints);
    _intensity_number_datapoints->setMinimum(nIntensityMinPoints);
    _intensity_number_datapoints->setValue(nIntensityMinPoints);

    _lineplot_number_datapoints->setMinimumWidth(1);
    _lineplot_number_datapoints->setMaximum(nLineplotMaxPoints);
    _lineplot_number_datapoints->setMinimum(nLineplotMinPoints);
    _lineplot_number_current->setMaximum(nLineplotMaxPoints);
    _lineplot_number_current->setMinimum(nLineplotMinPoints);
    _lineplot_number_datapoints->setValue(nLineplotMinPoints);

    _lineplot_number_current->setMaximumWidth(250);
    _lineplot_number_current->setMinimum(nLineplotMinPoints);
    _lineplot_number_datapoints->setMaximumWidth(250);
    _lineplot_number_current->setDecimals(0);
    _lineplot_number_current->setValue(nLineplotMinPoints);
    _lineplot_number_datapoints->setValue(nLineplotMinPoints);

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
               // auto plot = getPlot();
               // plot->clear();
            }
    );

    connect(
        _detector_widget->dataCombo(), QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
            [=](){
                updateRanges();
                plotIntensities();
                toggleUnsafeWidgets();
            }
    );

    connect(_totalHistogram, &QCheckBox::clicked, this,
        [=](){
            updateRanges();
            toggleUnsafeWidgets();
            plotIntensities();
        }
    );

    connect(_xZoom, &QCheckBox::clicked, this,
        [=](){
            updateRanges();
            toggleUnsafeWidgets();
        }
    );

    connect(_yZoom, &QCheckBox::clicked, this,
        [=](){
            updateRanges();
            toggleUnsafeWidgets();
        }
    );

    connect(_yLog, &QCheckBox::clicked, this,
        [=](){
            _plot->setYLog(_yLog->isChecked());
            updateRanges();
            plotIntensities();
        }
    );

    connect(_calc_intensity, &QPushButton::clicked, this,
        [=](){
            ohkl::Experiment* expt = gSession->currentProject()->experiment();
            auto data = expt->getDataMap()->at(_detector_widget->dataCombo()->currentText().toStdString());
            bool hasHistograms = data->getNumberHistograms() > 0;

            if (!data) return;
            if (hasHistograms) data->clearHistograms();
            data->getIntensityHistogram(_intensity_number_datapoints->value());

            _maxX->setMaximum(data->nCols()*data->nRows());
            _minX->setMaximum(data->nCols()*data->nRows()-1);
            _maxY->setMaximum(1e+100);
            _minY->setMaximum(1e+100-1);

            updateRanges();
            toggleUnsafeWidgets();
        }
    );

    connect(_intensity_number_datapoints, &QSlider::valueChanged, this,
        [=](){
            _intensity_number_current->setValue(_intensity_number_datapoints->value());
        }
    );

    connect(_lineplot_number_datapoints, &QSlider::valueChanged, this,
        [=](){
            _lineplot_number_current->setValue(_lineplot_number_datapoints->value());
        }
    );

    connect(_detector_widget->scroll(), &QScrollBar::valueChanged, this,
        [=](){
            updateRanges();
            plotIntensities();
        }
    );

    connect(_intensity_number_current, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
        [=](){
           _intensity_number_datapoints->setValue(_intensity_number_current->value());
        }
    );

    connect(_lineplot_number_current, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
        [=](){
           _lineplot_number_datapoints->setValue(_lineplot_number_current->value());
        }
    );

    connect(_update_plot, &QPushButton::clicked, this,
        [=](){
            updateRanges();
            plotIntensities();
        }
    );

    toggleUnsafeWidgets();
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
