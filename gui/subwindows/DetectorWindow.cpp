//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subwindows/DetectorWindow.h
//! @brief     Defines class DetectorWindow
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subwindows/DetectorWindow.h"

#include "core/experiment/Experiment.h"
#include "gui/graphics/DetectorScene.cpp"
#include "gui/graphics/DetectorView.cpp"
#include "gui/items/PeakCollectionItem.h"
#include "gui/models/PeakCollectionModel.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/CellComboBox.h"
#include "gui/utility/ColorButton.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/LinkedComboBox.h"
#include "gui/utility/PeakComboBox.h"
#include "gui/utility/PropertyScrollArea.h"
#include "gui/utility/SafeSpinBox.h"
#include "gui/utility/Spoiler.h"
#include "gui/views/ShortTable.h"
#include "gui/widgets/DetectorWidget.h"
#include "gui/widgets/PeakViewWidget.h"

#include <QCheckBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QSettings>

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
    _peak_view_widget_2->set1.setIntegrationRegionColors(Qt::darkYellow, Qt::darkGreen, 0.2);

    setDetectorViewUp();
    setPeakTableUp();
    setInputUp();
    set3rdPartyPeaksUp();
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
    detector_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _detector_widget = new DetectorWidget(false, true, true, detector_group);
    _detector_widget->linkPeakModel(&_peak_collection_model_1, &_peak_collection_model_2);
    _detector_widget->cursorCombo()->addItems(
        QStringList{"Cursor mode", "Pixel", "\u03B8", "\u03B3/\u03BD", "d", "Miller Indices"});

    connect(
        _detector_widget->scene(), &DetectorScene::signalSelectedPeakItemChanged, this,
        &DetectorWindow::changeSelected);

    _right_element->addWidget(detector_group);
}

void DetectorWindow::setPeakTableUp()
{
    QVBoxLayout* table_layout = new QVBoxLayout();

    _peak_table_1 = new ShortTable(this);
    _peak_collection_model_1.setRoot(&_peak_collection_item_1);
    _peak_table_1->setModel(&_peak_collection_model_1);
    _peak_table_1->resizeColumnsToContents();
    _peak_table_1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

    _peak_table_2 = new ShortTable(this, _peak_table_1->height());
    _peak_collection_model_2.setRoot(&_peak_collection_item_2);
    _peak_table_2->setModel(&_peak_collection_model_2);
    _peak_table_2->resizeColumnsToContents();
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
    _peak_combo_1 = f.addPeakCombo(ComboType::PeakCollection, "Peak collection 1:");
    _peak_combo_2 = f.addPeakCombo(ComboType::PeakCollection, "Peak collection 2:");
    _peak_combo_1->setEmptyFirst();
    _peak_combo_2->setEmptyFirst();
    _unit_cell_combo = f.addCellCombo("Unit cell");

    connect(
        _exp_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &DetectorWindow::refreshAll);

    connect(
        _peak_combo_1, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &DetectorWindow::refreshPeakTable);

    connect(
        _peak_combo_2, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &DetectorWindow::refreshPeakTable);

    connect(
        _unit_cell_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &DetectorWindow::setUnitCell);

    _control_layout->addWidget(input_spoiler);
}

void DetectorWindow::set3rdPartyPeaksUp()
{
    Spoiler* third_party_spoiler = new Spoiler("Plot 3rd party peaks");
    GridFiller f(third_party_spoiler, false);


    _draw_3rdparty = f.addCheckBox("Plot 3rd party peak centres", 1);
    _draw_3rdparty->setCheckState(Qt::Checked);
    connect(_draw_3rdparty, &QCheckBox::stateChanged, this, &DetectorWindow::refreshDetectorView);

    _3rdparty_size = f.addSpinBox("Size");
    _3rdparty_size->setValue(10);
    connect(
        _3rdparty_size, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &DetectorWindow::refreshDetectorView);

    _3rdparty_color = f.addColorButton(Qt::black, "Color", "3rd party peak color");
    connect(
        _3rdparty_color, &ColorButton::colorChanged, this, &DetectorWindow::refreshDetectorView);

    // _3rdparty_start_frame = f.addSpinBox("Start frame");
    // _3rdparty_start_frame->setValue(0);

    auto load_peaks = f.addButton(
        "Load 3rd party peaks",
        "<font>Load a set of peak centres computed from a 3rd party code (e.g. DENZO .x "
        "file)</font>");
    connect(load_peaks, &QPushButton::clicked, this, &DetectorWindow::load3rdPartyPeaks);

    _control_layout->addWidget(third_party_spoiler);
}

