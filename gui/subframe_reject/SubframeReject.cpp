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
#include "gui/MainWin.h" // gGui
#include "gui/frames/ProgressView.h"
#include "gui/graphics/DetectorScene.h"
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

SubframeReject::SubframeReject() : QWidget()
{
    auto main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);

    _left_layout = new QVBoxLayout();

    setInputUp();
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
}

void SubframeReject::setInputUp()
{
    auto input_box = new Spoiler("Input");
    GridFiller f(input_box, true);

    _data_combo = f.addDataCombo("Data set");
    _peak_combo = f.addPeakCombo(ComboType::PeakCollection, "Peaks to integrate");

    connect(
        _peak_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeReject::toggleUnsafeWidgets);

    _left_layout->addWidget(input_box);
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
            peak->peak()->enabled() ? _peak_view_widget->set1 : _peak_view_widget->set2);
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


DetectorWidget* SubframeReject::detectorWidget()
{
    return _detector_widget;
}
