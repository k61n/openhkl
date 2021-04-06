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

#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/peak/Peak3D.h"
#include "core/raw/IDataReader.h"
#include "core/shape/IPeakIntegrator.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/ShapeCollection.h"
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
    , _peak_collection("temp", nsx::listtype::FOUND)
    , _peak_collection_item()
    , _peak_collection_model()
    , _size_policy_right(QSizePolicy::Expanding, QSizePolicy::Expanding)
{
    auto main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);

    _left_layout = new QVBoxLayout();

    setInputUp();
    setParametersUp();
    setIntegrateUp();
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

void SubframePredictPeaks::setInputUp()
{
    auto input_box = new Spoiler("1. Shape collection");
    GridFiller f(input_box, true);

    _exp_combo = f.addCombo("Experiment");
    _peak_combo = f.addCombo("Peak collection");
    auto build_shape_lib = f.addButton(
        "Build shape collection",
        "<font>A shape collection is a collection of averaged peaks attached to a peak"
        "collection. A shape is the averaged peak shape of a peak and its neighbours within a "
        "specified cutoff.</font>"); // Rich text to force line break in tooltip

    connect(
        _exp_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframePredictPeaks::updatePeakList);

    connect(
        _peak_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframePredictPeaks::refreshPeakShapeStatus);

    connect(build_shape_lib, &QPushButton::clicked, this, &SubframePredictPeaks::openShapeBuilder);

    _left_layout->addWidget(input_box);
}

void SubframePredictPeaks::setParametersUp()
{
    _para_box = new Spoiler("2. Predict peaks");
    GridFiller f(_para_box, true);

    _unit_cells = f.addCombo("Unit cell:");
    _interpolation = f.addCombo("Interpolation:");
    _d_min = f.addDoubleSpinBox("d min:", QString::fromUtf8("(\u212B) - minimum d (Bragg's law)"));
    _d_max = f.addDoubleSpinBox("d max:", QString::fromUtf8("(\u212B) - maximum d (Bragg's law)"));
    _min_neighbors = f.addSpinBox(
        "Min. neighbors:", "Minimum number of neighbours to average over for one peak");
    _radius = f.addDoubleSpinBox("Search radius:", "(pixels) - neighbour search radius in pixels");
    _n_frames = f.addDoubleSpinBox("Frames:", "(frames) - neighbour search radius in frames");
    auto run_prediction = f.addButton("Predict");

    _interpolation->addItem(QString("No interpolation"));
    _interpolation->addItem(QString("Inverse distance"));
    _interpolation->addItem(QString("Intensity"));

    _d_min->setMaximum(100000);
    _d_min->setDecimals(2);

    _d_max->setMaximum(100000);
    _d_max->setDecimals(2);

    _radius->setMaximum(100000);
    _radius->setDecimals(2);

    _n_frames->setMaximum(100000);
    _n_frames->setDecimals(2);

    _min_neighbors->setMaximum(100000);

    connect(run_prediction, &QPushButton::clicked, this, &SubframePredictPeaks::runPrediction);

    _left_layout->addWidget(_para_box);
}

