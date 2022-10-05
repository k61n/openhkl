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
#include "gui/dialogs/UnitCellDialog.h"
#include "gui/frames/ProgressView.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/DataComboBox.h"
#include "gui/utility/PropertyScrollArea.h"
#include "gui/utility/SafeSpinBox.h"
#include "gui/utility/SideBar.h"
#include "gui/utility/Spoiler.h"
#include "gui/utility/SpoilerCheck.h"
#include "gui/views/UnitCellTableView.h"
#include "gui/widgets/DetectorWidget.h"
#include "gui/widgets/PlotPanel.h"
#include "gui/utility/PropertyScrollArea.h"

#include "core/experiment/MaskExporter.h"
#include "core/experiment/MaskImporter.h"

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QScrollBar>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QTabWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>
#include <QFileDialog>
#include <cstring>
#include <gsl/gsl_histogram.h>
#include <qabstractitemview.h>
#include <qsizepolicy.h>
#include <qspinbox.h>
#include <stdexcept>
#include <QMessageBox>

#include "gui/utility/Spoiler.h"
#include "gui/utility/GridFiller.h"

SubframeExperiment::SubframeExperiment()
    : QWidget()
    , _show_direct_beam(true)
{
    _main_layout = new QHBoxLayout(this);
    _left_layout = new QVBoxLayout();

    _tab_widget = new QTabWidget();
    QWidget* plot_tab = new QWidget(_tab_widget);
    QWidget* indexer_tab = new QWidget(_tab_widget);
    _tab_widget->addTab(plot_tab, "Plot");
    _tab_widget->addTab(indexer_tab, "Indexer solutions");
    _tab_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _tab_widget->sizePolicy().setHorizontalStretch(1);

    QHBoxLayout* plot_layout = new QHBoxLayout();
    QHBoxLayout* indexer_layout = new QHBoxLayout();

    _plot = new PlotPanel;
    plot_layout->addWidget(_plot);
    plot_tab->setLayout(plot_layout);

    _solution_table = new UnitCellTableView(this);
    indexer_layout->addWidget(_solution_table);
    indexer_tab->setLayout(indexer_layout);
    _solution_table->setModel(nullptr);

    plot_tab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    indexer_tab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QGroupBox* figure_group = new QGroupBox("Detector image");
    figure_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _detector_widget = new DetectorWidget(true, true, figure_group);

    QSplitter* right_splitter = new QSplitter();
    right_splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    right_splitter->setOrientation(Qt::Orientation::Vertical);
    right_splitter->setChildrenCollapsible(false);
    right_splitter->addWidget(figure_group);
    right_splitter->addWidget(_tab_widget);

    setLeftWidgetUp();
    setStrategyUp();
    setHistogramUp();
    setMaskUp();

    auto propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_left_layout);
    _main_layout->addWidget(propertyScrollArea);

    _main_layout->addWidget(right_splitter);

    connect(
        _solution_table->verticalHeader(), &QHeaderView::sectionClicked, this,
        &SubframeExperiment::selectSolutionHeader);

    connect(
        _solution_table, &UnitCellTableView::clicked, this,
        &SubframeExperiment::selectSolutionTable);

    connect(
        _detector_widget->dataCombo(), QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &SubframeExperiment::calculateIntensities);

    connect(_detector_widget->scroll(), &QScrollBar::valueChanged,
        this, &SubframeExperiment::refreshAll);

    connect(_update_plot, &QPushButton::clicked, this, &SubframeExperiment::refreshAll);


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
    connect(
        _detector_widget->dataCombo(), QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SubframeExperiment::deselectAllMasks);

    _set_initial_ki->setChecked(false);
    _lineplot_box->setChecked(false);
    _mask_box->setChecked(false);
    toggleUnsafeWidgets();
}

void SubframeExperiment::setLeftWidgetUp()
{
    _left_widget = new QTabWidget();
    _left_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QWidget* strategy_tab = new QWidget(_left_widget);
    QWidget* histogram_tab = new QWidget(_left_widget);
    QWidget* mask_tab = new QWidget(_left_widget);

    _strategy_layout = new QVBoxLayout();
    _histogram_layout = new QVBoxLayout();
    _mask_layout = new QVBoxLayout();

    strategy_tab->setLayout(_strategy_layout);
    histogram_tab->setLayout(_histogram_layout);
    mask_tab->setLayout(_mask_layout);

    _left_widget->addTab(strategy_tab, "Strategy");
    _left_widget->addTab(histogram_tab, "Histograms");
    _left_widget->addTab(mask_tab, "Masks");
    _left_layout->addWidget(_left_widget);
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

    _strategy_layout->addWidget(_set_initial_ki);
}

