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
#include "gui/utility/Spoiler.h"
#include "gui/widgets/PeakViewWidget.h"
#include "gui/views/PeakTableView.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QLabel>
#include <QFileInfo>

DetectorWindow::DetectorWindow(QWidget* parent)
    : QDialog(parent)
    , _peak_collection_item()
    , _peak_collection_model()
{
    setSizePolicies();

    _main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);

    QScrollArea* scroll_area = new QScrollArea(this);
    QWidget* scroll_widget = new QWidget();
    scroll_widget->setSizePolicy(*_size_policy_box);
    scroll_area->setSizePolicy(*_size_policy_box);
    scroll_area->setWidgetResizable(true);
    scroll_area->setWidget(scroll_widget);
    _control_layout = new QVBoxLayout(scroll_widget);

    setDetectorViewUp();
    setPeakTableUp();
    setInputUp();
    setPlotUp();
    _control_layout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    _main_layout->addWidget(scroll_area);
    _main_layout->addWidget(_right_element);
}

void DetectorWindow::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    setGeometry(gGui->x()+40, gGui->y()+80, gGui->width(), gGui->height());
}

void DetectorWindow::setSizePolicies()
{
    _size_policy_widgets = new QSizePolicy();
    _size_policy_widgets->setHorizontalPolicy(QSizePolicy::Preferred);
    _size_policy_widgets->setVerticalPolicy(QSizePolicy::Fixed);

    _size_policy_box = new QSizePolicy();
    _size_policy_box->setHorizontalPolicy(QSizePolicy::Fixed);
    _size_policy_box->setVerticalPolicy(QSizePolicy::Expanding);

    _size_policy_right = new QSizePolicy();
    _size_policy_right->setHorizontalPolicy(QSizePolicy::Expanding);
    _size_policy_right->setVerticalPolicy(QSizePolicy::Expanding);

    _size_policy_fixed = new QSizePolicy();
    _size_policy_fixed->setHorizontalPolicy(QSizePolicy::Fixed);
    _size_policy_fixed->setVerticalPolicy(QSizePolicy::Fixed);
}

void DetectorWindow::setDetectorViewUp()
{
    QGroupBox* detector_group = new QGroupBox("Detector image");
    QGridLayout* detector_grid = new QGridLayout(detector_group);

    _detector_view = new DetectorView();
    _detector_view->getScene()->linkPeakModel(&_peak_collection_model);
    _detector_view->scale(1, -1);
    detector_grid->addWidget(_detector_view, 0, 0, 1, 2);

    _detector_scroll = new QScrollBar();
    _detector_scroll->setOrientation(Qt::Horizontal);
    _detector_scroll->setSizePolicy(*_size_policy_widgets);
    detector_grid->addWidget(_detector_scroll, 1, 0, 1, 1);

    _detector_spin = new QSpinBox();
    _detector_spin->setSizePolicy(*_size_policy_fixed);
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
    QGroupBox* peak_group = new QGroupBox("Peaks");
    QGridLayout* peak_grid = new QGridLayout(peak_group);

    peak_group->setSizePolicy(*_size_policy_right);

    _peak_table = new PeakTableView(this);
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->setModel(&_peak_collection_model);

    peak_grid->addWidget(_peak_table, 0, 0, 0, 0);

    _right_element->addWidget(peak_group);
}

void DetectorWindow::setInputUp()
{
    Spoiler* input_spoiler = new Spoiler("Input Data");

    QGridLayout* input_grid = new QGridLayout();

    QLabel* exp_label = new QLabel("Experiment");
    exp_label->setAlignment(Qt::AlignRight);
    input_grid->addWidget(exp_label, 0, 0, 1, 1);

    QLabel* data_label = new QLabel("Data set");
    data_label->setAlignment(Qt::AlignRight);
    input_grid->addWidget(data_label, 1, 0, 1, 1);

    QLabel* peak_label = new QLabel("Peak collection");
    data_label->setAlignment(Qt::AlignRight);
    input_grid->addWidget(peak_label, 2, 0, 1, 1);

    _exp_combo = new QComboBox();
    _data_combo = new QComboBox();
    _peak_combo = new QComboBox();

    _exp_combo->setMaximumWidth(1000);
    _data_combo->setMaximumWidth(1000);
    _peak_combo->setMaximumWidth(1000);

    _exp_combo->setSizePolicy(*_size_policy_widgets);
    _data_combo->setSizePolicy(*_size_policy_widgets);
    _peak_combo->setSizePolicy(*_size_policy_widgets);

    input_grid->addWidget(_exp_combo, 0, 1, 1, 1);
    input_grid->addWidget(_data_combo, 1, 1, 1, 1);
    input_grid->addWidget(_peak_combo, 2, 1, 1, 1);

    input_spoiler->setContentLayout(*input_grid);
    input_spoiler->setSizePolicy(*_size_policy_box);
    input_spoiler->contentArea.setSizePolicy(*_size_policy_box);

    connect(
        _exp_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &DetectorWindow::refreshAll);

    connect(
        _peak_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &DetectorWindow::refreshPeakTable);

    _control_layout->addWidget(input_spoiler);
    input_spoiler->toggler(true);
}