void SubframePredictPeaks::setIntegrateUp()
{
    _integrate_box = new Spoiler("3. Integrate peaks");
    GridFiller f(_integrate_box, true);

    _integrator = f.addCombo();
    _fit_center =
        f.addCheckBox("Fit the center", "Allow the peak center to move during integration");
    _fit_covariance = f.addCheckBox(
        "Fit the covariance", "Allow the peak covariance matrix to vary during integration");
    _peak_end_int = f.addDoubleSpinBox("Peak end:", "(sigmas) - scaling factor for peak region");
    _bkg_start_int = f.addDoubleSpinBox(
        "Bkg. begin:", "(sigmas) - scaling factor for lower limit of background");
    _bkg_end_int =
        f.addDoubleSpinBox("Bkg. end:", "(sigmas) - scaling factor for upper limit of background");
    _radius_int =
        f.addDoubleSpinBox("Search radius:", "(pixels) - neighbour search radius in pixels");
    _n_frames_int = f.addDoubleSpinBox("Frames:", "(frames) - neighbour search radius in frames");
    auto run_integration = f.addButton("Integrate");

    _integrator->setMaximumWidth(1000);
    _integrator->addItem("Pixel sum integrator");
    _integrator->addItem("Gaussian integrator");
    _integrator->addItem("I/Sigma integrator");
    _integrator->addItem("1d profile integrator");
    _integrator->addItem("3d profile integrator");

    _peak_end_int->setMaximum(100000);
    _peak_end_int->setDecimals(2);

    _bkg_start_int->setMaximum(100000);
    _bkg_start_int->setDecimals(2);

    _bkg_end_int->setMaximum(100000);
    _bkg_end_int->setDecimals(2);

    _radius_int->setMaximum(100000);
    _radius_int->setDecimals(2);

    _n_frames_int->setMaximum(100000);
    _n_frames_int->setDecimals(2);

    connect(run_integration, &QPushButton::clicked, this, &SubframePredictPeaks::runIntegration);

    _left_layout->addWidget(_integrate_box);
}

void SubframePredictPeaks::setPreviewUp()
{
    _preview_box = new Spoiler("4. Show/hide peaks");
    _peak_view_widget = new PeakViewWidget("Valid peaks", "Invalid peaks");

    connect(
        _peak_view_widget, &PeakViewWidget::settingsChanged, this,
        &SubframePredictPeaks::refreshPeakVisual);

    _preview_box->setContentLayout(*_peak_view_widget);

    _left_layout->addWidget(_preview_box);
}

void SubframePredictPeaks::setSaveUp()
{
    auto save_button = new QPushButton("Create peak collection");
    _left_layout->addWidget(save_button);
    connect(save_button, &QPushButton::clicked, this, &SubframePredictPeaks::accept);
}

void SubframePredictPeaks::setFigureUp()
{
    QGroupBox* figure_group = new QGroupBox("Preview");
    QGridLayout* figure_grid = new QGridLayout(figure_group);

    figure_group->setSizePolicy(_size_policy_right);

    _figure_view = new DetectorView(this);
    _figure_view->getScene()->linkPeakModel(&_peak_collection_model);
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

    peak_grid->addWidget(_peak_table, 0, 0, 0, 0);
    _right_element->addWidget(peak_group);
}

void SubframePredictPeaks::refreshAll()
{
    setExperiments();
}

void SubframePredictPeaks::setExperiments()
{
    _exp_combo->blockSignals(true);
    _exp_combo->clear();

    if (gSession->experimentNames().empty())
        return;

    for (const QString& exp : gSession->experimentNames())
        _exp_combo->addItem(exp);

    _exp_combo->blockSignals(false);

    updatePeakList();
    updateUnitCellList();
    updateDatasetList();
    grabPredictorParameters();
}

void SubframePredictPeaks::updatePeakList()
{
    _peak_combo->blockSignals(true);
    _peak_combo->clear();
    _peak_list.clear();

    QStringList tmp = gSession->experimentAt(_exp_combo->currentIndex())
                          ->getPeakCollectionNames(nsx::listtype::FOUND);
    _peak_list.append(tmp);
    tmp.clear();
    tmp = gSession->experimentAt(_exp_combo->currentIndex())
              ->getPeakCollectionNames(nsx::listtype::FILTERED);
    _peak_list.append(tmp);

    if (!_peak_list.empty()) {
        _peak_combo->addItems(_peak_list);
        _peak_combo->setCurrentIndex(0);
    }
    refreshPeakShapeStatus();
    _peak_combo->blockSignals(false);
}

void SubframePredictPeaks::updateUnitCellList()
{
    _unit_cells->blockSignals(true);
    _unit_cells->clear();

    _unit_cell_list = gSession->experimentAt(_exp_combo->currentIndex())->getUnitCellNames();

    if (!_unit_cell_list.empty()) {
        _unit_cells->addItems(_unit_cell_list);
        _unit_cells->setCurrentIndex(0);
    }
    _unit_cells->blockSignals(false);
}