void SubframeExperiment::setStrategyUp()
{
    setAdjustBeamUp();
    setPeakFinder2DUp();
    setIndexerUp();
    _strategy_layout->addStretch();
}

void SubframeExperiment::setHistogramUp()
{

    _intensity_plot_box = new Spoiler("Per-pixel detector count histogram");
    _intensity_plot_box->setMaximumWidth(400);
    GridFiller gfiller(_intensity_plot_box, true);

    _npoints_intensity = gfiller.addSpinBox(QString("Number of bins:"));

    _calc_intensity = gfiller.addButton("Calculate intensity");

    _totalHistogram = gfiller.addCheckBox("Show total histogram", 1);
    _yLog = gfiller.addCheckBox("Use logarithmic y scale", 1);
    _xZoom = gfiller.addCheckBox("Range on x axis", 1);
    _yZoom = gfiller.addCheckBox("Range on y axis", 1);

    _histogram_layout->addWidget(_intensity_plot_box);


    _npoints_intensity->setMaximumWidth(100);
    _npoints_intensity->setMaximum(65535);
    _npoints_intensity->setMinimum(100);
    _npoints_intensity->setValue(100);

    _minX = gfiller.addSpinBox("Minimum x value:");
    _maxX = gfiller.addSpinBox("Maximum x value:");
    _minY = gfiller.addSpinBox("Minimum y value:");
    _maxY = gfiller.addSpinBox("Maximum y value:");


    _update_plot = gfiller.addButton("Update plot");

    connect(_totalHistogram, &QCheckBox::clicked, this, &SubframeExperiment::refreshAll);

    connect(_xZoom, &QCheckBox::clicked, this, &SubframeExperiment::refreshAll);

    connect(_yZoom, &QCheckBox::clicked, this, &SubframeExperiment::refreshAll);

    connect(_yLog, &QCheckBox::clicked, this, &SubframeExperiment::setLogarithmicScale);

    connect(
        _calc_intensity, &QPushButton::clicked, this, &SubframeExperiment::calculateIntensities);

    _lineplot_box = new SpoilerCheck("Plot intensity profiles");
    _lineplot_box->setMaximumWidth(400);
    GridFiller gfiller2(_lineplot_box, true);

    _lineplot_combo = gfiller2.addCombo("Plot type");
    _lineplot_combo->addItems(QStringList{"Line plot", "Horizontal slice", "Vertical slice"});

    _npoints_lineplot = gfiller2.addSpinBox(QString("Number of bins:"));

    _npoints_lineplot->setMaximum(1000);
    _npoints_lineplot->setMinimum(10);
    _npoints_lineplot->setMaximumWidth(250);
    _npoints_lineplot->setValue(100);

    _histogram_layout->addWidget(_lineplot_box);
    _histogram_layout->addStretch();

    connect(
        _lineplot_box->checkBox(), &QCheckBox::stateChanged, this,
        &SubframeExperiment::toggleCursorMode);
    connect(
        _lineplot_combo, &QComboBox::currentTextChanged, this, &SubframeExperiment::toggleCursorMode);
}

