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

#include "core/algo/AutoIndexer.h"
#include "core/convolve/Convolver.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/PeakFinder2D.h"
#include "core/peak/Qs2Events.h"
#include "gui/MainWin.h" // gGui
#include "gui/connect/Sentinel.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/DataComboBox.h"
#include "gui/utility/SafeSpinBox.h"
#include "gui/utility/Spoiler.h"
#include "gui/utility/SpoilerCheck.h"
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
    : QWidget()
    , _show_direct_beam(true)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    QSplitter* splitter = new QSplitter(this);

    QWidget* left_widget = new QWidget();
    left_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    _left_layout = new QVBoxLayout;

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

    _left_layout->addWidget(intensity_plot_box);
    _left_layout->addWidget(lineplot_box);
    _left_layout->addStretch();

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

    left_widget->setLayout(_left_layout);
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

    setAdjustBeamUp();
    setPeakFinder2DUp();
    setIndexerUp();

    connect(
        _detector_widget->scene(), &DetectorScene::beamPosChanged, this,
        &SubframeExperiment::onBeamPosChanged);
    connect(
        this, &SubframeExperiment::beamPosChanged, _detector_widget->scene(),
        &DetectorScene::setBeamSetterPos);
    connect(
        this, &SubframeExperiment::crosshairChanged, _detector_widget->scene(),
        &DetectorScene::onCrosshairChanged);
    connect(
        _data_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        _detector_widget->dataCombo(), &QComboBox::setCurrentIndex);
    connect(
        _detector_widget->dataCombo(), QOverload<int>::of(&QComboBox::currentIndexChanged),
        _data_combo, &QComboBox::setCurrentIndex);

    _set_initial_ki->setChecked(false);
    toggleUnsafeWidgets();
}

void SubframeExperiment::setAdjustBeamUp()
{
    _set_initial_ki = new SpoilerCheck("Set initial direct beam position");
    GridFiller f(_set_initial_ki, true);

    _beam_offset_x = f.addDoubleSpinBox("x offset", "Direct beam offset in x direction (pixels)");

    _beam_offset_y = f.addDoubleSpinBox("y offset", "Direct beam offset in y direction (pixels)");

    _crosshair_size = new QSlider(Qt::Horizontal);
    QLabel* crosshair_label = new QLabel("Crosshair size");
    crosshair_label->setToolTip("Radius of crosshair (pixels)");
    crosshair_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _crosshair_size->setMinimum(5);
    _crosshair_size->setMaximum(200);
    _crosshair_size->setValue(15);
    f.addLabel("Crosshair size", "Radius of crosshair (pixels)");
    f.addWidget(_crosshair_size, 1);

    _crosshair_linewidth = f.addSpinBox("Crosshair linewidth", "Line width of crosshair");

    _beam_offset_x->setValue(0.0);
    _beam_offset_x->setMaximum(1000.0);
    _beam_offset_x->setMinimum(-1000.0);
    _beam_offset_x->setDecimals(4);
    _beam_offset_y->setValue(0.0);
    _beam_offset_y->setMaximum(1000.0);
    _beam_offset_y->setMinimum(-1000.0);
    _beam_offset_y->setDecimals(4);
    _crosshair_linewidth->setValue(2);
    _crosshair_linewidth->setMinimum(1);
    _crosshair_linewidth->setMaximum(10);

    connect(
        _set_initial_ki->checkBox(), &QCheckBox::stateChanged, this,
        &SubframeExperiment::refreshVisual);
    connect(
        _set_initial_ki->checkBox(), &QCheckBox::stateChanged, this,
        &SubframeExperiment::toggleCursorMode);
    connect(
        _beam_offset_x,
        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
        &SubframeExperiment::onBeamPosSpinChanged);
    connect(
        _beam_offset_y,
        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
        &SubframeExperiment::onBeamPosSpinChanged);
    connect(
        _crosshair_size, static_cast<void (QSlider::*)(int)>(&QSlider::valueChanged), this,
        &SubframeExperiment::changeCrosshair);
    connect(
        _crosshair_linewidth, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &SubframeExperiment::changeCrosshair);

    _detector_widget->scene()->linkDirectBeamPositions(&_direct_beam_events);
    _detector_widget->scene()->linkOldDirectBeamPositions(&_old_direct_beam_events);

    _left_layout->addWidget(_set_initial_ki);
}

