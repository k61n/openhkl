//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_experiment/SubframeExperiment.cpp
//! @brief     Implements class SubframeExperiment
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_experiment/SubframeExperiment.h"

#include "base/utils/Logger.h"
#include "base/utils/Units.h"
#include "core/algo/AutoIndexer.h"
#include "core/data/DataSet.h"
#include "core/experiment/DataQuality.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/MaskExporter.h"
#include "core/experiment/MaskImporter.h"
#include "core/experiment/PeakFinder2D.h"
#include "core/instrument/Diffractometer.h"
#include "core/peak/Qs2Events.h"
#include "core/shape/Predictor.h"
#include "core/statistics/PeakMerger.h"
#include "gui/MainWin.h" // gGui
#include "gui/connect/Sentinel.h"
#include "gui/dialogs/ListNameDialog.h"
#include "gui/dialogs/UnitCellDialog.h"
#include "gui/frames/ProgressView.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/graphics_items/MaskItem.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/CellComboBox.h"
#include "gui/utility/DataComboBox.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/PropertyScrollArea.h"
#include "gui/utility/SafeSpinBox.h"
#include "gui/utility/SideBar.h"
#include "gui/utility/Spoiler.h"
#include "gui/utility/SpoilerCheck.h"
#include "gui/views/UnitCellTableView.h"
#include "gui/widgets/DetectorWidget.h"
#include "gui/widgets/DirectBeamWidget.h"
#include "gui/widgets/PeakViewWidget.h"
#include "gui/widgets/PlotPanel.h"

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollBar>
#include <QSplitter>
#include <QTabWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

#include <cstring>
#include <gsl/gsl_histogram.h>
#include <stdexcept>

SubframeExperiment::SubframeExperiment()
    : QWidget()
    , _mask_table_rows(15)
    , _show_direct_beam(true)
    , _peak_collection("temp", ohkl::PeakCollectionType::PREDICTED, nullptr)
    , _peak_collection_item()
    , _peak_collection_model()
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

    _detector_widget = new DetectorWidget(1, true, true, figure_group);
    _beam_setter_widget = new DirectBeamWidget(_detector_widget->scene());
    _peak_view_widget = new PeakViewWidget("Valid peaks", "Invalid peaks");

    QSplitter* right_splitter = new QSplitter();
    right_splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    right_splitter->setOrientation(Qt::Orientation::Vertical);
    right_splitter->setChildrenCollapsible(false);
    right_splitter->addWidget(figure_group);
    right_splitter->addWidget(_tab_widget);

    right_splitter->setStretchFactor(0, 2);
    right_splitter->setStretchFactor(1, 1);

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
        _detector_widget->dataCombo(), QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        &SubframeExperiment::calculateIntensities);

    connect(
        _detector_widget->scroll(), &QScrollBar::valueChanged, this,
        &SubframeExperiment::refreshAll);

    connect(_update_plot, &QPushButton::clicked, this, &SubframeExperiment::refreshAll);
    connect(
        _cell_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeExperiment::setUnitCell);

    connect(
        _data_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        _detector_widget->dataCombo(), &QComboBox::setCurrentIndex);
    connect(
        _detector_widget->dataCombo(), QOverload<int>::of(&QComboBox::currentIndexChanged),
        _data_combo, &QComboBox::setCurrentIndex);

    // if masks are selected graphicaly update mask table
    connect(
        _detector_widget->scene(), &DetectorScene::signalMasksSelected, this,
        &SubframeExperiment::refreshMaskTable);

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
    _set_initial_ki = new Spoiler("Set initial direct beam position");
    _set_initial_ki->setContentLayout(*_beam_setter_widget, true);

    connect(_left_widget, &QTabWidget::currentChanged, this, &SubframeExperiment::resetMode);
    connect(
        _beam_setter_widget->crosshairOn(), &QCheckBox::stateChanged, this,
        &SubframeExperiment::refreshVisual);
    connect(
        _beam_setter_widget->crosshairOn(), &QCheckBox::stateChanged, this,
        &SubframeExperiment::toggleCursorMode);

    _detector_widget->scene()->linkDirectBeam(&_direct_beam_events, &_old_direct_beam_events);

    _strategy_layout->addWidget(_set_initial_ki);
}