void SubframeExperiment::setMaskUp()
{
    _mask_box = new SpoilerCheck("Add detector image masks");
    _mask_box->setMaximumWidth(400);
    GridFiller gfiller(_mask_box, true);

    _mask_combo = gfiller.addCombo("Mask type");
    _mask_combo->addItems(QStringList{"Rectangular mask", "Elliptical mask"});

    _mask_layout->addWidget(_mask_box);

    Spoiler* mask_table_box = new Spoiler("List of Masks");
    GridFiller gfiller2(mask_table_box, true);
    _mask_table = new QTableWidget(0, 5);
    _mask_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    _mask_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _mask_table->setHorizontalHeaderLabels(QStringList{"x lower", "y lower", "x upper", "y upper", "select"});

    gfiller2.addWidget(_mask_table, 0, 2);
    _mask_table->resizeColumnsToContents();

    _mask_layout->addWidget(mask_table_box);

    _import_masks = new QPushButton("Import masks");
    _export_masks = new QPushButton("Export masks");
    _delete_masks = new QPushButton("Delete masks");
    _toggle_selection = new QPushButton("Select all");

    QWidget* w = new QWidget();
    QHBoxLayout* left_bot = new QHBoxLayout();
    left_bot->addWidget(_import_masks);
    left_bot->addWidget(_export_masks);
    left_bot->addWidget(_delete_masks);
    left_bot->addWidget(_toggle_selection);
    w->setLayout(left_bot);
    gfiller2.addWidget(w, 0, 2);

    _mask_layout->addStretch();

    connect(
        _mask_box->checkBox(), &QCheckBox::stateChanged, this,
        &SubframeExperiment::toggleCursorMode);
    connect(
        _mask_combo, &QComboBox::currentTextChanged, this, &SubframeExperiment::toggleCursorMode);
    connect(
        _detector_widget->scene(), &DetectorScene::signalMaskChanged, this,
        &SubframeExperiment::refreshMaskTable);
    connect(
       _export_masks, &QPushButton::clicked, this,
        &SubframeExperiment::exportMasks);
    connect(
        _import_masks, &QPushButton::clicked, this,
        &SubframeExperiment::importMasks);
    connect(
        _delete_masks, &QPushButton::clicked, this,
        &SubframeExperiment::deleteSelectedMasks);
    connect(
        _toggle_selection, &QPushButton::clicked, this,
        &SubframeExperiment::selectAllMasks);
}

void SubframeExperiment::importMasks()
{
    QSettings settings = gGui->qSettings();
    settings.beginGroup("RecentDirectories");
    QString loadDirectory = settings.value("masks", QDir::homePath()).toString() +
    "/mask.yml";

    std::string file_path = QFileDialog::getOpenFileName(this, "Import masks from file", loadDirectory, "YAML (*.yml)").toStdString();

    if (file_path.empty()) return;

    ohkl::MaskImporter mimp(file_path);

    for (auto & e : mimp.getMasks())
        _data_combo->currentData()->addMask(e);

    _detector_widget->scene()->loadMasksFromData();
    toggleUnsafeWidgets();
}

void SubframeExperiment::exportMasks()
{
    QSettings settings = gGui->qSettings();
    settings.beginGroup("RecentDirectories");
    QString loadDirectory = settings.value("masks", QDir::homePath()).toString() +
    "/masks.yml";

    std::string file_path =
    QFileDialog::getSaveFileName(
        this, "Export maks to ", loadDirectory, "YAML (*.yml)").toStdString();

    if (file_path.empty()) return;

    ohkl::MaskExporter mexp(_data_combo->currentData()->masks());
    mexp.exportToFile(file_path);

    toggleUnsafeWidgets();
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
    _search_all_frames = gfiller.addCheckBox(
        "Search all images", "Find blobs in all images in this data set", 1);
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

    _strategy_layout->addWidget(peak2D_spoiler);
}

