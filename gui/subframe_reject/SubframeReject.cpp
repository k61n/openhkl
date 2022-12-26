//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_reject/SubframeReject.cpp

//! @brief     Implements class SubframeReject
//!
//! @homepage  https://openhkl.org
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
#include "gui/graphics/PeakCollectionGraphics.h"
#include "gui/graphics/SXPlot.h"
#include "gui/graphics_items/PeakItemGraphic.h"
#include "gui/items/PeakItem.h"
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
#include "tables/crystal/MillerIndex.h"

#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QItemSelectionModel>
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
    setFindUp();
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
    connect(
        _peak_view_widget, &PeakViewWidget::settingsChanged, _detector_widget,
        &DetectorWidget::refresh);
    connect(_peak_combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]() {
        updateStatistics();
        computeHistogram();
        _plot_widget->sxplot()->resetZoom();
    });
    connect(_histo_combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]() {
        updateStatistics();
        computeHistogram();
    });

    auto propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_left_layout);
    main_layout->addWidget(propertyScrollArea);
    main_layout->addWidget(_right_element);

    _right_element->setStretchFactor(0, 2);
    _right_element->setStretchFactor(1, 1);
    _right_element->setStretchFactor(2, 2);

    _peak_stats = ohkl::PeakStatistics();

    _selection_color = Qt::black;
}

void SubframeReject::setInputUp()
{
    auto* input_box = new Spoiler("Input");
    GridFiller f(input_box, true);

    _data_combo = f.addDataCombo("Data set");
    _peak_combo = f.addPeakCombo(ComboType::PeakCollection, "Peak collection");

    connect(
        _peak_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeReject::toggleUnsafeWidgets);

    _left_layout->addWidget(input_box);
}

void SubframeReject::setFindUp()
{
    auto* find_box = new Spoiler("Find peak by index");
    GridFiller gfiller(find_box, true);

    _find_h = gfiller.addSpinBox("h", "h Miller index");
    _find_k = gfiller.addSpinBox("k", "k Miller index");
    _find_l = gfiller.addSpinBox("l", "l Miller index");
    _find_by_index = gfiller.addButton("Find peak", "Find peak by miller index (hkl)");

    connect(_find_by_index, &QPushButton::clicked, this, &SubframeReject::findByIndex);

    _left_layout->addWidget(find_box);
}

void SubframeReject::setHistogramUp()
{
    auto* histo_spoiler = new Spoiler("Histogram");
    GridFiller filler(histo_spoiler, true);

    _histo_combo = filler.addCombo("Histogram type");
    for (const auto& [type, description] : _peak_stats.getHistoStrings())
        _histo_combo->addItem(QString::fromStdString(description));

    _n_bins = filler.addSpinBox("Number of bins", "Number of histogram bins");
    std::tie(_freq_min, _freq_max) =
        filler.addSpinBoxPair("Frequency range", "Maximum and minimum frequecies for histogram");
    std::tie(_x_min, _x_max) =
        filler.addSpinBoxPair("Data range", "Minimum and maximum of x data series");
    _log_freq = filler.addCheckBox("Logarithmic vertical axis", "Switch to log-linear plot", 1);
    _plot_histogram = filler.addButton("Replot", "Refresh histogram");
    _sigma_factor =
        filler.addDoubleSpinBox("Threshold", "Threshold for peak rejection in standard deviations");
    _reject_outliers =
        filler.addButton("Reject outliers", "Reject peaks outside specified threshold");

    _n_bins->setMaximum(10000);
    _x_min->setMaximum(10000);
    _x_max->setMaximum(10000);
    _freq_min->setMaximum(10000);
    _freq_max->setMaximum(10000);
    _sigma_factor->setMaximum(5);

    _x_min->setMinimum(-10000);
    _x_max->setMinimum(-10000);

    _n_bins->setValue(100);
    _x_min->setValue(0);
    _x_max->setValue(10000);
    _freq_min->setValue(0);
    _freq_max->setValue(1000);
    _sigma_factor->setValue(3);

    connect(_log_freq, &QCheckBox::stateChanged, this, [=]() {
        updateStatistics();
        computeHistogram();
    });
    connect(_plot_histogram, &QPushButton::clicked, this, &SubframeReject::computeHistogram);
    connect(_reject_outliers, &QPushButton::clicked, this, &SubframeReject::rejectOutliers);

    _left_layout->addWidget(histo_spoiler);
}