void SubframeExperiment::setPreviewUp()
{
    Spoiler* preview_box = new Spoiler("Show/hide peaks");
    connect(
        _peak_view_widget, &PeakViewWidget::settingsChanged, this,
        &SubframeExperiment::refreshPeaks);
    preview_box->setContentLayout(*_peak_view_widget);
    _detector_widget->linkPeakModel(&_peak_collection_model, _peak_view_widget);
    _strategy_layout->addWidget(preview_box);
}

void SubframeExperiment::setStrategyUp()
{
    setPeakFinder2DUp();
    setAdjustBeamUp();
    setIndexerUp();
    setPredictUp();
    setPreviewUp();

    _strategy_layout->addStretch();
}

void SubframeExperiment::setHistogramUp()
{

    _intensity_plot_box = new Spoiler("Per-pixel detector count histogram");
    //_intensity_plot_box->setMaximumWidth(400);
    GridFiller gfiller(_intensity_plot_box, true);

    _npoints_intensity = gfiller.addSpinBox(QString("Number of bins:"));

    _calc_intensity = gfiller.addButton("Build pixel count histogram");

    _totalHistogram = gfiller.addCheckBox("All images", 1);
    _yLog = gfiller.addCheckBox("Log-linear", 1);

    _histogram_layout->addWidget(_intensity_plot_box);


    _npoints_intensity->setMaximumWidth(100);
    _npoints_intensity->setMaximum(65535);
    _npoints_intensity->setMinimum(100);
    _npoints_intensity->setValue(100);

    std::tie(_minX, _maxX) = gfiller.addSpinBoxPair("x range", "Horizontal axis range");
    std::tie(_minY, _maxY) = gfiller.addSpinBoxPair("y range", "vertical (frequency) axis range");

    _update_plot = gfiller.addButton("Update plot");

    connect(_totalHistogram, &QCheckBox::clicked, this, &SubframeExperiment::refreshAll);
    connect(_yLog, &QCheckBox::clicked, this, &SubframeExperiment::setLogarithmicScale);
    connect(
        _calc_intensity, &QPushButton::clicked, this, &SubframeExperiment::calculateIntensities);

    _lineplot_box = new SpoilerCheck("Plot intensity profiles");
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
        _lineplot_combo, &QComboBox::currentTextChanged, this,
        &SubframeExperiment::toggleCursorMode);
}

void SubframeExperiment::setMaskUp()
{
    _mask_box = new SpoilerCheck("Add detector image masks");
    GridFiller gfiller(_mask_box, true);

    _mask_combo = gfiller.addCombo("Mask type");
    _mask_combo->addItems(QStringList{"Rectangular mask", "Elliptical mask"});

    _mask_layout->addWidget(_mask_box);

    Spoiler* mask_table_box = new Spoiler("List of Masks");
    GridFiller gfiller2(mask_table_box, true);
    _mask_table = new QTableWidget(0, 5);
    _mask_table->setMinimumHeight(
        _mask_table->verticalHeader()->defaultSectionSize() * _mask_table_rows);
    _mask_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    _mask_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _mask_table->setHorizontalHeaderLabels(
        QStringList{"x lower", "y lower", "x upper", "y upper", "select"});

    gfiller2.addWidget(_mask_table, 0, 2);
    _mask_table->resizeColumnsToContents();

    _mask_layout->addWidget(mask_table_box);

    _import_masks = new QPushButton("Import masks");
    _export_masks = new QPushButton("Export masks");
    _delete_masks = new QPushButton("Delete masks");
    _toggle_selection = new QPushButton("(De)select all");

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

    connect(_export_masks, &QPushButton::clicked, this, &SubframeExperiment::exportMasks);
    connect(_import_masks, &QPushButton::clicked, this, &SubframeExperiment::importMasks);
    connect(_delete_masks, &QPushButton::clicked, this, &SubframeExperiment::deleteSelectedMasks);
    connect(_toggle_selection, &QPushButton::clicked, this, &SubframeExperiment::selectAllMasks);
}