void SubframePredictPeaks::updateDatasetList()
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

void SubframePredictPeaks::updateDatasetParameters(int idx)
{
    if (_data_list.empty() || idx < 0)
        return;

    const int nFrames = _data_list.at(idx)->nFrames();

    _figure_view->getScene()->slotChangeSelectedData(_data_list.at(idx), 0);
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
    _params = gSession->experimentAt(_exp_combo->currentIndex())->experiment()->predict_params;
    _shape_params = gSession->experimentAt(_exp_combo->currentIndex())->experiment()->shape_params;

    // Prediction parameters
    _d_min->setValue(_params.detector_range_min);
    _d_max->setValue(_params.detector_range_max);
    _radius->setValue(_params.neighbour_range_pixels);
    _n_frames->setValue(_params.neighbour_range_frames);
    _min_neighbors->setValue(_params.min_n_neighbors);

    // Integration parameters
    _peak_end_int->setValue(_params.peak_end);
    _bkg_start_int->setValue(_params.bkg_begin);
    _bkg_end_int->setValue(_params.bkg_end);
    _radius_int->setValue(_params.neighbour_range_pixels);
    _n_frames_int->setValue(_params.neighbour_range_frames);
    _fit_center->setChecked(_params.fit_center);
    _fit_covariance->setChecked(_params.fit_cov);
}

void SubframePredictPeaks::setPredictorParameters() const { }

