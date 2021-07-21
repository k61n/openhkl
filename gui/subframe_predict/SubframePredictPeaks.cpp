//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_predict/SubframePredictPeaks.cpp
//! @brief     Implements classes FoundPeaks, SubframePredictPeaks
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_predict/SubframePredictPeaks.h"

#include "base/utils/Logger.h"
#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/integration/ShapeIntegrator.h"
#include "core/peak/Peak3D.h"
#include "core/shape/IPeakIntegrator.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/Predictor.h"
#include "core/shape/ShapeCollection.h"
#include "gui/MainWin.h" // gGui
#include "gui/dialogs/ListNameDialog.h"
#include "gui/frames/ProgressView.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/graphics/DetectorView.h"
#include "gui/items/PeakItem.h"
#include "gui/models/Meta.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/subframe_predict/ShapeCollectionDialog.h"
#include "gui/utility/ColorButton.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/PropertyScrollArea.h"
#include "gui/utility/SideBar.h"
#include "gui/utility/Spoiler.h"
#include "gui/views/PeakTableView.h"
#include "gui/widgets/PeakViewWidget.h"
#include "tables/crystal/UnitCell.h"

#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QItemDelegate>
#include <QMessageBox>
#include <QScrollArea>
#include <QScrollBar>
#include <QSpacerItem>
#include <QTableWidgetItem>

SubframePredictPeaks::SubframePredictPeaks()
    : QWidget()
    , _peak_collection("temp", nsx::listtype::PREDICTED)
    , _peak_collection_item()
    , _peak_collection_model()
    , _shape_params()
    , _size_policy_right(QSizePolicy::Expanding, QSizePolicy::Expanding)
{
    auto main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);

    _left_layout = new QVBoxLayout();

    setParametersUp();
    setShapeCollectionUp();
    setPreviewUp();
    setSaveUp();
    setFigureUp();
    setPeakTableUp();

    _right_element->setSizePolicy(_size_policy_right);

    auto propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_left_layout);
    main_layout->addWidget(propertyScrollArea);
    main_layout->addWidget(_right_element);
}

void SubframePredictPeaks::setParametersUp()
{
    _para_box = new Spoiler("Predict peaks");
    GridFiller f(_para_box, true);

    _exp_combo = f.addCombo("Experiment");
    _cell_combo = f.addCombo("Unit cell:");
    _d_min = f.addDoubleSpinBox("d min:", QString::fromUtf8("(\u212B) - minimum d (Bragg's law)"));
    _d_max = f.addDoubleSpinBox("d max:", QString::fromUtf8("(\u212B) - maximum d (Bragg's law)"));
    _predict_button = f.addButton("Predict");
    _predict_button->setEnabled(false);

    _d_min->setMaximum(100);
    _d_min->setDecimals(2);

    _d_max->setMaximum(100);
    _d_max->setDecimals(2);

    connect(
        _exp_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframePredictPeaks::updateUnitCellList);
    connect(_predict_button, &QPushButton::clicked, this, &SubframePredictPeaks::runPrediction);
    connect(
        gGui->sideBar(), &SideBar::subframeChanged, this,
        &SubframePredictPeaks::setPredictorParameters);
    connect(
        gGui->sideBar(), &SideBar::subframeChanged, this,
        &SubframePredictPeaks::setShapeCollectionParameters);
    connect(
        _exp_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframePredictPeaks::refreshPeakCombo);

    _left_layout->addWidget(_para_box);
}