void SubframeExperiment::setPeakFinder2DUp()
{
    Spoiler* peak2D_spoiler = new Spoiler("Find blobs in this image");
    GridFiller gfiller(peak2D_spoiler, true);

    _data_combo = gfiller.addDataCombo("Data set");
    _convolver_combo = gfiller.addCombo(
        "Convolution kernel", "Convolver kernel type to use in image filtering");
    _threshold = gfiller.addSpinBox(
        "Filtered image threshold", "Minimum counts to use in image thresholding");
    _blob_min_thresh = gfiller.addSpinBox(
        "Minimum blob threshold", "Minimum threshold for blob detection");
    _blob_max_thresh = gfiller.addSpinBox(
        "Maximum blob threshold", "Maximum threshold for blob detection");
    _find_peaks_2d = gfiller.addButton("Find spots", "Find detector spots in current image");

    auto kernel_types = ohkl::Convolver::kernelTypes;
    for (auto it = kernel_types.begin(); it != kernel_types.end(); ++it)
        _convolver_combo->addItem(QString::fromStdString(it->second));
    _convolver_combo->setCurrentIndex(1);

    _blob_min_thresh->setMaximum(256);
    _blob_max_thresh->setMaximum(256);

    _blob_min_thresh->setValue(1);
    _blob_max_thresh->setValue(100);

    connect(
        _data_combo, &QComboBox::currentTextChanged, this, &SubframeExperiment::toggleUnsafeWidgets);
    connect(
        _data_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        _detector_widget->dataCombo(), &QComboBox::setCurrentIndex);
    connect(
        _find_peaks_2d, &QPushButton::clicked, this, &SubframeExperiment::find_2d);

    _left_layout->addWidget(peak2D_spoiler);
}

void SubframeExperiment::setIndexerUp()
{
    Spoiler* index_spoiler = new Spoiler("Autoindex using spots in this image");
    GridFiller gfiller(index_spoiler, true);
    _index_button = gfiller.addButton("Autoindex", "Attempt to find a unit cell using spots in this image");

    connect(_index_button, &QPushButton::clicked, this, &SubframeExperiment::autoindex);

    _left_layout->addWidget(index_spoiler);
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
    _maxY->setMaximum(1e+9);
    _minY->setMaximum(1e+9-1);

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
        histo = data->getHistogram(_detector_widget->scroll()->value() - 1);
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
        histo = data->getHistogram(_detector_widget->scroll()->value() - 1);
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
        throw std::runtime_error(
            "SubframeExperiment::plotIntensities received invalid arrays for gsl_histogram");

    auto plot = getPlot();
    if (plot != nullptr)
        plot->plotData(histo, QString("Pixels"), QString("Counts"), xmin, xmax, ymin, ymax);
}

void SubframeExperiment::refreshVisual()
{
    auto scene = _detector_widget->scene();

    if (_set_initial_ki->isChecked()) {
        scene->addBeamSetter(_crosshair_size->value(), _crosshair_linewidth->value());
        changeCrosshair();
    }
    showDirectBeamEvents();
    _detector_widget->refresh();
}