void SubframeExperiment::importMasks()
{
    QSettings settings = gGui->qSettings();
    settings.beginGroup("RecentDirectories");
    QString loadDirectory = settings.value("masks", QDir::homePath()).toString();
    settings.setValue("masks", loadDirectory);

    QString file_path =
        QFileDialog::getOpenFileName(this, "Import masks from file", loadDirectory, "YAML (*.yml)");

    if (file_path.isEmpty())
        return;

    ohkl::MaskImporter importer(
        file_path.toStdString(), _data_combo->currentData()->nFrames()); // TODO: update nframes
    for (auto* mask : importer.getMasks())
        _data_combo->currentData()->addMask(mask);

    QFileInfo info(file_path);
    loadDirectory = info.absolutePath();
    settings.setValue("masks", loadDirectory);

    _detector_widget->scene()->loadMasksFromData();
    toggleUnsafeWidgets();
}

void SubframeExperiment::exportMasks()
{
    QSettings settings = gGui->qSettings();
    settings.beginGroup("RecentDirectories");
    QString loadDirectory = settings.value("experiment", QDir::homePath()).toString();

    QString file_path =
        QFileDialog::getSaveFileName(this, "Export maks to ", loadDirectory, "YAML (*.yml)");

    if (file_path.isEmpty())
        return;

    ohkl::MaskExporter exporter(_data_combo->currentData()->masks());
    exporter.exportToFile(file_path.toStdString());

    QFileInfo info(file_path);
    loadDirectory = info.absolutePath();
    settings.setValue("masks", loadDirectory);

    toggleUnsafeWidgets();
}

void SubframeExperiment::setPeakFinder2DUp()
{
    Spoiler* peak2D_spoiler = new Spoiler("Find blobs in this image");
    GridFiller gfiller(peak2D_spoiler, true);

    _data_combo = gfiller.addDataCombo("Data set");
    _convolver_combo =
        gfiller.addCombo("Convolution kernel", "Convolver kernel type to use in image filtering");
    _threshold_spin = gfiller.addSpinBox(
        "Filtered image threshold", "Minimum counts to use in image thresholding");
    _blob_min_thresh =
        gfiller.addSpinBox("Minimum blob threshold", "Minimum threshold for blob detection");
    _blob_max_thresh =
        gfiller.addSpinBox("Maximum blob threshold", "Maximum threshold for blob detection");
    _search_all_frames =
        gfiller.addCheckBox("Search all images", "Find blobs in all images in this data set", 1);
    _threshold_check = gfiller.addCheckBox(
        "Apply threshold to preview", "Show detector image post filtering/thresholding", 1);
    _find_peaks_2d = gfiller.addButton("Find spots", "Find detector spots in current image");

    auto kernel_types = ohkl::ImageFilterStrings;
    for (auto it = kernel_types.begin(); it != kernel_types.end(); ++it)
        _convolver_combo->addItem(QString::fromStdString(it->second));
    _convolver_combo->setCurrentIndex(1);

    _blob_min_thresh->setMaximum(256);
    _blob_max_thresh->setMaximum(256);

    _blob_min_thresh->setValue(1);
    _blob_max_thresh->setValue(100);

    connect(
        _data_combo, &QComboBox::currentTextChanged, this,
        &SubframeExperiment::toggleUnsafeWidgets);
    connect(
        _data_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        _detector_widget->dataCombo(), &QComboBox::setCurrentIndex);
    connect(_threshold_check, &QCheckBox::clicked, this, &SubframeExperiment::showFilteredImage);
    connect(_find_peaks_2d, &QPushButton::clicked, this, &SubframeExperiment::find_2d);
    connect(
        _threshold_spin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &SubframeExperiment::showFilteredImage);

    _strategy_layout->addWidget(peak2D_spoiler);
}

void SubframeExperiment::setIndexerUp()
{
    Spoiler* index_spoiler = new Spoiler("Autoindex using spots in this image");
    GridFiller gfiller(index_spoiler, true);
    std::tie(_d_min, _d_max) =
        gfiller.addDoubleSpinBoxPair("d range", "Resolution range for peaks used in indexing");
    _gruber = gfiller.addDoubleSpinBox("Gruber tolerance:", "Tolerance for Gruber reduction");

    _niggli = gfiller.addDoubleSpinBox("Niggli tolerance:", "Tolerance for Niggli reduction");

    _only_niggli = gfiller.addCheckBox("Find Niggli cell only", 1);

    _max_cell_dimension = gfiller.addDoubleSpinBox(
        "Max. Cell dimension:",
        QString::fromUtf8("(\u212B) - maximum length of any lattice vector"));

    _number_vertices = gfiller.addSpinBox(
        "Num. Q-space trial vectors:",
        "Number of points on reciprocal space unit sphere to test against candidate lattice "
        "vector");

    _number_subdivisions = gfiller.addSpinBox(
        "Num. FFT histogram bins:", "Number of histogram bins for Fast Fourier transform");

    _number_solutions =
        gfiller.addSpinBox("Number of solutions:", "Number of unit cell solutions to find");

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
    _save_cell = gfiller.addButton("Save unit cell", "Save the selected unit cell");

    _d_min->setMinimum(0);
    _d_min->setMaximum(100);
    _d_min->setValue(1.5);

    _d_max->setMaximum(0);
    _d_max->setMaximum(100);
    _d_max->setValue(50);

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
    connect(_save_cell, &QPushButton::clicked, this, &SubframeExperiment::saveCell);

    _strategy_layout->addWidget(index_spoiler);
}