void SubframePredictPeaks::setShapeCollectionUp()
{
    _shapes_box = new Spoiler("Generate shapes");
    GridFiller f(_shapes_box, true);

    _found_peaks_combo =
        f.addCombo("Found peak collection", "Found peaks from which to construct shape collection");
    _nx = f.addSpinBox("histogram bins x", "Number of bins in x direction");
    _ny = f.addSpinBox("histogram bins y", "Number of bins in x direction");
    _nz = f.addSpinBox("histogram bins f", "Number of bins in frames direction");
    _kabsch = f.addCheckBox(
        "Kabsch coordinates", "Use Kabsch coordinate to mitigate effects of detector geometry", 1);
    _sigma_m = f.addDoubleSpinBox(
        QString("Mosaicity ") + QChar(0x03C3), "Variance arising from crystal mosaicity");
    _sigma_d = f.addDoubleSpinBox(
        QString("Beam Divergence ") + QString(QChar(0x03C3)),
        "Variance arising from beam divergence");
    _min_strength = f.addDoubleSpinBox("Minimum I/" + QString(QChar(0x03C3)),
            "Minimum strength for peak to be included in shape collection");
    _min_d =
        f.addDoubleSpinBox("Minimum d", "Minimum d for peak to be included in shape collection");
    _max_d =
        f.addDoubleSpinBox("Maximum d", "Minimum d for peak to be included in shape collection");
    _peak_end = f.addDoubleSpinBox("Peak end", "(sigmas) - scaling factor for peak region");
    _bkg_begin =
        f.addDoubleSpinBox("Bkg begin:", "(sigmas) - scaling factor for lower limit of background");
    _bkg_end =
        f.addDoubleSpinBox("Bkg end:", "(sigmas) - scaling factor for upper limit of background");
    _radius_pix =
        f.addDoubleSpinBox("Search radius:", "(pixels) - neighbour search radius in pixels");
    _radius_frames =
        f.addDoubleSpinBox("N. of frames:", "(frames) - neighbour search radius in frames");
    _min_neighbours = f.addSpinBox(
        "Min. neighbours", "Minimum number of neighbouring shapes to predict peak shape");
    _interpolation_combo = f.addCombo("Interpolation", "Interpolation type for peak shape");
    _assign_peak_shapes = f.addButton(
        "Assign peak shapes", "Assign peak shapes from shape collection to a predicted collection");

    _nx->setMaximum(100);
    _ny->setMaximum(100);
    _nz->setMaximum(100);

    _sigma_m->setMaximum(10);
    _sigma_m->setDecimals(2);

    _sigma_d->setMaximum(10);
    _sigma_d->setDecimals(2);

    _min_d->setMaximum(100);
    _min_d->setDecimals(2);

    _max_d->setMaximum(100);
    _max_d->setDecimals(2);

    _peak_end->setMaximum(100);
    _peak_end->setDecimals(2);

    _bkg_begin->setMaximum(100);
    _bkg_begin->setDecimals(2);

    _bkg_end->setMaximum(100);
    _bkg_end->setDecimals(2);

    _radius_pix->setMaximum(1000);
    _radius_pix->setDecimals(2);

    _radius_frames->setMaximum(50);
    _radius_frames->setDecimals(2);

    _interpolation_combo->addItem("None");
    _interpolation_combo->addItem("Inverse distance");
    _interpolation_combo->addItem("Intensity");

    connect(
        _assign_peak_shapes, &QPushButton::clicked, this, &SubframePredictPeaks::assignPeakShapes);

    _left_layout->addWidget(_shapes_box);
    grabShapeCollectionParameters();
}

void SubframePredictPeaks::setPreviewUp()
{
    _preview_box = new Spoiler("Show/hide peaks");
    _peak_view_widget = new PeakViewWidget("Valid peaks", "Invalid peaks");

    connect(
        _peak_view_widget, &PeakViewWidget::settingsChanged, this,
        &SubframePredictPeaks::refreshPeakVisual);

    _preview_box->setContentLayout(*_peak_view_widget);
    _left_layout->addWidget(_preview_box);
}

void SubframePredictPeaks::setSaveUp()
{
    _save_button = new QPushButton("Create peak collection");
    _save_button->setEnabled(false);
    _left_layout->addWidget(_save_button);
    connect(_save_button, &QPushButton::clicked, this, &SubframePredictPeaks::accept);
}