void SubframeReject::setFigureUp()
{
    QGroupBox* figure_group = new QGroupBox("Detector image");
    figure_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _detector_widget = new DetectorWidget(1, false, true, figure_group);
    _detector_widget->setVisualisationMode(VisualisationType::Filtered);
    _detector_widget->linkPeakModel(&_peak_collection_model, _peak_view_widget);

    // connect(
    //     _detector_widget->scene(), &DetectorScene::signalSelectedPeakItemChanged, this,
    //     &SubframeReject::changeSelected);
    connect(
        _peak_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeReject::refreshPeakTable);

    _right_element->addWidget(figure_group);
}

void SubframeReject::setPlotUp()
{
    _plot_widget = new PlotPanel;

    connect(_plot_widget, &PlotPanel::signalXRangeChanged, this, &SubframeReject::filterSelection);
    connect(_plot_widget, &PlotPanel::signalXRangeChanged, this, &SubframeReject::updateXRange);
    connect(_plot_widget, &PlotPanel::signalYRangeChanged, this, &SubframeReject::updateYRange);

    _right_element->addWidget(_plot_widget);
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

    peak_grid->addWidget(_peak_table, 0, 0, 0, 0);

    _right_element->addWidget(peak_group);

    connect(
        _peak_table->selectionModel(), &QItemSelectionModel::selectionChanged, this,
        &SubframeReject::onPeakTableSelection);
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
    _peak_table->model()->sort(PeakColumn::Filtered, Qt::DescendingOrder);

    _detector_widget->refresh();
}

void SubframeReject::refreshAll()
{
    toggleUnsafeWidgets();
    if (!gSession->hasProject())
        return;


    _data_combo->refresh();
    _detector_widget->refresh();
    _peak_combo->refresh();
    refreshPeakTable();
    toggleUnsafeWidgets();

    if (!gSession->currentProject()->hasDataSet()
        || !gSession->currentProject()->hasPeakCollection())
        return;
    updateStatistics();
    computeHistogram();
    _plot_widget->sxplot()->resetZoom();
}

void SubframeReject::setPreviewUp()
{
    Spoiler* preview_spoiler = new Spoiler("Show/hide peaks");
    _peak_view_widget = new PeakViewWidget("Valid peaks", "Invalid Peaks");

    preview_spoiler->setContentLayout(*_peak_view_widget);

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
    bool hasPeaks = false;
    if (gSession->hasProject())
        hasPeaks = gSession->currentProject()->hasPeakCollection();

    if (hasPeaks)
        if (!_peak_combo->currentPeakCollection()->isIndexed())
            hasPeaks = false;

    _find_h->setEnabled(hasPeaks);
    _find_k->setEnabled(hasPeaks);
    _find_l->setEnabled(hasPeaks);
    _find_by_index->setEnabled(hasPeaks);

    if (hasPeaks) {
        int h_max = 0;
        int k_max = 0;
        int l_max = 0;
        int h_min = 0;
        int k_min = 0;
        int l_min = 0;
        for (ohkl::Peak3D* peak : _peak_combo->currentPeakCollection()->getPeakList()) {
            int h = peak->hkl().h();
            int k = peak->hkl().k();
            int l = peak->hkl().l();
            if (h < h_min)
                h_min = h;
            if (h > h_max)
                h_max = h;
            if (k < k_min)
                k_min = k;
            if (k > k_max)
                k_max = k;
            if (l < l_min)
                l_min = l;
            if (l > l_max)
                l_max = l;
        }
        _find_h->setMaximum(h_max);
        _find_h->setMinimum(h_min);
        _find_k->setMaximum(k_max);
        _find_k->setMinimum(k_min);
        _find_l->setMaximum(l_max);
        _find_l->setMinimum(l_min);
    }
}