void SubframeExperiment::setPredictUp()
{
    Spoiler* predict_spoiler = new Spoiler("Predict peaks");
    GridFiller gfiller(predict_spoiler, true);

    _cell_combo = gfiller.addCellCombo("Unit cell", "Unit cell to use for peak prediction");
    _delta_chi = gfiller.addDoubleSpinBox(
        QString((QChar)0x0394) + " " + QString((QChar)0x03C7),
        "Angle increment about the chi instrument axis");
    _delta_omega = gfiller.addDoubleSpinBox(
        QString((QChar)0x0394) + " " + QString((QChar)0x03C9),
        "Angle increment about the omega instrument axis");
    _delta_phi = gfiller.addDoubleSpinBox(
        QString((QChar)0x0394) + " " + QString((QChar)0x03C6),
        "Angle incremet about the phi instrument axis");
    _n_increments = gfiller.addSpinBox(
        "Number of increments", "Number of angular steps to use in strategy prediction");
    std::tie(_predict_d_min, _predict_d_max) =
        gfiller.addDoubleSpinBoxPair("d range", "Resolution range for peaks used in indexing");
    _predict_button = gfiller.addButton("Predict", "Predict peaks using given strategy");
    _save_peaks = gfiller.addButton("Create peak collection");

    _delta_chi->setSingleStep(0.1);
    _delta_chi->setValue(0);
    _delta_chi->setMaximum(5);

    _delta_omega->setSingleStep(0.1);
    _delta_omega->setValue(0.5);
    _delta_omega->setMaximum(5);

    _delta_phi->setSingleStep(0.1);
    _delta_phi->setValue(0);
    _delta_phi->setMaximum(5);

    _n_increments->setMaximum(1000);
    _n_increments->setValue(100);

    _predict_d_min->setMinimum(0);
    _predict_d_min->setMaximum(100);
    _predict_d_min->setValue(1.5);

    _predict_d_max->setMaximum(0);
    _predict_d_max->setMaximum(100);
    _predict_d_max->setValue(50);

    connect(_predict_button, &QPushButton::clicked, this, &SubframeExperiment::predict);
    connect(_save_peaks, &QPushButton::clicked, this, &SubframeExperiment::savePeaks);

    _strategy_layout->addWidget(predict_spoiler);
}

void SubframeExperiment::setLogarithmicScale()
{
    _plot->setYLog(_yLog->isChecked());
    updateRanges();
    plotIntensities();
}

void SubframeExperiment::calculateIntensities()
{
    auto data = _data_combo->currentData();
    bool hasHistograms = data->getNumberHistograms() > 0;

    if (!data)
        return;
    if (hasHistograms)
        data->clearHistograms();
    data->getIntensityHistogram(_npoints_intensity->value());

    _maxX->setMaximum(data->nCols() * data->nRows());
    _minX->setMaximum(data->nCols() * data->nRows() - 1);
    _maxY->setMaximum(1e+9);
    _minY->setMaximum(1e+9 - 1);

    updateRanges();
    toggleUnsafeWidgets();
    plotIntensities();
}

void SubframeExperiment::updateRanges()
{
    auto data = _data_combo->currentData();

    gsl_histogram* histo = nullptr;

    if (!_totalHistogram->isChecked())
        histo = data->getHistogram(_detector_widget->scroll()->value() - 1);
    else
        histo = data->getTotalHistogram();

    if (!histo)
        return;

    _minX->setValue(0);
    _maxX->setValue(data->maxCount());

    double max_element = *(std::max_element(histo->bin, histo->bin + histo->n * 8));
    _minY->setValue(0);
    _maxY->setValue(max_element);
}

