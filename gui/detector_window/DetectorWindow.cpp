//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/detector_window/DetectorWindow.h
//! @brief     Defines class DetectorWindow
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/detector_window/DetectorWindow.h"

#include "core/experiment/Experiment.h"
#include "gui/graphics/DetectorScene.cpp"
#include "gui/graphics/DetectorView.cpp"
#include "gui/models/Project.h"
#include "gui/utility/ColorButton.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/PropertyScrollArea.h"
#include "gui/utility/Spoiler.h"
#include "gui/views/ShortTable.h"
#include "gui/widgets/PeakViewWidget.h"

#include <QCheckBox>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QScrollArea>

DetectorWindow::DetectorWindow(QWidget* parent)
    : QDialog(parent)
    , _peak_collection_item_1()
    , _peak_collection_model_1()
    , _peak_collection_item_2()
    , _peak_collection_model_2()
{
    auto main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);

    _control_layout = new QVBoxLayout();
    _peak_view_widget_1 = new PeakViewWidget("Valid peaks", "Invalid Peaks");
    _peak_view_widget_2 = new PeakViewWidget("Valid peaks", "Invalid Peaks");
    _peak_view_widget_2->set1.setColor(Qt::darkGreen);
    _peak_view_widget_2->set2.setColor(Qt::darkRed);
    _peak_view_widget_2->set1.setIntegrationRegionColors(Qt::darkGreen, Qt::darkYellow, 0.5);

    setDetectorViewUp();
    setPeakTableUp();
    setInputUp();
    setPlotUp(_peak_view_widget_1, "Show/hide peak collection 1");
    setPlotUp(_peak_view_widget_2, "Show/hide peak collection 2");

    auto propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_control_layout);
    main_layout->addWidget(propertyScrollArea);
    main_layout->addWidget(_right_element);
}

void DetectorWindow::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);
    setGeometry(gGui->x() + 40, gGui->y() + 80, gGui->width(), gGui->height());
}

void DetectorWindow::setDetectorViewUp()
{
    QGroupBox* detector_group = new QGroupBox("Detector image");
    QGridLayout* detector_grid = new QGridLayout(detector_group);

    _detector_view = new DetectorView();
    _detector_view->getScene()->linkPeakModel(&_peak_collection_model_1);
    _detector_view->getScene()->linkPeakModel(&_peak_collection_model_2);
    _detector_view->scale(1, -1);
    detector_grid->addWidget(_detector_view, 0, 0, 1, 2);

    _detector_scroll = new QScrollBar();
    _detector_scroll->setOrientation(Qt::Horizontal);
    _detector_scroll->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    detector_grid->addWidget(_detector_scroll, 1, 0, 1, 1);

    _detector_spin = new QSpinBox();
    _detector_spin->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    detector_grid->addWidget(_detector_spin, 1, 1, 1, 1);

    connect(
        _detector_scroll, SIGNAL(valueChanged(int)), _detector_view->getScene(),
        SLOT(slotChangeSelectedFrame(int)));

    connect(_detector_scroll, SIGNAL(valueChanged(int)), _detector_spin, SLOT(setValue(int)));

    connect(_detector_spin, SIGNAL(valueChanged(int)), _detector_scroll, SLOT(setValue(int)));

    connect(
        _detector_view->getScene(), &DetectorScene::signalSelectedPeakItemChanged, this,
        &DetectorWindow::changeSelected);

    _right_element->addWidget(detector_group);
}

void DetectorWindow::setPeakTableUp()
{
    QVBoxLayout* table_layout = new QVBoxLayout();

    _peak_table_1 = new ShortTable(this);
    _peak_collection_model_1.setRoot(&_peak_collection_item_1);
    _peak_table_1->setModel(&_peak_collection_model_1);
    _peak_table_1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

    _peak_table_2 = new ShortTable(this, _peak_table_1->height());
    _peak_collection_model_2.setRoot(&_peak_collection_item_2);
    _peak_table_2->setModel(&_peak_collection_model_2);
    _peak_table_2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

    table_layout->addWidget(_peak_table_1);
    table_layout->addWidget(_peak_table_2);

    _right_element->addWidget(_peak_table_1);
    _right_element->addWidget(_peak_table_2);
}

void DetectorWindow::setInputUp()
{
    Spoiler* input_spoiler = new Spoiler("Input Data");
    GridFiller f(input_spoiler, true);

    _exp_combo = f.addCombo("Experiment:");
    _data_combo = f.addCombo("Data set:");
    _peak_combo_1 = f.addCombo("Peak collection 1:");
    _peak_combo_2 = f.addCombo("Peak collection 2:");

    connect(
        _exp_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &DetectorWindow::refreshAll);

    connect(
        _peak_combo_1, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &DetectorWindow::refreshPeakTable);

    connect(
        _peak_combo_2, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &DetectorWindow::refreshPeakTable);

    _control_layout->addWidget(input_spoiler);
}

void DetectorWindow::setPlotUp(PeakViewWidget* peak_widget, QString name)
{
    Spoiler* preview_spoiler = new Spoiler(name);

    connect(
        peak_widget, &PeakViewWidget::settingsChanged, this, &DetectorWindow::refreshDetectorView);

    preview_spoiler->setContentLayout(*peak_widget, true);

    _control_layout->addWidget(preview_spoiler);
}