void SubframeReject::findByIndex()
{
    std::vector<PeakItem*> items;
    ohkl::MillerIndex index_to_find = {_find_h->value(), _find_k->value(), _find_l->value()};
    for (int i = 0; i < _peak_collection_item.childCount(); i++) {
        PeakItem* item = _peak_collection_item.peakItemAt(i);
        ohkl::Peak3D* peak = item->peak();
        if (peak->hkl() == index_to_find)
            items.emplace_back(item);
    }
    if (items.empty()) {
        gGui->statusBar()->showMessage(QString::fromStdString(
            "No peak with this Miller index in collection "
            + _peak_combo->currentPeakCollection()->name()));
    } else if (items.size() == 1) {
        changeSelected(items[0]->peakGraphic());
    } else {
        gGui->statusBar()->showMessage(QString::fromStdString(
            "Multiple peaks with this Miller index in collection "
            + _peak_combo->currentPeakCollection()->name()));
    }
}

void SubframeReject::updateStatistics()
{
    ohkl::PeakCollection* peaks = _peak_combo->currentPeakCollection();
    if (!peaks->isIntegrated())
        return;

    ohkl::PeakHistogramType type =
        static_cast<ohkl::PeakHistogramType>(_histo_combo->currentIndex());
    _peak_stats.setPeakCollection(peaks, type);
    updatePlotRange();
}

void SubframeReject::computeHistogram()
{
    ohkl::PeakCollection* peaks = _peak_combo->currentPeakCollection();
    if (!peaks->isIntegrated())
        return;

    _current_histogram = _peak_stats.computeHistogram(_n_bins->value());

    ohkl::PeakHistogramType type =
        static_cast<ohkl::PeakHistogramType>(_histo_combo->currentIndex());
    if (type == ohkl::PeakHistogramType::BkgGradient ||
        type == ohkl::PeakHistogramType::BkgGradientSigma) {
        if (!peaks->hasBkgGradient()) {
            gGui->statusBar()->showMessage(
                "Can't filter by intensity: background gradients not integrated");
            return;
        }
    }

    QString xLabel = QString::fromStdString(_peak_stats.getHistoStrings().find(type)->second);
    _plot_widget->setYLog(_log_freq->isChecked());
    _plot_widget->plotData(
        _current_histogram, xLabel, QString("Frequency"), _x_min->value(), _x_max->value(),
        _freq_min->value(), _freq_max->value());
}

void SubframeReject::filterSelection(double xmin, double xmax)
{
    gGui->setReady(false);
    ohkl::PeakFilter* filter = gSession->currentProject()->experiment()->peakFilter();
    ohkl::PeakCollection* collection = _peak_combo->currentPeakCollection();
    filter->resetFiltering(collection);
    ohkl::PeakHistogramType type =
        static_cast<ohkl::PeakHistogramType>(_histo_combo->currentIndex());

    switch (type) {
        case ohkl::PeakHistogramType::Intensity:
            if (!collection->isIntegrated()) {
                gGui->statusBar()->showMessage(
                    "Can't filter by intensity: peaks are not integrated");
                return;
            }
            filter->flags()->intensity = true;
            filter->parameters()->intensity_min = xmin;
            filter->parameters()->intensity_max = xmax;
            break;
        case ohkl::PeakHistogramType::Sigma:
            if (!collection->isIntegrated()) {
                gGui->statusBar()->showMessage(
                    "Can't filter by intensity: peaks are not integrated");
                return;
            }
            filter->flags()->sigma = true;
            filter->parameters()->sigma_min = xmin;
            filter->parameters()->sigma_max = xmax;
            break;
        case ohkl::PeakHistogramType::Strength:
            if (!collection->isIntegrated()) {
                gGui->statusBar()->showMessage(
                    "Can't filter by intensity: peaks are not integrated");
                return;
            }
            filter->flags()->strength = true;
            filter->parameters()->strength_min = xmin;
            filter->parameters()->strength_max = xmax;
            break;
        case ohkl::PeakHistogramType::BkgGradient:
            if (!collection->hasBkgGradient()) {
                gGui->statusBar()->showMessage(
                    "Can't filter by intensity: background gradients not integrated");
                return;
            }
            filter->flags()->gradient = true;
            filter->parameters()->gradient_min = xmin;
            filter->parameters()->gradient_max = xmax;
            break;
        case ohkl::PeakHistogramType::BkgGradientSigma:
            if (!collection->hasBkgGradient()) {
                gGui->statusBar()->showMessage(
                    "Can't filter by intensity: background gradients not integrated");
                return;
            }
            filter->flags()->gradient_sigma = true;
            filter->parameters()->gradient_sigma_min = xmin;
            filter->parameters()->gradient_sigma_max = xmax;
            break;
    }
    filter->filter(collection);

    refreshPeakTable();

    int n_peaks = _peak_collection_item.numberOfPeaks();
    int n_caught = _peak_collection_item.numberCaughtByFilter();

    gGui->statusBar()->showMessage(
        QString::number(n_caught) + "/" + QString::number(n_peaks) + " caught by filter");
    gGui->setReady(true);
}