void SubframePredictPeaks::setFigureUp()
{
    QGroupBox* figure_group = new QGroupBox("Preview");
    QGridLayout* figure_grid = new QGridLayout(figure_group);

    figure_group->setSizePolicy(_size_policy_right);

    _figure_view = new DetectorView(this);
    _figure_view->getScene()->linkPeakModel1(&_peak_collection_model);
    _figure_view->scale(1, -1);
    figure_grid->addWidget(_figure_view, 0, 0, 1, 3);

    _data_combo = new QComboBox(this);
    _data_combo->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    figure_grid->addWidget(_data_combo, 1, 0, 1, 1);

    _figure_scroll = new QScrollBar(this);
    _figure_scroll->setOrientation(Qt::Horizontal);
    _figure_scroll->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    figure_grid->addWidget(_figure_scroll, 1, 1, 1, 1);

    _figure_spin = new QSpinBox(this);
    _figure_spin->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    figure_grid->addWidget(_figure_spin, 1, 2, 1, 1);

    connect(
        _figure_scroll, SIGNAL(valueChanged(int)), _figure_view->getScene(),
        SLOT(slotChangeSelectedFrame(int)));

    connect(_figure_scroll, SIGNAL(valueChanged(int)), _figure_spin, SLOT(setValue(int)));

    connect(_figure_spin, SIGNAL(valueChanged(int)), _figure_scroll, SLOT(setValue(int)));

    connect(
        _figure_view->getScene(), &DetectorScene::signalSelectedPeakItemChanged, this,
        &SubframePredictPeaks::changeSelected);

    _right_element->addWidget(figure_group);
}

void SubframePredictPeaks::setPeakTableUp()
{
    QGroupBox* peak_group = new QGroupBox("Peaks");
    QGridLayout* peak_grid = new QGridLayout(peak_group);

    peak_group->setSizePolicy(_size_policy_right);

    _peak_table = new PeakTableView(this);
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->setModel(&_peak_collection_model);
    _peak_table->resizeColumnsToContents();

    peak_grid->addWidget(_peak_table, 0, 0, 0, 0);
    _right_element->addWidget(peak_group);
}

void SubframePredictPeaks::refreshAll()
{
    setExperiments();
    toggleUnsafeWidgets();
}

void SubframePredictPeaks::setExperiments()
{
    _exp_combo->blockSignals(true);
    QString current_exp = _exp_combo->currentText();
    _exp_combo->clear();
    for (const QString& exp : gSession->experimentNames())
        _exp_combo->addItem(exp);
    _exp_combo->setCurrentText(current_exp);

    if (!(_exp_combo->count() == 0)) {
        updateUnitCellList();
        updateDatasetList();
        refreshPeakCombo();
        grabPredictorParameters();
        grabShapeCollectionParameters();
        computeSigmas();
    }

    _exp_combo->blockSignals(false);
}

void SubframePredictPeaks::updateUnitCellList()
{
    _cell_combo->blockSignals(true);
    QString current_cell = _cell_combo->currentText();
    _cell_combo->clear();

    _unit_cell_list = gSession->experimentAt(_exp_combo->currentIndex())->getUnitCellNames();

    if (!_unit_cell_list.empty()) {
        _cell_combo->addItems(_unit_cell_list);
        _cell_combo->setCurrentText(current_cell);
    }
    _cell_combo->blockSignals(false);
}

void SubframePredictPeaks::updateDatasetList()
{
    _data_combo->blockSignals(true);
    QString current_data = _data_combo->currentText();
    _data_combo->clear();

    _data_list = gSession->experimentAt(_exp_combo->currentIndex())->allData();

    if (!_data_list.empty()) {
        for (const nsx::sptrDataSet& data : _data_list) {
            _data_combo->addItem(QString::fromStdString(data->name()));
        }
        _data_combo->setCurrentText(current_data);
        updateDatasetParameters(_data_combo->currentIndex());
    }
    _data_combo->blockSignals(false);
}

void SubframePredictPeaks::updateDatasetParameters(int idx)
{
    if (_data_list.empty() || idx < 0)
        return;

    const int nFrames = _data_list.at(idx)->nFrames();

    _figure_view->getScene()->slotChangeSelectedData(_data_list.at(idx), _figure_spin->value());
    //_figure_view->getScene()->setMaxIntensity(3000);
    emit _figure_view->getScene()->dataChanged();
    _figure_view->getScene()->update();

    _figure_scroll->setMaximum(nFrames - 1);
    _figure_scroll->setMinimum(0);

    _figure_spin->setMaximum(nFrames - 1);
    _figure_spin->setMinimum(0);
}