void SubframeExperiment::showFilteredImage()
{
    _detector_widget->scene()->params()->filteredImage = _threshold_check->isChecked();
    _detector_widget->scene()->params()->threshold = _threshold_spin->value();
    _detector_widget->scene()->params()->filter =
        static_cast<ohkl::ImageFilterType>(_convolver_combo->currentIndex());
    _detector_widget->scene()->loadCurrentImage();
    setFinderParameters();
}

void SubframeExperiment::plotIntensities()
{
    auto data = _data_combo->currentData();

    if (!data)
        return;

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

    if (histo->range == nullptr || histo->bin == nullptr)
        throw std::runtime_error(
            "SubframeExperiment::plotIntensities received invalid arrays for gsl_histogram");

    auto plot = getPlot();
    if (plot != nullptr)
        plot->plotData(
            histo, QString("Number of pixels"), QString("Counts"), xmin, xmax, ymin, ymax);
    _tab_widget->setCurrentIndex(0);
}

void SubframeExperiment::refreshVisual()
{
    showDirectBeamEvents();
    _detector_widget->refresh();
}

void SubframeExperiment::refreshAll()
{
    if (!gSession->hasProject())
        return;

    _data_combo->refresh();
    _cell_combo->refresh();
    _detector_widget->refresh();
    setUnitCell();
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
    _find_peaks_2d->setEnabled(false);
    _index_button->setEnabled(false);
    _save_cell->setEnabled(false);
    _save_peaks->setEnabled(false);

    _calc_intensity->setEnabled(false);
    _yLog->setEnabled(false);
    _update_plot->setEnabled(false);
    _totalHistogram->setEnabled(false);

    _mask_box->setEnabled(false);
    _import_masks->setEnabled(false);
    _export_masks->setEnabled(false);
    _delete_masks->setEnabled(false);
    _toggle_selection->setEnabled(false);
    _predict_button->setEnabled(false);
    _save_peaks->setEnabled(false);

    if (!gSession->hasProject())
        return;

    _find_peaks_2d->setEnabled(gSession->currentProject()->hasDataSet());
    _index_button->setEnabled(gSession->currentProject()->hasDataSet());
    auto* indexer = gSession->currentProject()->experiment()->autoIndexer();
    _save_cell->setEnabled(!indexer->solutions().empty());
    auto* predictor = gSession->currentProject()->experiment()->predictor();
    _save_peaks->setEnabled(!predictor->peaks().empty());

    _calc_intensity->setEnabled(gSession->currentProject()->hasDataSet());

    if (!gSession->currentProject()->hasDataSet())
        return;

    bool hasHistograms = _data_combo->currentData()->getNumberHistograms() > 0;

    _yLog->setEnabled(hasHistograms);
    _totalHistogram->setEnabled(hasHistograms);

    _update_plot->setEnabled(hasHistograms);

    bool hasSelectedMasks = false;
    for (auto* graphic : _detector_widget->scene()->maskItems()) {
        if (graphic->isSelected()) {
            hasSelectedMasks = true;
            break;
        }
    }

    _mask_box->setEnabled(gSession->currentProject()->hasDataSet());
    _import_masks->setEnabled(gSession->currentProject()->hasDataSet());
    _export_masks->setEnabled(gSession->currentProject()->hasDataSet());
    _delete_masks->setEnabled(hasSelectedMasks);
    _toggle_selection->setEnabled(_data_combo->currentData()->hasMasks());

    if (gSession->currentProject()->strategyMode()) {
        _n_increments->setEnabled(true);
        _predict_button->setEnabled(gSession->currentProject()->hasUnitCell());
        _save_peaks->setEnabled(_peak_collection.numberOfPeaks() > 0);
    }
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
    _detector_widget->scene()->params()->detectorSpots = true;
    _detector_widget->scene()->linkKeyPoints(finder->keypoints(), 0);

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

    ohkl::sptrDataSet data = _data_combo->currentData();
    setInitialKi(data);

    std::size_t current_frame = _detector_widget->scene()->currentFrame();
    std::vector<ohkl::Peak3D*> peaks = finder->getPeakList(current_frame);
    if (peaks.empty())
        return;

    setIndexerParameters();

    const ohkl::InstrumentState& state =
        data->instrumentStates().at(_detector_widget->scene()->currentFrame());
    indexer->autoIndex(peaks, data, &state, true);

    _solutions.clear();
    _solutions = indexer->solutions();
    buildSolutionTable();
    _tab_widget->setCurrentIndex(1);
}