void SubframeExperiment::refreshAll()
{
    if (!gSession->hasProject())
        return;

    _data_combo->refresh();
    _detector_widget->refresh();
    grabFinderParameters();
    toggleUnsafeWidgets();

    if (!gSession->currentProject()->hasDataSet())
        return;
    updateRanges();
    plotIntensities();
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

    if (!gSession->currentProject()->hasDataSet())
        return;
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

void SubframeExperiment::find_2d()
{
    ohkl::Experiment* expt = gSession->currentProject()->experiment();
    ohkl::PeakFinder2D* finder = expt->peakFinder2D();
    ohkl::sptrDataSet data = _data_combo->currentData();
    int frame = _detector_widget->scene()->currentFrame();

    finder->setData(data);
    _detector_widget->scene()->linkPerFrameSpots(finder->keypoints());

    setFinderParameters();

    finder->find(frame);

    _detector_widget->refresh();
}

void SubframeExperiment::autoindex()
{
    ohkl::Experiment* expt = gSession->currentProject()->experiment();
    ohkl::PeakFinder2D* finder = expt->peakFinder2D();
    ohkl::AutoIndexer* indexer = expt->autoIndexer();

    std::size_t current_frame = _detector_widget->scene()->currentFrame();
    std::vector<ohkl::Peak3D*> peaks = finder->getPeakList(current_frame);

    indexer->autoIndex(peaks);
    std::cout << indexer->solutionsToString() << std::endl;
}

void SubframeExperiment::grabFinderParameters()
{
    if (!gSession->hasProject())
        return;

    auto* finder = gSession->currentProject()->experiment()->peakFinder2D();
    auto* params = finder->parameters();

    _blob_min_thresh->setValue(params->minThreshold);
    _blob_max_thresh->setValue(params->maxThreshold);
    _threshold->setValue(params->threshold);
    _convolver_combo->setCurrentIndex(static_cast<int>(params->kernel));
}

void SubframeExperiment::setFinderParameters()
{
    if (!gSession->hasProject())
        return;

    auto* finder = gSession->currentProject()->experiment()->peakFinder2D();
    auto* params = finder->parameters();

    params->minThreshold = _blob_min_thresh->value();
    params->maxThreshold = _blob_max_thresh->value();
    params->threshold = _threshold->value();
    params->kernel = static_cast<ohkl::ConvolutionKernelType>(_convolver_combo->currentIndex());
}

void SubframeExperiment::onBeamPosChanged(QPointF pos)
{
    const QSignalBlocker blocker(this);
    auto data = _detector_widget->currentData();
    _beam_offset_x->setValue(pos.x() - (static_cast<double>(data->nCols()) / 2.0));
    _beam_offset_y->setValue(-pos.y() + (static_cast<double>(data->nRows()) / 2.0));
}

void SubframeExperiment::onBeamPosSpinChanged()
{
    auto data = _detector_widget->currentData();
    double x = _beam_offset_x->value() + static_cast<double>(data->nCols()) / 2.0;
    double y = -_beam_offset_y->value() + static_cast<double>(data->nRows()) / 2.0;
    emit beamPosChanged({x, y});
}

void SubframeExperiment::changeCrosshair()
{
    emit crosshairChanged(_crosshair_size->value(), _crosshair_linewidth->value());
}

void SubframeExperiment::toggleCursorMode()
{
    if (_set_initial_ki->isChecked()) {
        _stored_cursor_mode = _detector_widget->scene()->mode();
        _detector_widget->scene()->changeInteractionMode(7);
    } else {
        _detector_widget->scene()->changeInteractionMode(_stored_cursor_mode);
    }
}

void SubframeExperiment::setInitialKi(ohkl::sptrDataSet data)
{
    const auto* detector = data->diffractometer()->detector();
    const auto coords = _detector_widget->scene()->beamSetterCoords();

    ohkl::DirectVector direct = detector->pixelPosition(coords.x(), coords.y());
    for (ohkl::InstrumentState& state : data->instrumentStates())
        state.adjustKi(direct);
    emit gGui->sentinel->instrumentStatesChanged();
}

void SubframeExperiment::showDirectBeamEvents()
{
    if (!_show_direct_beam)
        return;

    _detector_widget->scene()->showDirectBeam(true);
    auto data_name = _detector_widget->dataCombo()->currentText().toStdString();
    if (data_name.empty()) {
        return;
    }
    const auto data = _detector_widget->currentData();

    _direct_beam_events.clear();
    const auto& states = data->instrumentStates();
    auto* detector = data->diffractometer()->detector();
    std::vector<ohkl::DetectorEvent> events = ohkl::algo::getDirectBeamEvents(states, *detector);

    for (auto&& event : events)
        _direct_beam_events.push_back(event);
}
