//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_filter/SubframeReject.cpp

//! @brief     Implements class SubframeReject
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_reject/SubframeReject.h"

#include "core/experiment/Experiment.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakFilter.h"
#include "core/statistics/PeakStatistics.h"
#include "gui/MainWin.h" // gGui
#include "gui/frames/ProgressView.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/graphics/SXPlot.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/subwindows/DetectorWindow.h"
#include "gui/utility/DataComboBox.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/PeakComboBox.h"
#include "gui/utility/PropertyScrollArea.h"
#include "gui/utility/SafeSpinBox.h"
#include "gui/utility/SideBar.h"
#include "gui/utility/Spoiler.h"
#include "gui/views/PeakTableView.h"
#include "gui/widgets/DetectorWidget.h"
#include "gui/widgets/PlotPanel.h"

#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QScrollBar>
#include <QSpacerItem>
#include <qpushbutton.h>

SubframeReject::SubframeReject() : QWidget()
{
    auto main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);

    _left_layout = new QVBoxLayout();

    setInputUp();
    setHistogramUp();
    setPreviewUp();
    setFigureUp();
    setPeakTableUp();
    setPlotUp();

    _right_element->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(
        _data_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        _detector_widget->dataCombo(), &QComboBox::setCurrentIndex);
    connect(
        _detector_widget->dataCombo(), QOverload<int>::of(&QComboBox::currentIndexChanged),
        _data_combo, &QComboBox::setCurrentIndex);

    auto propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_left_layout);
    main_layout->addWidget(propertyScrollArea);
    main_layout->addWidget(_right_element);

    _peak_stats = ohkl::PeakStatistics();
}

void SubframeReject::setInputUp()
{
    auto* input_box = new Spoiler("Input");
    GridFiller f(input_box, true);

    _data_combo = f.addDataCombo("Data set");
    _peak_combo = f.addPeakCombo(ComboType::PeakCollection, "Peaks collection");

    connect(
        _peak_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeReject::toggleUnsafeWidgets);

    _left_layout->addWidget(input_box);
}

void SubframeReject::setHistogramUp()
{
    auto* histo_spoiler = new Spoiler("Histogram");
    GridFiller filler(histo_spoiler, true);

    _histo_combo = filler.addCombo("Histogram type");
    for (const auto& [type, description] : _peak_stats.getHistoStrings())
        _histo_combo->addItem(QString::fromStdString(description));

    _n_bins = filler.addSpinBox("Number of bins", "Number of histogram bins");
    std::tie(_freq_min, _freq_max) = filler.addSpinBoxPair(
        "Frequency range", "Maximum and minimum frequecies for histogram");
    std::tie(_x_min, _x_max) = filler.addSpinBoxPair(
        "Data range", "Minimum and maximum of x data series");
    _log_freq = filler.addCheckBox("Lograithmic vertical axis", "Switch to log-linear plot", 1);
    _plot_histogram = filler.addButton("Plot histogram");

    _n_bins->setMaximum(10000);
    _x_min->setMaximum(10000);
    _x_max->setMaximum(10000);
    _freq_min->setMaximum(10000);
    _freq_max->setMaximum(10000);

    _n_bins->setValue(100);
    _x_min->setValue(0);
    _x_max->setValue(10000);
    _freq_min->setValue(0);
    _freq_max->setValue(1000);

    connect(_plot_histogram, &QPushButton::clicked, this, &SubframeReject::computeHistogram);

    _left_layout->addWidget(histo_spoiler);
}

void SubframeReject::setFigureUp()
{
    QGroupBox* figure_group = new QGroupBox("Detector image");
    figure_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _detector_widget = new DetectorWidget(false, false, true, figure_group);
    _detector_widget->linkPeakModel(&_peak_collection_model);

    connect(
        _detector_widget->scene(), &DetectorScene::signalSelectedPeakItemChanged, this,
        &SubframeReject::changeSelected);
    connect(
        _peak_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeReject::refreshPeakTable);

    _right_element->addWidget(figure_group);
}

void SubframeReject::setPlotUp()
{
    _plot_widget = new PlotPanel;

    connect(_plot_widget->sxplot(), &SXPlot::signalXRange, this, &SubframeReject::filterSelection);

    _right_element->addWidget(_plot_widget);
}

void SubframeReject::refreshPeakVisual()
{
    if (_peak_collection_item.childCount() == 0)
        return;

    for (int i = 0; i < _peak_collection_item.childCount(); i++) {
        PeakItem* peak = _peak_collection_item.peakItemAt(i);
        auto graphic = peak->peakGraphic();

        graphic->showLabel(false);
        graphic->setColor(Qt::transparent);
        graphic->initFromPeakViewWidget(
            peak->peak()->caughtByFilter() ? _peak_view_widget->set1 : _peak_view_widget->set2);
    }
    _detector_widget->scene()->initIntRegionFromPeakWidget(_peak_view_widget->set1);
    _detector_widget->refresh();
}