void SubframeExperiment::predict()
{
    gGui->setReady(false);

    try {
        auto* expt = gSession->currentProject()->experiment();
        auto* predictor = expt->predictor();
        auto* merger = expt->peakMerger();
        setStrategyParameters();

        auto data = _data_combo->currentData();
        auto cell = _cell_combo->currentCell();
        data->setNFrames(_n_increments->value());

        ohkl::sptrProgressHandler handler(new ohkl::ProgressHandler);
        ProgressView progressView(nullptr);
        progressView.watch(handler);

        predictor->setHandler(handler);
        predictor->strategyPredict(data, cell);

        std::vector<ohkl::Peak3D*> predicted_peaks;
        for (ohkl::Peak3D* peak : predictor->peaks())
            predicted_peaks.push_back(peak);

        _peak_collection.setUnitCell(cell, false);
        _peak_collection.populate(predicted_peaks);
        _peak_collection.setData(data);
        for (ohkl::Peak3D* peak : predicted_peaks)
            delete peak;
        predicted_peaks.clear();

        _peak_collection_item.setPeakCollection(&_peak_collection);
        _peak_collection_model.setRoot(&_peak_collection_item);

        merger->setHandler(handler);
        merge();

        toggleUnsafeWidgets();
        refreshPeaks();

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString(e.what()));
    }
    gGui->setReady(true);
}

void SubframeExperiment::merge()
{
    gGui->setReady(false);
    auto* expt = gSession->currentProject()->experiment();
    auto* merger = expt->peakMerger();
    merger->reset();

    auto* merge_params = merger->parameters();

    merge_params->d_min = _d_min->value();
    merge_params->d_max = _d_max->value();
    merge_params->first_frame = 0;
    merge_params->last_frame = _n_increments->value();
    merge_params->n_shells = 10;
    merge_params->friedel = true;

    auto group = _cell_combo->currentCell()->spaceGroup();
    merger->setSpaceGroup(group);
    merger->addPeakCollection(&_peak_collection);
    merger->mergePeaks();
    merger->computeQuality();
    auto* quality = merger->sumOverallQuality();
    gGui->statusBar()->showMessage(
        "Projected completeness: "
        + QString::number(quality->shells[0].Completeness * 100.0, 'f', 2) + "%");

    std::vector<double> completeness =
        merger->strategyMerge(0, _n_increments->value(), _n_increments->value());
    QVector<double> comp, frame, error;
    for (std::size_t idx = 0; idx < completeness.size(); ++idx) {
        comp.push_back(completeness.at(idx));
        frame.push_back(idx);
    }

    _tab_widget->setCurrentIndex(0);
    _plot->plotData(frame, comp, error, QString("Image index"), QString("Completeness"));

    gGui->setReady(true);
}

void SubframeExperiment::savePeaks()
{
    auto* project = gSession->currentProject();
    auto* expt = project->experiment();
    auto data = _detector_widget->currentData();
    auto cell = _cell_combo->currentCell();
    std::string suggestion = expt->generatePeakCollectionName();
    std::unique_ptr<ListNameDialog> dlg(new ListNameDialog(QString::fromStdString(suggestion)));
    dlg->exec();
    if (dlg->listName().isEmpty())
        return;
    if (dlg->result() == QDialog::Rejected)
        return;

    if (!expt->addPeakCollection(
            dlg->listName().toStdString(), ohkl::PeakCollectionType::PREDICTED,
            _peak_collection.getPeakList(), data, cell)) {
        QMessageBox::warning(
            this, "Unable to add PeakCollection",
            "Unable to add PeakCollection, please use a unique name");
        return;
    }

    gSession->onPeaksChanged();
    auto* collection = expt->getPeakCollection(dlg->listName().toStdString());
    collection->setIndexed(true);
    project->generatePeakModel(dlg->listName());
}