void DetectorWindow::setPlotUp()
{
    Spoiler* preview_spoiler = new Spoiler("Show/hide peaks");
    _peak_view_widget = new PeakViewWidget("Valid peaks", "Invalid Peaks");

    connect(
        _peak_view_widget->drawPeaks1(), &QCheckBox::stateChanged, this,
        &DetectorWindow::refreshDetectorView);
    connect(
        _peak_view_widget->drawPeaks2(), &QCheckBox::stateChanged, this,
        &DetectorWindow::refreshDetectorView);
    connect(
        _peak_view_widget->drawBoxes1(), &QCheckBox::stateChanged, this,
        &DetectorWindow::refreshDetectorView);
    connect(
        _peak_view_widget->drawBoxes2(), &QCheckBox::stateChanged, this,
        &DetectorWindow::refreshDetectorView);
    connect(
        _peak_view_widget->drawBkg1(), &QCheckBox::stateChanged, this,
        &DetectorWindow::refreshDetectorView);
    connect(
        _peak_view_widget->drawBkg2(), &QCheckBox::stateChanged, this,
        &DetectorWindow::refreshDetectorView);
    connect(
        _peak_view_widget->peakSize1(),
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &DetectorWindow::refreshDetectorView);
    connect(
        _peak_view_widget->peakSize2(),
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &DetectorWindow::refreshDetectorView);
    connect(
        _peak_view_widget->peakColor1(), &ColorButton::colorChanged, this,
        &DetectorWindow::refreshDetectorView);
    connect(
        _peak_view_widget->peakColor2(), &ColorButton::colorChanged, this,
        &DetectorWindow::refreshDetectorView);
    connect(
        _peak_view_widget->boxColor1(), &ColorButton::colorChanged, this,
        &DetectorWindow::refreshDetectorView);
    connect(
        _peak_view_widget->boxColor2(), &ColorButton::colorChanged, this,
        &DetectorWindow::refreshDetectorView);
    connect(
        _peak_view_widget->bkgColor1(), &ColorButton::colorChanged, this,
        &DetectorWindow::refreshDetectorView);
    connect(
        _peak_view_widget->bkgColor2(), &ColorButton::colorChanged, this,
        &DetectorWindow::refreshDetectorView);

    preview_spoiler->setContentLayout(*_peak_view_widget);
    preview_spoiler->setSizePolicy(*_size_policy_box);
    preview_spoiler->contentArea.setSizePolicy(*_size_policy_box);

    _control_layout->addWidget(preview_spoiler);
    preview_spoiler->toggler(true);
}

void DetectorWindow::refreshDetectorView()
{
    if (_peak_collection_item.childCount() == 0)
        return;

    bool valid;
    PeakItemGraphic* graphic;

    for (int i = 0; i < _peak_collection_item.childCount(); i++) {
        PeakItem* peak = _peak_collection_item.peakItemAt(i);
        graphic = peak->peakGraphic();
        valid = peak->peak()->enabled();

        if (valid) {
            graphic->showLabel(false);
            graphic->showArea(_peak_view_widget->drawPeaks1()->isChecked());
            graphic->setSize(_peak_view_widget->peakSize1()->value());
            graphic->setColor(Qt::transparent);
            graphic->setOutlineColor(_peak_view_widget->peakColor1()->getColor());
            graphic->showBox(_peak_view_widget->drawBoxes1()->isChecked());
            graphic->setBoxColor(_peak_view_widget->boxColor1()->getColor());
            graphic->showBox(_peak_view_widget->drawBkg1()->isChecked());
            graphic->setBoxColor(_peak_view_widget->bkgColor1()->getColor());
        } else {
            graphic->showLabel(false);
            graphic->showArea(_peak_view_widget->drawPeaks2()->isChecked());
            graphic->setSize(_peak_view_widget->peakSize2()->value());
            graphic->setColor(Qt::transparent);
            graphic->setOutlineColor(_peak_view_widget->peakColor2()->getColor());
            graphic->showBox(_peak_view_widget->drawBoxes2()->isChecked());
            graphic->setBoxColor(_peak_view_widget->boxColor2()->getColor());
            graphic->showBox(_peak_view_widget->drawBkg2()->isChecked());
            graphic->setBoxColor(_peak_view_widget->bkgColor2()->getColor());
        }
    }
    _detector_view->getScene()->update();
    _detector_view->getScene()->drawPeakitems();
}

void DetectorWindow::refreshPeakTable()
{
    auto expt = gSession->experimentAt(_exp_combo->currentIndex())->experiment();
    _peak_collection = expt->getPeakCollection(_peak_combo->currentText().toStdString());

    _detector_view->getScene()->clearPeakItems();
    _peak_collection_item.setPeakCollection(_peak_collection);
    _peak_collection_model.setRoot(&_peak_collection_item);

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
        for (QString exp : gSession->experimentNames())
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
    _peak_combo->blockSignals(true);
    _peak_combo->clear();
    _peak_list.clear();
    _peak_list = gSession->experimentAt(_exp_combo->currentIndex())->getPeakListNames();

    if (!_peak_list.empty()) {
        _peak_combo->addItems(_peak_list);
        _peak_combo->setCurrentIndex(0);
    }
    _peak_combo->blockSignals(false);
}

void DetectorWindow::changeSelected(PeakItemGraphic* peak_graphic)
{
    int row = _peak_collection_item.returnRowOfVisualItem(peak_graphic);
    QModelIndex index = _peak_collection_model.index(row, 0);
    _peak_table->selectRow(row);
    _peak_table->scrollTo(index, QAbstractItemView::PositionAtTop);
}