void SubframePredictPeaks::runPrediction()
{
    try {
        const std::vector<nsx::sptrDataSet>& data = gSession->currentProject()->allData();

        nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
        ProgressView progressView(nullptr);
        progressView.watch(handler);

        nsx::ShapeCollection* lib =
            gSession->experimentAt(_exp_combo->currentIndex())
                ->experiment()
                ->getPeakCollection(_peak_combo->currentText().toStdString())
                ->shapeCollection();
        nsx::UnitCell* cell = gSession->currentProject()->experiment()->getUnitCell(
            _unit_cells->currentText().toStdString());

        nsx::PredictionParameters params;
        params.detector_range_min = _d_min->value();
        params.detector_range_max = _d_max->value();
        params.neighbour_range_pixels = _radius->value();
        params.neighbour_range_frames = _n_frames->value();
        params.min_n_neighbors = _min_neighbors->value();
        params.fit_center = _fit_center->isChecked();
        params.fit_cov = _fit_covariance->isChecked();
        int interpol = _interpolation->currentIndex();

        nsx::PeakInterpolation peak_interpolation = static_cast<nsx::PeakInterpolation>(interpol);
        std::vector<nsx::Peak3D*> predicted_peaks;

        for (const nsx::sptrDataSet& d : data) {
            std::vector<nsx::Peak3D*> predicted =
                nsx::predictPeaks(lib, d, cell, peak_interpolation, params, handler);

            for (nsx::Peak3D* peak : predicted)
                predicted_peaks.push_back(peak);
        }

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

void SubframePredictPeaks::runIntegration()
{
    try {
        nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
        ProgressView progressView(nullptr);
        progressView.watch(handler);

        nsx::IPeakIntegrator* integrator =
            gSession->experimentAt(_exp_combo->currentIndex())
                ->experiment()
                ->getIntegrator(_integrator->currentText().toStdString());

        nsx::ShapeCollection* lib =
            gSession->experimentAt(_exp_combo->currentIndex())
                ->experiment()
                ->getPeakCollection(_peak_combo->currentText().toStdString())
                ->shapeCollection();

        if (!integrator)
            return;

        nsx::PredictionParameters params;
        params.bkg_begin = _bkg_start_int->value();
        params.bkg_end = _bkg_end_int->value();
        params.neighbour_range_pixels = _radius_int->value();
        params.neighbour_range_frames = _n_frames_int->value();
        params.fit_center = _fit_center->isChecked();
        params.fit_cov = _fit_covariance->isChecked();

        integrator->setHandler(handler);

        gSession->experimentAt(_exp_combo->currentIndex())
            ->experiment()
            ->integratePredictedPeaks(
                _integrator->currentText().toStdString(), &_peak_collection, lib, params);

        refreshPeakTable();
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString(e.what()));
    }
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

void SubframePredictPeaks::refreshPeakShapeStatus()
{
    bool shape_collection_present = true;

    if (_peak_list.empty() || _exp_combo->count() < 1)
        shape_collection_present = false;

    if (shape_collection_present) {
        nsx::PeakCollection* collection =
            gSession->experimentAt(_exp_combo->currentIndex())
                ->experiment()
                ->getPeakCollection(_peak_combo->currentText().toStdString());
        if (collection->shapeCollection() == nullptr)
            shape_collection_present = false;
    }

    _para_box->setEnabled(shape_collection_present);
    _integrate_box->setEnabled(shape_collection_present);
    _preview_box->setEnabled(shape_collection_present);
}

void SubframePredictPeaks::refreshPeakTable()
{
    if (_peak_list.empty() || _exp_combo->count() < 1)
        return;

    _figure_view->getScene()->clearPeakItems();
    _peak_collection_model.setRoot(&_peak_collection_item);
    refreshPeakVisual();
}

void SubframePredictPeaks::refreshPeakVisual()
{
    if (_peak_collection_item.childCount() == 0)
        return;

    bool valid;
    PeakItemGraphic* graphic;

    for (int i = 0; i < _peak_collection_item.childCount(); ++i) {
        PeakItem* peak = _peak_collection_item.peakItemAt(i);
        graphic = peak->peakGraphic();
        valid = peak->peak()->enabled();

        if (valid) {
            graphic->showArea(_peak_view_widget->drawPeaks1->isChecked());
            graphic->showLabel(false);
            graphic->setSize(_peak_view_widget->sizePeaks1->value());
            graphic->setColor(Qt::transparent);
            graphic->setCenterColor(_peak_view_widget->colorPeaks1->color());
            graphic->showBox(_peak_view_widget->drawBoxes1->isChecked());
            graphic->setBoxColor(_peak_view_widget->colorBoxes1->color());
            graphic->showBkg(_peak_view_widget->drawBkg1->isChecked());
            graphic->setBkgColor(_peak_view_widget->colorBkg1->color());
        } else {
            graphic->showArea(_peak_view_widget->drawPeaks2->isChecked());
            graphic->showLabel(false);
            graphic->setSize(_peak_view_widget->sizePeaks2->value());
            graphic->setColor(Qt::transparent);
            graphic->setCenterColor(_peak_view_widget->colorPeaks2->color());
            graphic->showBox(_peak_view_widget->drawBoxes2->isChecked());
            graphic->setBoxColor(_peak_view_widget->colorBoxes2->color());
            graphic->showBkg(_peak_view_widget->drawBkg2->isChecked());
            graphic->setBkgColor(_peak_view_widget->colorBkg2->color());
        }
    }
    _figure_view->getScene()->update();
    _figure_view->getScene()->drawPeakitems();
}

void SubframePredictPeaks::changeSelected(PeakItemGraphic* peak_graphic)
{
    int row = _peak_collection_item.returnRowOfVisualItem(peak_graphic);
    QModelIndex index = _peak_collection_model.index(row, 0);
    _peak_table->selectRow(row);
    _peak_table->scrollTo(index, QAbstractItemView::PositionAtTop);
}

void SubframePredictPeaks::openShapeBuilder()
{
    nsx::PeakCollection* peak_collection =
        gSession->experimentAt(_exp_combo->currentIndex())
            ->experiment()
            ->getPeakCollection(_peak_combo->currentText().toStdString());

    std::unique_ptr<ShapeCollectionDialog> dialog(
        new ShapeCollectionDialog(peak_collection, _shape_params));

    dialog->exec();
    _d_min->setValue(dialog->getDMin());
    _d_max->setValue(dialog->getDMax());
    refreshPeakShapeStatus();
}