void SubframeExperiment::grabFinderParameters()
{
    if (!gSession->hasProject())
        return;

    auto* finder = gSession->currentProject()->experiment()->peakFinder2D();
    auto* params = finder->parameters();

    _blob_min_thresh->setValue(params->minThreshold);
    _blob_max_thresh->setValue(params->maxThreshold);
    _threshold_spin->setValue(params->threshold);
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
    params->threshold = _threshold_spin->value();
    params->kernel = static_cast<ohkl::ImageFilterType>(_convolver_combo->currentIndex());
}

void SubframeExperiment::grabIndexerParameters()
{
    if (!gSession->hasProject())
        return;

    auto* indexer = gSession->currentProject()->experiment()->autoIndexer();
    auto* params = indexer->parameters();

    _d_min->setValue(params->d_min);
    _d_max->setValue(params->d_max);
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

    params->d_min = _d_min->value();
    params->d_max = _d_max->value();
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
    params->peaks_integrated = false;
}

void SubframeExperiment::grabStrategyParameters()
{
    if (!gSession->hasProject())
        return;

    auto* predictor = gSession->currentProject()->experiment()->predictor();
    auto* params = predictor->strategyParamters();

    _predict_d_min->setValue(params->d_min);
    _predict_d_max->setValue(params->d_max);
    _delta_chi->setValue(params->delta_chi);
    _delta_omega->setValue(params->delta_omega);
    _delta_phi->setValue(params->delta_phi);
    if (gSession->currentProject()->strategyMode())
        _n_increments->setValue(params->nframes);
    else
        _n_increments->setValue(_data_combo->currentData()->nFrames());
}

void SubframeExperiment::setStrategyParameters()
{
    if (!gSession->hasProject())
        return;

    auto* predictor = gSession->currentProject()->experiment()->predictor();
    auto* params = predictor->strategyParamters();

    params->d_min = _predict_d_min->value();
    params->d_max = _predict_d_max->value();
    params->delta_chi = _delta_chi->value();
    params->delta_omega = _delta_omega->value();
    params->delta_phi = _delta_phi->value();
    params->nframes = _n_increments->value();
}

void SubframeExperiment::toggleCursorMode()
{
    switch (_left_widget->currentIndex()) {
        case 0: {
            if (_beam_setter_widget->crosshairOn()->isChecked()) {
                _detector_widget->enableCursorMode(false);
                _detector_widget->scene()->changeInteractionMode(7);
            }
            break;
        }
        case 1: {
            if (_lineplot_box->isChecked()) {
                _detector_widget->enableCursorMode(false);
                setPlotMode();
            }
            break;
        }
        case 2: {
            if (_mask_box->isChecked()) {
                _detector_widget->enableCursorMode(false);
                setMaskMode();
                refreshMaskTable();
            }
            break;
        }
        default: _detector_widget->scene()->changeInteractionMode(0);
    }
}