void DetectorWindow::load3rdPartyPeaks()
{
    QSettings s;
    s.beginGroup("RecentDirectories");
    QString loadDirectory = s.value("experiment", QDir::homePath()).toString();

    QStringList files = QFileDialog::getOpenFileNames(
        this, "Load 3rd party peaks file", loadDirectory, "3rd party output (*.x)");

    int current_frame = _detector_widget->scene()->currentFrame();

    if (files.empty())
        return;

    if (files.size() > _nframes) {
        QMessageBox::critical(this, "Error", QString("More .x files than frames in this data set"));
        return;
    }

    if (files.size() > _nframes - current_frame) {
        QMessageBox::critical(this, "Error", QString("Too many .x files selected"));
        return;
    }

    _peakCenterData.init(_nframes);
    for (int i = current_frame; i < (current_frame + files.size()); ++i)
        _peakCenterData.addFrame(files[i].toStdString(), i);

    _detector_widget->scene()->link3rdPartyPeaks(&_peakCenterData);
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
    if (!(_peak_collection_item_1.childCount() == 0)) {
        for (int i = 0; i < _peak_collection_item_1.childCount(); i++) {
            PeakItem* peak = _peak_collection_item_1.peakItemAt(i);
            auto graphic = peak->peakGraphic();

            graphic->showLabel(false);
            graphic->setColor(Qt::transparent);
            graphic->initFromPeakViewWidget(
                peak->peak()->enabled() ? _peak_view_widget_1->set1 : _peak_view_widget_1->set2);
        }
    }

    if (!(_peak_collection_item_2.childCount() == 0)) {
        for (int i = 0; i < _peak_collection_item_2.childCount(); i++) {
            PeakItem* peak = _peak_collection_item_2.peakItemAt(i);
            auto graphic = peak->peakGraphic();

            graphic->showLabel(false);
            graphic->setColor(Qt::transparent);
            graphic->initFromPeakViewWidget(
                peak->peak()->enabled() ? _peak_view_widget_2->set1 : _peak_view_widget_2->set2);
        }
    }

    _detector_widget->scene()->initIntRegionFromPeakWidget(_peak_view_widget_1->set1);
    _detector_widget->scene()->initIntRegionFromPeakWidget(_peak_view_widget_2->set1, true);
    _detector_widget->refresh();

    _detector_widget->scene()->setup3rdPartyPeaks(
        _draw_3rdparty->isChecked(), _3rdparty_color->color(), _3rdparty_size->value());
    _detector_widget->scene()->drawPeakitems();
}

void DetectorWindow::refreshPeakTable()
{
    auto expt = gSession->experimentAt(_exp_combo->currentIndex())->experiment();
    _peak_collection_1 = _peak_combo_1->currentPeakCollection();

    if (_peak_collection_1) {
        _peak_collection_item_1.setPeakCollection(_peak_collection_1);
        _peak_collection_model_1.setRoot(&_peak_collection_item_1);
        _peak_table_1->resizeColumnsToContents();
    } else {
        _peak_collection_item_1.reset();
        _peak_collection_model_1.reset();
    }


    QString collection_2 = _peak_combo_2->currentText();
    if (!collection_2.isEmpty()) {
        _peak_collection_2 = expt->getPeakCollection(collection_2.toStdString());
        _peak_collection_item_2.setPeakCollection(_peak_collection_2);
        _peak_collection_model_2.setRoot(&_peak_collection_item_2);
        _peak_table_2->resizeColumnsToContents();
    } else {
        _peak_collection_item_2.reset();
        _peak_collection_model_2.reset();
    }

    refreshDetectorView();
}

void DetectorWindow::refreshAll()
{
    updateExptList();
}

void DetectorWindow::updateExptList()
{
    if (!gSession->hasProject())
        return;

    QSignalBlocker blocker(_exp_combo);
    QString current_exp = _exp_combo->currentText();
    _exp_combo->clear();

    for (const QString& exp : gSession->experimentNames())
        _exp_combo->addItem(exp);
    _exp_combo->setCurrentText(current_exp);
    updateDatasetList();
    refreshPeakTable();
}

void DetectorWindow::updateDatasetList()
{
    if (!gSession->experimentAt(_exp_combo->currentIndex())->hasDataSet())
        return;

    _nframes = _detector_widget->currentData()->nFrames();
    _peakCenterData.init(_nframes);
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

void DetectorWindow::setUnitCell()
{
    if (!gSession->experimentAt(_exp_combo->currentIndex())->hasUnitCell())
        return;
    nsx::UnitCell* cell = _unit_cell_combo->currentCell().get();
    _detector_widget->scene()->setUnitCell(cell);
}

DetectorWidget* DetectorWindow::detectorWidget()
{
    return _detector_widget;
}