void SubframePredictPeaks::grabPredictorParameters()
{
    auto params =
        gSession->experimentAt(_exp_combo->currentIndex())->experiment()->predictor()->parameters();

    _d_min->setValue(params->d_min);
    _d_max->setValue(params->d_max);
}

void SubframePredictPeaks::setPredictorParameters()
{
    if (_exp_combo->count() == 0 || _cell_combo->count() == 0)
        return;

    auto params =
        gSession->experimentAt(_exp_combo->currentIndex())->experiment()->predictor()->parameters();

    params->d_min = _d_min->value();
    params->d_max = _d_max->value();
}

void SubframePredictPeaks::computeSigmas()
{
    if (!(_peak_collection.numberOfPeaks() == 0)) {
        _peak_collection.computeSigmas();
        _sigma_m->setValue(_peak_collection.sigmaM());
        _sigma_d->setValue(_peak_collection.sigmaD());
    }
}

void SubframePredictPeaks::grabShapeCollectionParameters()
{
    _nx->setValue(_shape_params.nbins_x);
    _ny->setValue(_shape_params.nbins_y);
    _nz->setValue(_shape_params.nbins_z);
    _kabsch->setChecked(_shape_params.kabsch_coords);
    _min_strength->setValue(_shape_params.strength_min);
    _min_d->setValue(_shape_params.d_min);
    _max_d->setValue(_shape_params.d_max);
    _peak_end->setValue(_shape_params.peak_end);
    _bkg_begin->setValue(_shape_params.bkg_begin);
    _bkg_end->setValue(_shape_params.bkg_end);
    _radius_pix->setValue(_shape_params.neighbour_range_pixels);
    _radius_frames->setValue(_shape_params.neighbour_range_frames);
    _min_neighbours->setValue(_shape_params.min_neighbors);
    _interpolation_combo->setCurrentIndex(static_cast<int>(_shape_params.interpolation));
}

void SubframePredictPeaks::setShapeCollectionParameters()
{
    if (!(_peak_collection.numberOfPeaks() == 0)) {
        _peak_collection.computeSigmas();
        _sigma_m->setValue(_peak_collection.sigmaM());
        _sigma_d->setValue(_peak_collection.sigmaD());
    }
    _shape_params.nbins_x = _nx->value();
    _shape_params.nbins_y = _ny->value();
    _shape_params.nbins_z = _nz->value();
    _shape_params.kabsch_coords = _kabsch->isChecked();
    _shape_params.strength_min = _min_strength->value();
    _shape_params.d_min = _min_d->value();
    _shape_params.d_max = _max_d->value();
    _shape_params.peak_end = _peak_end->value();
    _shape_params.bkg_begin = _bkg_begin->value();
    _shape_params.bkg_end = _bkg_end->value();
    _shape_params.neighbour_range_pixels = _radius_pix->value();
    _shape_params.neighbour_range_frames = _radius_frames->value();
    _shape_params.min_neighbors = _min_neighbours->value();
    _shape_params.interpolation =
        static_cast<nsx::PeakInterpolation>(_interpolation_combo->currentIndex());
}

void SubframePredictPeaks::refreshPeakCombo()
{
    _found_peaks_combo->blockSignals(true);
    QString current_peaks = _found_peaks_combo->currentText();
    _found_peaks_combo->clear();
    _found_peaks_combo->addItems(
        gSession->experimentAt(_exp_combo->currentIndex())->
        getPeakCollectionNames(nsx::listtype::FOUND));
    _found_peaks_combo->setCurrentText(current_peaks);
    _found_peaks_combo->blockSignals(false);
}