void SubframeReject::updateXRange(double xmin, double xmax)
{
    _x_min->setValue(xmin);
    _x_max->setValue(xmax);
}

void SubframeReject::updateYRange(double ymin, double ymax)
{
    _freq_min->setValue(ymin);
    _freq_max->setValue(ymax);
}

void SubframeReject::onPeakTableSelection()
{
    for (std::size_t idx = 0; idx < _selected_graphics.size(); ++idx)
        _selected_graphics[idx]->setCenterColor(_saved_colors[idx]);
    _selected_graphics.clear();
    _saved_colors.clear();
    QModelIndexList selected_rows = _peak_table->selectionModel()->selectedRows();
    for (auto row : selected_rows) {
        int idx = row.row();
        PeakItemGraphic* peak_graphic = _peak_collection_item.peakItemAt(idx)->peakGraphic();
        _saved_colors.push_back(peak_graphic->centerColor());
        peak_graphic->setCenterColor(_selection_color);
        _selected_graphics.emplace_back(peak_graphic);
    }
    PeakItemGraphic* peak_graphic = _selected_graphics[0];
    unsigned int frame = static_cast<unsigned int>(peak_graphic->peak()->shape().center()[2]);
    _detector_widget->spin()->setValue(frame);
}

void SubframeReject::updatePlotRange()
{
    _x_min->setMaximum(_peak_stats.maxValue());
    _x_max->setMaximum(_peak_stats.maxValue());

    _x_min->setMinimum(_peak_stats.minValue());
    _x_max->setMinimum(_peak_stats.minValue());

    _x_max->setValue(_peak_stats.maxValue());
    _x_min->setValue(_peak_stats.minValue());

    if (_peak_stats.hasHistogram()) {
        _freq_min->setMinimum(0);
        _freq_max->setMinimum(0);

        _freq_min->setMaximum(_peak_stats.maxCount());
        _freq_max->setMaximum(_peak_stats.maxCount());

        _freq_max->setValue(_peak_stats.maxCount());
        _freq_min->setValue(0);
    }
}

void SubframeReject::rejectOutliers()
{
    std::vector<ohkl::Peak3D*> outliers = _peak_stats.findOutliers(_sigma_factor->value());
    ohkl::PeakFilter* filter = gSession->currentProject()->experiment()->peakFilter();
    ohkl::PeakCollection* collection = _peak_combo->currentPeakCollection();
    filter->resetFiltering(collection);

    for (auto* peak : outliers) {
        peak->setSelected(false);
        peak->setRejectionFlag(ohkl::RejectionFlag::Outlier);
        peak->caughtYou(true); // For sorting in the peak table
    }
    refreshPeakTable();
    gGui->statusBar()->showMessage(
        QString::number(outliers.size()) + "/" + QString::number(collection->numberOfPeaks())
        + " outliers rejected");
}

DetectorWidget* SubframeReject::detectorWidget()
{
    return _detector_widget;
}