void SubframeExperiment::setIndexerUp()
{
    Spoiler* index_spoiler = new Spoiler("Autoindex using spots in this image");
    GridFiller gfiller(index_spoiler, true);
    _gruber = gfiller.addDoubleSpinBox("Gruber tolerance:", "Tolerance for Gruber reduction");

    _niggli = gfiller.addDoubleSpinBox("Niggli tolerance:", "Tolerance for Niggli reduction");

    _only_niggli = gfiller.addCheckBox("Find Niggli cell only", 1);

    _max_cell_dimension = gfiller.addDoubleSpinBox(
        "Max. Cell dimension:", QString::fromUtf8("(\u212B) - maximum length of any lattice vector"));

    _number_vertices = gfiller.addSpinBox(
        "Num. Q-space trial vectors:",
        "Number of points on reciprocal space unit sphere to test against candidate lattice "
        "vector");

    _number_subdivisions = gfiller.addSpinBox(
        "Num. FFT histogram bins:", "Number of histogram bins for Fast Fourier transform");

    _number_solutions = gfiller.addSpinBox("Number of solutions:", "Number of unit cell solutions to find");

    _min_cell_volume = gfiller.addDoubleSpinBox(
        "Minimum Volume:",
        QString::fromUtf8("(\u212B^3) - discard candidate cells below this volume"));

    _indexing_tolerance = gfiller.addDoubleSpinBox("Indexing tolerance:");

    _frequency_tolerance = gfiller.addDoubleSpinBox(
        "Frequency tolerance:",
        "Minimum fraction of amplitude of the zeroth Fourier frequency to accept as a candidate "
        "lattice vector");

    _index_button =
        gfiller.addButton("Autoindex", "Attempt to find a unit cell using spots in this image");
    _save_button = gfiller.addButton("Save unit cell", "Save the selected unit cell");

    _gruber->setMaximum(10);
    _gruber->setDecimals(6);

    _niggli->setMaximum(10);
    _niggli->setDecimals(6);

    _max_cell_dimension->setMaximum(1000);
    _max_cell_dimension->setDecimals(2);

    _number_vertices->setMaximum(100000);

    _number_solutions->setMaximum(1000);

    _min_cell_volume->setMaximum(1000000);
    _min_cell_volume->setDecimals(2);

    _indexing_tolerance->setMaximum(10);
    _indexing_tolerance->setDecimals(6);

    _frequency_tolerance->setMaximum(1);
    _frequency_tolerance->setDecimals(3);

    connect(_index_button, &QPushButton::clicked, this, &SubframeExperiment::autoindex);
    connect(_save_button, &QPushButton::clicked, this, &SubframeExperiment::saveCell);
    connect(
        gGui->sideBar(), &SideBar::subframeChanged, this,
        &SubframeExperiment::setIndexerParameters);

    _strategy_layout->addWidget(index_spoiler);
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
    _tab_widget->setCurrentIndex(0);
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
    grabIndexerParameters();
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
    _import_masks->setEnabled(false);
    _export_masks->setEnabled(false);
    _delete_masks->setEnabled(false);
    _toggle_selection->setEnabled(false);

    bool hasProject = gSession->hasProject();

    if (!hasProject) return;
    bool hasData = gSession->currentProject()->hasDataSet();

    _calc_intensity->setEnabled(hasData);

    if (!gSession->currentProject()->hasDataSet())
        return;
    ohkl::Experiment* expt = gSession->currentProject()->experiment();
    auto data = expt->getDataMap()->at(_detector_widget->dataCombo()->currentText().toStdString());
    bool hasMasks = data->hasMasks();

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

    _import_masks->setEnabled(hasData);
    _export_masks->setEnabled(hasMasks);
    _delete_masks->setEnabled(_selected_masks.size() > 0);
    _toggle_selection->setEnabled(hasMasks);
}