void SubframePredictPeaks::runPrediction()
{
    try {
        auto* experiment = gSession->experimentAt(_exp_combo->currentIndex())->experiment();
        auto data = experiment->getData(_data_combo->currentText().toStdString());
        auto* predictor = experiment->predictor();
        setPredictorParameters();

        nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
        ProgressView progressView(nullptr);
        progressView.watch(handler);

        nsx::UnitCell* cell = gSession->currentProject()->experiment()->getUnitCell(
            _cell_combo->currentText().toStdString());

        predictor->predictPeaks(data, cell, handler);

        std::vector<nsx::Peak3D*> predicted_peaks;
        for (nsx::Peak3D* peak : predictor->peaks())
            predicted_peaks.push_back(peak);

        _peak_collection.populate(predicted_peaks);
        for (nsx::Peak3D* peak : predicted_peaks)
            delete peak;
        predicted_peaks.clear();

        _peak_collection_item.setPeakCollection(&_peak_collection);
        _peak_collection_model.setRoot(&_peak_collection_item);
        refreshPeakTable();
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString(e.what()));
    }
}

void SubframePredictPeaks::assignPeakShapes()
{
    auto* experiment = gSession->experimentAt(_exp_combo->currentIndex())->experiment();
    auto data = experiment->getData(_data_combo->currentText().toStdString());
    auto* found_peaks = experiment->getPeakCollection(_found_peaks_combo->currentText().toStdString());

    setShapeCollectionParameters();

    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
    ProgressView progressView(nullptr);
    progressView.watch(handler);
    experiment->integrator()->setHandler(handler);

    experiment->buildShapeCollection(found_peaks, data, _shape_params);
    _shape_collection = found_peaks->shapeCollection();
    _shape_collection->setPredictedShapes(&_peak_collection, _shape_params.interpolation, handler);
    refreshPeakTable();
}

void SubframePredictPeaks::accept()
{
    std::unique_ptr<ListNameDialog> dlg(new ListNameDialog());
    dlg->exec();
    if (!dlg->listName().isEmpty()) {
        gSession->experimentAt(_exp_combo->currentIndex())
            ->experiment()
            ->addPeakCollection(
                dlg->listName().toStdString(), nsx::listtype::PREDICTED,
                _peak_collection.getPeakList());
        gSession->experimentAt(_exp_combo->currentIndex())->generatePeakModel(dlg->listName());
    }
}

void SubframePredictPeaks::refreshPeakTable()
{
    if (_exp_combo->count() == 0)
        return;

    computeSigmas();
    _figure_view->getScene()->clearPeakItems();
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->resizeColumnsToContents();
    refreshPeakVisual();
}

void SubframePredictPeaks::refreshPeakVisual()
{
    if (_peak_collection_item.childCount() == 0)
        return;

    for (int i = 0; i < _peak_collection_item.childCount(); ++i) {
        PeakItem* peak = _peak_collection_item.peakItemAt(i);
        auto graphic = peak->peakGraphic();

        graphic->showLabel(false);
        graphic->setColor(Qt::transparent);
        graphic->initFromPeakViewWidget(
            peak->peak()->enabled() ? _peak_view_widget->set1 : _peak_view_widget->set2);
    }
    _figure_view->getScene()->update();
    _figure_view->getScene()->initIntRegionFromPeakWidget(_peak_view_widget->set1);
    _figure_view->getScene()->drawPeakitems();
}

void SubframePredictPeaks::changeSelected(PeakItemGraphic* peak_graphic)
{
    int row = _peak_collection_item.returnRowOfVisualItem(peak_graphic);
    QModelIndex index = _peak_collection_model.index(row, 0);
    _peak_table->selectRow(row);
    _peak_table->scrollTo(index, QAbstractItemView::PositionAtTop);
}

void SubframePredictPeaks::toggleUnsafeWidgets()
{
    _predict_button->setEnabled(true);
    _save_button->setEnabled(true);
    if (_cell_combo->count() == 0 || _exp_combo->count() == 0) {
        _predict_button->setEnabled(false);
        _save_button->setEnabled(false);
    }

    _sigma_d->setEnabled(true);
    _sigma_m->setEnabled(true);
    if (!_kabsch->isChecked()) {
        _sigma_d->setEnabled(false);
        _sigma_m->setEnabled(false);
    }

    _assign_peak_shapes->setEnabled(true);
    if (!(_peak_collection.numberOfPeaks() == 0))
        _assign_peak_shapes->setEnabled(false);
}