void SubframeExperiment::setPlotMode()
{
    switch (_lineplot_combo->currentIndex()) {
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

void SubframeExperiment::resetMode(int index)
{
    QSignalBlocker blocker1(_beam_setter_widget->crosshairOn());
    QSignalBlocker blocker2(_lineplot_box);
    QSignalBlocker blocker3(_mask_box);
    switch (index) {
        case 0: {
            _lineplot_box->setChecked(false);
            _mask_box->setChecked(false);
            break;
        }
        case 1: {
            _mask_box->setChecked(false);
            _beam_setter_widget->crosshairOn()->setChecked(false);
            break;
        }
        case 2: {
            _beam_setter_widget->crosshairOn()->setChecked(false);
            _lineplot_box->setChecked(false);
            break;
        }
        default: break;
    }
    _detector_widget->enableCursorMode(true);
    _detector_widget->scene()->changeInteractionMode(0);
}

void SubframeExperiment::setInitialKi(ohkl::sptrDataSet data)
{
    data->adjustDirectBeam(_beam_setter_widget->xOffset(), _beam_setter_widget->yOffset());
    emit gGui->sentinel->instrumentStatesChanged();
}

void SubframeExperiment::showDirectBeamEvents()
{
    if (!_show_direct_beam)
        return;

    _detector_widget->scene()->params()->directBeam = true;
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
        toggleUnsafeWidgets();
    }
}

void SubframeExperiment::setUnitCell()
{
    if (!gSession->currentProject()->hasUnitCell())
        return;

    auto* cell = _cell_combo->currentCell().get();
    _detector_widget->scene()->setUnitCell(cell);
}

void SubframeExperiment::refreshMaskTable()
{
    _mask_table->clearContents();
    _mask_table->setRowCount(0);
    if (!gSession->hasProject())
        return;
    if (!gSession->currentProject()->hasDataSet())
        return;

    auto data = _detector_widget->currentData();
    int row = 0;
    QDoubleSpinBox* spin;
    QCheckBox* cbox;
    for (auto* gmask : _detector_widget->scene()->maskItems()) {
        if (row >= _mask_table->rowCount())
            _mask_table->insertRow(_mask_table->rowCount());
        auto* aabb = gmask->getAABB();
        int col = 0;
        spin = new QDoubleSpinBox;
        spin->setMaximum(data->nCols());
        spin->setValue(aabb->lower()[0]);
        spin->setProperty("row", row);
        spin->setProperty("col", col);
        connect(spin, &QDoubleSpinBox::editingFinished, this, &SubframeExperiment::onMaskChanged);
        _mask_table->setCellWidget(row, col++, spin);
        spin = new QDoubleSpinBox;
        spin->setMaximum(data->nRows());
        spin->setValue(aabb->lower()[1]);
        spin->setProperty("row", row);
        spin->setProperty("col", col);
        connect(spin, &QDoubleSpinBox::editingFinished, this, &SubframeExperiment::onMaskChanged);
        _mask_table->setCellWidget(row, col++, spin);
        spin = new QDoubleSpinBox;
        spin->setMaximum(data->nCols());
        spin->setValue(aabb->upper()[0]);
        spin->setProperty("row", row);
        spin->setProperty("col", col);
        connect(spin, &QDoubleSpinBox::editingFinished, this, &SubframeExperiment::onMaskChanged);
        _mask_table->setCellWidget(row, col++, spin);
        spin = new QDoubleSpinBox;
        spin->setMaximum(data->nRows());
        spin->setValue(aabb->upper()[1]);
        spin->setProperty("row", row);
        spin->setProperty("col", col);
        connect(spin, &QDoubleSpinBox::editingFinished, this, &SubframeExperiment::onMaskChanged);
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
    QVector<MaskItem*> mask_items = _detector_widget->scene()->maskItems();
    auto it = mask_items.begin();
    std::advance(it, row);
    double x1 = dynamic_cast<QDoubleSpinBox*>(_mask_table->cellWidget(row, 0))->value();
    double y1 = dynamic_cast<QDoubleSpinBox*>(_mask_table->cellWidget(row, 1))->value();
    double x2 = dynamic_cast<QDoubleSpinBox*>(_mask_table->cellWidget(row, 2))->value();
    double y2 = dynamic_cast<QDoubleSpinBox*>(_mask_table->cellWidget(row, 3))->value();
    std::size_t nframes = _data_combo->currentData()->nFrames();
    (*it)->mask()->setAABB(ohkl::AABB({x1, y1, 0}, {x2, y2, static_cast<double>(nframes)}));
    _detector_widget->scene()->loadMasksFromData();
}

void SubframeExperiment::onMaskSelected()
{
    _mask_box->setChecked(false);
    toggleCursorMode();

    int row = sender()->property("row").toInt();
    bool checked = dynamic_cast<QCheckBox*>(_mask_table->cellWidget(row, 4))->isChecked();
    _detector_widget->scene()->maskItems().at(row)->setSelected(checked);
    toggleUnsafeWidgets();
}

void SubframeExperiment::deleteSelectedMasks()
{
    auto data = _detector_widget->currentData();
    auto* scene = _detector_widget->scene();
    for (auto* gmask : scene->maskItems()) {
        if (gmask->isSelected()) {
            scene->removeItem(gmask);
            data->removeMask(gmask->mask());
            delete gmask;
        }
    }

    refreshMaskTable();
    _detector_widget->scene()->loadMasksFromData();
    toggleUnsafeWidgets();
    _mask_table->setRowCount(data->getNMasks());
}

void SubframeExperiment::selectAllMasks()
{
    for (auto* gmask : _detector_widget->scene()->maskItems())
        gmask->setSelected(true);

    refreshMaskTable();
    toggleUnsafeWidgets();
}

void SubframeExperiment::refreshPeaks()
{
    _detector_widget->refresh();
}