void SubframeExperiment::find_2d()
{
    ohkl::Experiment* expt = gSession->currentProject()->experiment();
    ohkl::PeakFinder2D* finder = expt->peakFinder2D();
    ohkl::sptrDataSet data = _data_combo->currentData();
    int frame = _detector_widget->scene()->currentFrame();

    ohkl::sptrProgressHandler progHandler = ohkl::sptrProgressHandler(new ohkl::ProgressHandler);
    ProgressView progressView(nullptr);
    progressView.watch(progHandler);
    finder->setHandler(progHandler);

    finder->setData(data);
    _detector_widget->scene()->linkPerFrameSpots(finder->keypoints());

    setFinderParameters();

    if (_search_all_frames->isChecked())
        finder->findAll();
    else
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

    setIndexerParameters();

    indexer->autoIndex(peaks);

    _solutions.clear();
    _solutions = indexer->solutions();
    buildSolutionTable();
    _tab_widget->setCurrentIndex(1);
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

void SubframeExperiment::grabIndexerParameters()
{
    if (!gSession->hasProject())
        return;

    auto* indexer = gSession->currentProject()->experiment()->autoIndexer();
    auto* params = indexer->parameters();

    _niggli->setValue(params->niggliTolerance);
    _only_niggli->setChecked(params->niggliReduction);
    _gruber->setValue(params->gruberTolerance);
    _number_vertices->setValue(params->nVertices);
    _number_subdivisions->setValue(params->subdiv);
    _number_solutions->setValue(params->nSolutions);
    _max_cell_dimension->setValue(params->maxdim);
    _indexing_tolerance->setValue(params->indexingTolerance);
    _frequency_tolerance->setValue(params->frequencyTolerance);
    _min_cell_volume->setValue(params->minUnitCellVolume);
}

void SubframeExperiment::setIndexerParameters()
{
    if (!gSession->hasProject())
        return;

    auto* indexer = gSession->currentProject()->experiment()->autoIndexer();
    auto* params = indexer->parameters();

    params->niggliTolerance = _niggli->value();
    params->niggliReduction = _only_niggli->isChecked();
    params->gruberTolerance = _gruber->value();
    params->nVertices = _number_vertices->value();
    params->subdiv = _number_subdivisions->value();
    params->nSolutions = _number_solutions->value();
    params->maxdim = _max_cell_dimension->value();
    params->indexingTolerance = _indexing_tolerance->value();
    params->frequencyTolerance = _frequency_tolerance->value();
    params->minUnitCellVolume = _min_cell_volume->value();
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
    switch(_left_widget->currentIndex()) {
    case 0: {
        if (_set_initial_ki->isChecked()) {
            _detector_widget->enableCursorMode(false);
            _lineplot_box->setChecked(false);
            _mask_box->setChecked(false);
            _detector_widget->scene()->changeInteractionMode(7);
        } else {
            _detector_widget->enableCursorMode(true);
            _detector_widget->scene()->changeInteractionMode(0);
        }
        break;
    }
    case 1: {
        if (_lineplot_box->isChecked()) {
            _detector_widget->enableCursorMode(false);
            _set_initial_ki->setChecked(false);
            _mask_box->setChecked(false);
            setPlotMode();
        } else {
            _detector_widget->enableCursorMode(true);
            _detector_widget->scene()->changeInteractionMode(0);
        }
        break;
    }
    case 2: {
        if (_mask_box->isChecked()) {
            _detector_widget->enableCursorMode(false);
            _set_initial_ki->setChecked(false);
            _lineplot_box->setChecked(false);
            setMaskMode();
        } else {
            _detector_widget->enableCursorMode(true);
            _detector_widget->scene()->changeInteractionMode(0);
        }
        break;
    }
    default: _detector_widget->scene()->changeInteractionMode(0);
    }
}

void SubframeExperiment::setPlotMode()
{
    switch(_lineplot_combo->currentIndex()) {
        case 0: _detector_widget->scene()->changeInteractionMode(4); break;
        case 1: _detector_widget->scene()->changeInteractionMode(5); break;
        case 2: _detector_widget->scene()->changeInteractionMode(6); break;
    }
}

void SubframeExperiment::setMaskMode()
{
    switch (_mask_combo->currentIndex()) {
        case 0: _detector_widget->scene()->changeInteractionMode(2); break;
        case 1: _detector_widget->scene()->changeInteractionMode(3); break;
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

void SubframeExperiment::buildSolutionTable()
{
    // Create table with 9 columns
    QStandardItemModel* model = new QStandardItemModel(_solutions.size(), 9, this);
    model->setHorizontalHeaderItem(0, new QStandardItem("a"));
    model->setHorizontalHeaderItem(1, new QStandardItem("b"));
    model->setHorizontalHeaderItem(2, new QStandardItem("c"));
    model->setHorizontalHeaderItem(3, new QStandardItem(QString((QChar)0x03B1))); // alpha
    model->setHorizontalHeaderItem(4, new QStandardItem(QString((QChar)0x03B2))); // beta
    model->setHorizontalHeaderItem(5, new QStandardItem(QString((QChar)0x03B3))); // gamma
    model->setHorizontalHeaderItem(6, new QStandardItem("Volume"));
    model->setHorizontalHeaderItem(7, new QStandardItem("Bravais type"));
    model->setHorizontalHeaderItem(8, new QStandardItem("Quality"));

    // Display solutions
    for (unsigned int i = 0; i < _solutions.size(); ++i) {
        const ohkl::sptrUnitCell cell = _solutions[i].first;
        const double quality = _solutions[i].second;
        const double volume = cell->volume();

        const ohkl::UnitCellCharacter ch = cell->character();
        const ohkl::UnitCellCharacter sigma = cell->characterSigmas();

        ValueTupleItem* col1 = new ValueTupleItem(
            QString::number(ch.a, 'f', 3) + "(" + QString::number(sigma.a * 1000, 'f', 0) + ")",
            ch.a, sigma.a);
        ValueTupleItem* col2 = new ValueTupleItem(
            QString::number(ch.b, 'f', 3) + "(" + QString::number(sigma.b * 1000, 'f', 0) + ")",
            ch.b, sigma.b);
        ValueTupleItem* col3 = new ValueTupleItem(
            QString::number(ch.c, 'f', 3) + "(" + QString::number(sigma.c * 1000, 'f', 0) + ")",
            ch.c, sigma.c);
        ValueTupleItem* col4 = new ValueTupleItem(
            QString::number(ch.alpha / ohkl::deg, 'f', 3) + "("
                + QString::number(sigma.alpha / ohkl::deg * 1000, 'f', 0) + ")",
            ch.alpha, sigma.alpha);
        ValueTupleItem* col5 = new ValueTupleItem(
            QString::number(ch.beta / ohkl::deg, 'f', 3) + "("
                + QString::number(sigma.beta / ohkl::deg * 1000, 'f', 0) + ")",
            ch.beta, sigma.beta);
        ValueTupleItem* col6 = new ValueTupleItem(
            QString::number(ch.gamma / ohkl::deg, 'f', 3) + "("
                + QString::number(sigma.gamma / ohkl::deg * 1000, 'f', 0) + ")",
            ch.gamma, sigma.gamma);
        ValueTupleItem* col7 = new ValueTupleItem(QString::number(volume, 'f', 3), volume);
        QStandardItem* col8 = new QStandardItem(QString::fromStdString(cell->bravaisTypeSymbol()));
        ValueTupleItem* col9 = new ValueTupleItem(QString::number(quality, 'f', 2) + "%", quality);

        model->setItem(i, 0, col1);
        model->setItem(i, 1, col2);
        model->setItem(i, 2, col3);
        model->setItem(i, 3, col4);
        model->setItem(i, 4, col5);
        model->setItem(i, 5, col6);
        model->setItem(i, 6, col7);
        model->setItem(i, 7, col8);
        model->setItem(i, 8, col9);
    }
    _solution_table->setModel(model);
}

void SubframeExperiment::selectSolutionTable()
{
    const QItemSelectionModel* select = _solution_table->selectionModel();
    QModelIndexList indices = select->selectedRows();
    if (!indices.empty())
        selectSolutionHeader(indices[0].row());
    toggleUnsafeWidgets();
}

void SubframeExperiment::selectSolutionHeader(int index)
{
    _selected_unit_cell = _solutions[index].first;
}

void SubframeExperiment::saveCell()
{
    if (_selected_unit_cell) {
        auto* expt = gSession->currentProject()->experiment();
        QStringList collections =
            gSession->currentProject()->getPeakCollectionNames(ohkl::PeakCollectionType::FOUND);

        QStringList space_groups;
        for (const std::string& name : _selected_unit_cell->compatibleSpaceGroups())
            space_groups.push_back(QString::fromStdString(name));

        std::unique_ptr<UnitCellDialog> dlg(
            new UnitCellDialog(QString::fromStdString(expt->generateUnitCellName()), space_groups));
        dlg->exec();
        if (dlg->unitCellName().isEmpty())
            return;
        if (dlg->result() == QDialog::Rejected)
            return;

        std::string cellName = dlg->unitCellName().toStdString();
        _selected_unit_cell->setName(cellName);
        _selected_unit_cell->setSpaceGroup(dlg->spaceGroup().toStdString());
        expt->addUnitCell(dlg->unitCellName().toStdString(), *_selected_unit_cell.get());
        gSession->onUnitCellChanged();

        gGui->refreshMenu();
    }
}

void SubframeExperiment::refreshMaskTable()
{
    _mask_table->clearContents();
    if (!gSession->hasProject())
        return;
    if (!gSession->currentProject()->hasDataSet())
        return;

    auto data = _detector_widget->currentData();
    int row = 0;
    QDoubleSpinBox* spin;
    QCheckBox* cbox;
    for (auto* mask : data->masks()) {
        if (row >= _mask_table->rowCount())
            _mask_table->insertRow(_mask_table->rowCount());
        auto aabb = mask->aabb();
        int col = 0;
        spin = new QDoubleSpinBox;
        spin->setMaximum(data->nCols());
        spin->setValue(aabb.lower()[0]);
        spin->setProperty("row", row);
        spin->setProperty("col", col);
        connect(
            spin, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &SubframeExperiment::onMaskChanged);
        _mask_table->setCellWidget(row, col++, spin);
        spin = new QDoubleSpinBox;
        spin->setMaximum(data->nRows());
        spin->setValue(aabb.lower()[1]);
        spin->setProperty("row", row);
        spin->setProperty("col", col);
        connect(
            spin, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &SubframeExperiment::onMaskChanged);
        _mask_table->setCellWidget(row, col++, spin);
        spin = new QDoubleSpinBox;
        spin->setMaximum(data->nCols());
        spin->setValue(aabb.upper()[0]);
        spin->setProperty("row", row);
        spin->setProperty("col", col);
        connect(
            spin, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &SubframeExperiment::onMaskChanged);
        _mask_table->setCellWidget(row, col++, spin);
        spin = new QDoubleSpinBox;
        spin->setMaximum(data->nRows());
        spin->setValue(aabb.upper()[1]);
        spin->setProperty("row", row);
        spin->setProperty("col", col);
        connect(
            spin, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &SubframeExperiment::onMaskChanged);
        _mask_table->setCellWidget(row, col++, spin);

        cbox = new QCheckBox(_mask_table);
        cbox->setStyleSheet("margin-left:20%; margin-right:20%;");
        cbox->setProperty("row", row);

        _mask_table->setCellWidget(row++, col++, cbox);
        connect(cbox, &QCheckBox::stateChanged, this, &SubframeExperiment::onMaskSelected);
    }
    _mask_table->resizeColumnsToContents();
    _mask_table->clearSpans();
    toggleUnsafeWidgets();
}

void SubframeExperiment::onMaskChanged()
{
    int row = sender()->property("row").toInt();
    auto it = _detector_widget->currentData()->masks().begin();
    std::advance(it, row);
    double x1 = dynamic_cast<QDoubleSpinBox*>(_mask_table->cellWidget(row, 0))->value();
    double y1 = dynamic_cast<QDoubleSpinBox*>(_mask_table->cellWidget(row, 1))->value();
    double x2 = dynamic_cast<QDoubleSpinBox*>(_mask_table->cellWidget(row, 2))->value();
    double y2 = dynamic_cast<QDoubleSpinBox*>(_mask_table->cellWidget(row, 3))->value();
    (*it)->setAABB(ohkl::AABB({x1, y1, 0}, {x2, y2, 0}));
    _detector_widget->scene()->loadMasksFromData();
}

void SubframeExperiment::onMaskSelected()
{
    int row = sender()->property("row").toInt();
    if (((QCheckBox*)sender())->isChecked())
        _selected_masks.emplace_back(row);
    else
        _selected_masks.erase(std::remove(_selected_masks.begin(), _selected_masks.end(), row), _selected_masks.end());

    if (_selected_masks.size() > 0)
        _toggle_selection->setText("Deselect all");
    else
        _toggle_selection->setText("Select all");

    toggleUnsafeWidgets();
}

void SubframeExperiment::deleteSelectedMasks()
{
    if (_selected_masks.size() == 0) return;
    auto data = _detector_widget->currentData();
    if (data == nullptr) return;

    data->removeMaskByIndex(_selected_masks);
    _selected_masks.clear();
    refreshMaskTable();

    _detector_widget->scene()->loadMasksFromData();
    toggleUnsafeWidgets();
    _mask_table->setRowCount(data->getNMasks());
}

void SubframeExperiment::deselectAllMasks()
{
    if (_selected_masks.size() == 0) return;
    _selected_masks.clear();
    toggleUnsafeWidgets();
}

void SubframeExperiment::selectAllMasks()
{
    if (!_detector_widget->currentData()->hasMasks()) return;

    auto nmasks = _detector_widget->currentData()->getNMasks();

    if (_selected_masks.size() == 0){ // gonna select all masks
        for (auto i = 0; i < nmasks; ++i){
            QCheckBox* cb = ((QCheckBox*)_mask_table->cellWidget(i, 4));
            cb->setChecked(true);
            _selected_masks.emplace_back(i);
        }
        _toggle_selection->setText("Deselect all");
    } else { // we clear everything from the list
        for (auto i = 0; i < nmasks; ++i){
            QCheckBox* cb = ((QCheckBox*)_mask_table->cellWidget(i, 4));
            cb->setChecked(false);
        }
        _selected_masks.clear();
        _toggle_selection->setText("Select all");
    }
    toggleUnsafeWidgets();
}