void SubframeReject::setPeakTableUp()
{
    QGroupBox* peak_group = new QGroupBox("Peaks");
    QGridLayout* peak_grid = new QGridLayout(peak_group);

    peak_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    _peak_table = new PeakTableView(this);
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->setModel(&_peak_collection_model);
    _peak_table->resizeColumnsToContents();
    _peak_table->setColumnHidden(13, true);

    peak_grid->addWidget(_peak_table, 0, 0, 0, 0);

    _right_element->addWidget(peak_group);
}

void SubframeReject::refreshPeakTable()
{
    if (!gSession->currentProject()->hasPeakCollection())
        return;

    _peak_collection = _peak_combo->currentPeakCollection();
    _peak_collection_item.setPeakCollection(_peak_collection);
    _peak_collection_item.setFilterMode();
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->resizeColumnsToContents();

    refreshPeakVisual();
}

void SubframeReject::refreshAll()
{
    if (!gSession->hasProject())
        return;

    _data_combo->refresh();
    _detector_widget->refresh();
    _peak_combo->refresh();
    refreshPeakTable();
    toggleUnsafeWidgets();
}

void SubframeReject::setPreviewUp()
{
    Spoiler* preview_spoiler = new Spoiler("Show/hide peaks");
    _peak_view_widget = new PeakViewWidget("Valid peaks", "Invalid Peaks");

    connect(
        _peak_view_widget, &PeakViewWidget::settingsChanged, this,
        &SubframeReject::refreshPeakVisual);

    preview_spoiler->setContentLayout(*_peak_view_widget);

    _peak_view_widget->set1.drawIntegrationRegion->setChecked(false);
    _peak_view_widget->set1.previewIntRegion->setChecked(false);

    _left_layout->addWidget(preview_spoiler);
}

void SubframeReject::changeSelected(PeakItemGraphic* peak_graphic)
{
    int row = _peak_collection_item.returnRowOfVisualItem(peak_graphic);
    QModelIndex index = _peak_collection_model.index(row, 0);
    _peak_table->selectRow(row);
    _peak_table->scrollTo(index, QAbstractItemView::PositionAtTop);
}

void SubframeReject::toggleUnsafeWidgets()
{
}

void SubframeReject::computeHistogram()
{
    _peak_stats.setPeakCollection(_peak_combo->currentPeakCollection());
    ohkl::PeakHistogramType type =
        static_cast<ohkl::PeakHistogramType>(_histo_combo->currentIndex());
    _current_histogram = _peak_stats.computeHistogram(type, _n_bins->value());

    _freq_min->setMaximum(_peak_stats.maxCount());
    _freq_max->setMaximum(_peak_stats.maxCount());
    _x_max->setMaximum(_peak_stats.maxValue());
    _x_min->setMaximum(_peak_stats.maxValue());

    _x_max->setValue(_peak_stats.maxValue());
    _freq_max->setValue(_peak_stats.maxCount());

    QVector<double> value;
    QVector<double> frequency;
    QVector<double> error;

    value.resize(_current_histogram->n);
    frequency.resize(_current_histogram->n);

    memcpy(value.data(), _current_histogram->range, _current_histogram->n * sizeof(double));
    memcpy(frequency.data(), _current_histogram->bin, _current_histogram->n * sizeof(double));

    QString xLabel = QString::fromStdString(_peak_stats.getHistoStrings().find(type)->second);
    _plot_widget->setYLog(_log_freq->isChecked());
    _plot_widget->plotData(
        value, frequency, error, xLabel, QString("Frequency"),
        _x_min->value(), _x_max->value(), _freq_min->value(), _freq_max->value());
}

void SubframeReject::filterSelection(double xmin, double xmax)
{
    qDebug() << xmin << xmax;
    ohkl::PeakFilter* filter = gSession->currentProject()->experiment()->peakFilter();
    ohkl::PeakCollection* collection = _peak_combo->currentPeakCollection();
    filter->resetFiltering(collection);
    ohkl::PeakHistogramType type =
        static_cast<ohkl::PeakHistogramType>(_histo_combo->currentIndex());

    switch (type) {
    case ohkl::PeakHistogramType::Intensity:
        break;
    case ohkl::PeakHistogramType::Sigma:
        break;
    case ohkl::PeakHistogramType::Strength:
        filter->flags()->strength = true;
        filter->parameters()->strength_min = xmin;
        filter->parameters()->strength_max = xmax;
        break;
    }


    filter->filter(collection);

    refreshPeakTable();

    int n_peaks = _peak_collection_item.numberOfPeaks();
    int n_caught = _peak_collection_item.numberCaughtByFilter();

    gGui->statusBar()->showMessage(
        QString::number(n_caught) + "/" + QString::number(n_peaks) + " caught by filter");
}

DetectorWidget* SubframeReject::detectorWidget()
{
    return _detector_widget;
}