void DetectorWindow::refreshDetectorView()
{
    if (_peak_collection_item_1.childCount() == 0)  // #nsxAudit Really? what if _peak_collection_item_2.childCount() is not empty?
        return;

    for (int i = 0; i < _peak_collection_item_1.childCount(); i++) {
        PeakItem* peak = _peak_collection_item_1.peakItemAt(i);
        auto graphic = peak->peakGraphic();

        graphic->showLabel(false);
        graphic->setColor(Qt::transparent);
        graphic->initFromPeakViewWidget(
            peak->peak()->enabled() ? _peak_view_widget_1->set1 : _peak_view_widget_1->set2);
    }

    for (int i = 0; i < _peak_collection_item_2.childCount(); i++) {
        PeakItem* peak = _peak_collection_item_2.peakItemAt(i);
        auto graphic = peak->peakGraphic();

        graphic->showLabel(false);
        graphic->setColor(Qt::transparent);
        graphic->initFromPeakViewWidget(
            peak->peak()->enabled() ? _peak_view_widget_2->set1 : _peak_view_widget_2->set2);
    }

    _detector_view->getScene()->update();
    _detector_view->getScene()->initIntRegionFromPeakWidget(_peak_view_widget_1->set1);
    _detector_view->getScene()->initIntRegionFromPeakWidget(_peak_view_widget_2->set1, true);
    _detector_view->getScene()->drawPeakitems();
}

void DetectorWindow::refreshPeakTable()
{
    auto expt = gSession->experimentAt(_exp_combo->currentIndex())->experiment();
    _peak_collection_1 = expt->getPeakCollection(_peak_combo_1->currentText().toStdString());

    if (!_peak_collection_1)
        return;

    _detector_view->getScene()->clearPeakItems();
    _peak_collection_item_1.setPeakCollection(_peak_collection_1);
    _peak_collection_model_1.setRoot(&_peak_collection_item_1);

    QString collection_2 = _peak_combo_2->currentText();
    if (!collection_2.isEmpty()) {
        _peak_collection_2 = expt->getPeakCollection(collection_2.toStdString());
        _peak_collection_item_2.setPeakCollection(_peak_collection_2);
        _peak_collection_model_2.setRoot(&_peak_collection_item_2);
    }

    refreshDetectorView();
}

void DetectorWindow::refreshAll()
{
    updateExptList();
}

void DetectorWindow::updateExptList()
{
    _exp_combo->blockSignals(true);
    _exp_combo->clear();

    if (!gSession->experimentNames().empty()) {
        for (const QString& exp : gSession->experimentNames())
            _exp_combo->addItem(exp);
        updateDatasetList();
        updatePeakList();
        refreshPeakTable();
    }
    _exp_combo->blockSignals(false);
}

void DetectorWindow::updateDatasetList()
{
    _data_combo->blockSignals(true);
    _data_combo->clear();
    _data_list = gSession->experimentAt(_exp_combo->currentIndex())->allData();

    if (!_data_list.empty()) {
        for (const nsx::sptrDataSet& data : _data_list) {
            QFileInfo fileinfo(QString::fromStdString(data->filename()));
            _data_combo->addItem(fileinfo.baseName());
        }
        _data_combo->setCurrentIndex(0);
        updateDatasetParameters(0);
    }
    _data_combo->blockSignals(false);
}

void DetectorWindow::updateDatasetParameters(int idx)
{
    if (_data_list.empty() || idx < 0)
        return;

    nsx::sptrDataSet data = _data_list.at(idx);

    _detector_view->getScene()->slotChangeSelectedData(_data_list.at(idx), 0);
    emit _detector_view->getScene()->dataChanged();
    _detector_view->getScene()->update();

    _detector_scroll->setMaximum(data->nFrames() - 1);
    _detector_scroll->setMinimum(0);

    _detector_spin->setMaximum(data->nFrames() - 1);
    _detector_spin->setMinimum(0);
}

void DetectorWindow::updatePeakList()
{
    _peak_combo_1->blockSignals(true);
    _peak_combo_1->clear();
    _peak_list.clear();
    _peak_list = gSession->experimentAt(_exp_combo->currentIndex())->getPeakListNames();

    if (!_peak_list.empty()) {
        _peak_combo_1->addItems(_peak_list);
        _peak_combo_1->setCurrentIndex(0);
    }
    _peak_combo_1->blockSignals(false);

    _peak_combo_2->blockSignals(true);
    _peak_combo_2->clear();

    _peak_list.clear();
    _peak_list = gSession->experimentAt(_exp_combo->currentIndex())->getPeakListNames();

    _peak_list.push_front("");
    _peak_combo_2->addItems(_peak_list);
    _peak_combo_2->setCurrentIndex(0);
    _peak_combo_2->blockSignals(false);
}

void DetectorWindow::changeSelected(PeakItemGraphic* peak_graphic)
{
    int row_1 = _peak_collection_item_1.returnRowOfVisualItem(peak_graphic);
    QModelIndex index_1 = _peak_collection_model_1.index(row_1, 0);
    int row_2 = _peak_collection_item_2.returnRowOfVisualItem(peak_graphic);
    QModelIndex index_2 = _peak_collection_model_2.index(row_2, 0);
    if (row_1 > 0 && row_2 == 0) {
        _peak_table_1->selectRow(row_1);
        _peak_table_1->scrollTo(index_1, QAbstractItemView::PositionAtTop);
    } else if (row_1 == 0 && row_2 > 0) {
        _peak_table_2->selectRow(row_2);
        _peak_table_2->scrollTo(index_2, QAbstractItemView::PositionAtTop);
    }
}
