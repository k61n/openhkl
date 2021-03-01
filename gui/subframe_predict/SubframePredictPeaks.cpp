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
    , _pixmap(nullptr)
{
    setSizePolicies();
    _main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);

    QScrollArea* scroll_area = new QScrollArea(this);
    QWidget* scroll_widget = new QWidget();
    scroll_area->setSizePolicy(*_size_policy_box);
    scroll_widget->setSizePolicy(*_size_policy_box);
    _left_layout = new QVBoxLayout(scroll_widget);
    scroll_area->setWidgetResizable(true);
    scroll_area->setWidget(scroll_widget);

    setInputUp();
    setParametersUp();
    setIntegrateUp();
    setPreviewUp();
    setSaveUp();
    setFigureUp();
    setPeakTableUp();

    _right_element->setSizePolicy(*_size_policy_right);

    _main_layout->addWidget(scroll_area);
    _main_layout->addWidget(_right_element);
}

void SubframePredictPeaks::setSizePolicies()
{
    _size_policy_widgets = new QSizePolicy();
    _size_policy_widgets->setHorizontalPolicy(QSizePolicy::Preferred);
    _size_policy_widgets->setVerticalPolicy(QSizePolicy::Fixed);

    _size_policy_box = new QSizePolicy();
    _size_policy_box->setHorizontalPolicy(QSizePolicy::Preferred);
    _size_policy_box->setVerticalPolicy(QSizePolicy::Preferred);

    _size_policy_right = new QSizePolicy();
    _size_policy_right->setHorizontalPolicy(QSizePolicy::Expanding);
    _size_policy_right->setVerticalPolicy(QSizePolicy::Expanding);

    _size_policy_fixed = new QSizePolicy();
    _size_policy_fixed->setHorizontalPolicy(QSizePolicy::Fixed);
    _size_policy_fixed->setVerticalPolicy(QSizePolicy::Fixed);
}

void SubframePredictPeaks::setInputUp()
{
    _input_box = new Spoiler("1. Shape collection");

    QGridLayout* _input_grid = new QGridLayout();

    QLabel* exp_label = new QLabel("Experiment");
    exp_label->setAlignment(Qt::AlignRight);
    _input_grid->addWidget(exp_label, 0, 0, 1, 1);

    QLabel* list_label = new QLabel("Peak collection");
    list_label->setAlignment(Qt::AlignRight);
    _input_grid->addWidget(list_label, 1, 0, 1, 1);

    _exp_combo = new QComboBox();
    _peak_combo = new QComboBox();
    _build_shape_lib = new QPushButton("Build shape collection");

    _exp_combo->setMaximumWidth(1000);
    _peak_combo->setMaximumWidth(1000);
    _build_shape_lib->setMaximumWidth(1000);

    _exp_combo->setSizePolicy(*_size_policy_widgets);
    _peak_combo->setSizePolicy(*_size_policy_widgets);
    _build_shape_lib->setSizePolicy(*_size_policy_widgets);

    _input_grid->addWidget(_exp_combo, 0, 1, 1, 1);
    _input_grid->addWidget(_peak_combo, 1, 1, 1, 1);
    _input_grid->addWidget(_build_shape_lib, 2, 0, 1, 2);

    connect(
        _exp_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframePredictPeaks::updatePeakList);

    connect(
        _peak_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframePredictPeaks::refreshPeakShapeStatus);

    connect(_build_shape_lib, &QPushButton::clicked, this, &SubframePredictPeaks::openShapeBuilder);

    _input_box->setContentLayout(*_input_grid, true);
    _input_box->setSizePolicy(*_size_policy_box);
    _input_box->contentArea.setSizePolicy(*_size_policy_box);

    _left_layout->addWidget(_input_box);
}

void SubframePredictPeaks::setParametersUp()
{
    _para_box = new Spoiler("2. Predict peaks");

    QGridLayout* para_grid = new QGridLayout();

    QLabel* label_ptr;

    label_ptr = new QLabel("Unit cell:");
    label_ptr->setAlignment(Qt::AlignRight);
    para_grid->addWidget(label_ptr, 0, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Interpolation:");
    label_ptr->setAlignment(Qt::AlignRight);
    para_grid->addWidget(label_ptr, 1, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("d min (A):");
    label_ptr->setAlignment(Qt::AlignRight);
    para_grid->addWidget(label_ptr, 2, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("d max (A):");
    label_ptr->setAlignment(Qt::AlignRight);
    para_grid->addWidget(label_ptr, 3, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Min. neighbors:");
    label_ptr->setAlignment(Qt::AlignRight);
    para_grid->addWidget(label_ptr, 4, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Search radius:");
    label_ptr->setAlignment(Qt::AlignRight);
    para_grid->addWidget(label_ptr, 5, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Frames:");
    label_ptr->setAlignment(Qt::AlignRight);
    para_grid->addWidget(label_ptr, 6, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    _unit_cells = new QComboBox();
    _interpolation = new QComboBox();
    _d_min = new QDoubleSpinBox();
    _d_max = new QDoubleSpinBox();
    _radius = new QDoubleSpinBox();
    _n_frames = new QDoubleSpinBox();
    _min_neighbors = new QSpinBox();
    _run_prediction = new QPushButton("Predict");

    _unit_cells->setMaximumWidth(1000);
    _interpolation->setMaximumWidth(1000);
    _interpolation->addItem(QString("No interpolation"));
    _interpolation->addItem(QString("Inverse distance"));
    _interpolation->addItem(QString("Intensity"));

    _d_min->setMaximumWidth(1000);
    _d_min->setMaximum(100000);
    _d_min->setDecimals(2);
    _d_min->setValue(1.5);

    _d_max->setMaximumWidth(1000);
    _d_max->setMaximum(100000);
    _d_max->setDecimals(2);
    _d_max->setValue(50);

    _radius->setMaximumWidth(1000);
    _radius->setMaximum(100000);
    _radius->setDecimals(2);
    _radius->setValue(400.);

    _n_frames->setMaximumWidth(1000);
    _n_frames->setMaximum(100000);
    _n_frames->setDecimals(2);
    _n_frames->setValue(10.0);

    _min_neighbors->setMaximumWidth(1000);
    _min_neighbors->setMaximum(100000);
    _min_neighbors->setValue(20);

    _run_prediction->setMaximumWidth(1000);

    _unit_cells->setSizePolicy(*_size_policy_widgets);
    _interpolation->setSizePolicy(*_size_policy_widgets);
    _d_min->setSizePolicy(*_size_policy_widgets);
    _d_max->setSizePolicy(*_size_policy_widgets);
    _radius->setSizePolicy(*_size_policy_widgets);
    _n_frames->setSizePolicy(*_size_policy_widgets);
    _min_neighbors->setSizePolicy(*_size_policy_widgets);
    _run_prediction->setSizePolicy(*_size_policy_widgets);

    para_grid->addWidget(_unit_cells, 0, 1, 1, 1);
    para_grid->addWidget(_interpolation, 1, 1, 1, 1);
    para_grid->addWidget(_d_min, 2, 1, 1, 1);
    para_grid->addWidget(_d_max, 3, 1, 1, 1);
    para_grid->addWidget(_min_neighbors, 4, 1, 1, 1);
    para_grid->addWidget(_radius, 5, 1, 1, 1);
    para_grid->addWidget(_n_frames, 6, 1, 1, 1);
    para_grid->addWidget(_run_prediction, 7, 0, 1, 2);

    _para_box->setContentLayout(*para_grid, true);
    _para_box->setSizePolicy(*_size_policy_box);
    _para_box->contentArea.setSizePolicy(*_size_policy_box);

    connect(_run_prediction, &QPushButton::clicked, this, &SubframePredictPeaks::runPrediction);

    _left_layout->addWidget(_para_box);
}


void SubframePredictPeaks::setIntegrateUp()
{
    _integrate_box = new Spoiler("3. Integrate peaks");

    QGridLayout* integrate_grid = new QGridLayout();

    QLabel* label_ptr;

    label_ptr = new QLabel("Peak end:");
    label_ptr->setAlignment(Qt::AlignRight);
    integrate_grid->addWidget(label_ptr, 3, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Bkg begin:");
    label_ptr->setAlignment(Qt::AlignRight);
    integrate_grid->addWidget(label_ptr, 4, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Bkg end:");
    label_ptr->setAlignment(Qt::AlignRight);
    integrate_grid->addWidget(label_ptr, 5, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Minimum d:");
    label_ptr->setAlignment(Qt::AlignRight);
    integrate_grid->addWidget(label_ptr, 6, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Maximum d:");
    label_ptr->setAlignment(Qt::AlignRight);
    integrate_grid->addWidget(label_ptr, 7, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Search radius:");
    label_ptr->setAlignment(Qt::AlignRight);
    integrate_grid->addWidget(label_ptr, 8, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("N. of frames:");
    label_ptr->setAlignment(Qt::AlignRight);
    integrate_grid->addWidget(label_ptr, 9, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    _integrator = new QComboBox();
    _fit_center = new QCheckBox("Fit the center");
    _fit_covariance = new QCheckBox("Fit the covariance");
    _peak_end_int = new QDoubleSpinBox();
    _bkg_start_int = new QDoubleSpinBox();
    _bkg_end_int = new QDoubleSpinBox();
    _d_min_int = new QDoubleSpinBox();
    _d_max_int = new QDoubleSpinBox();
    _radius_int = new QDoubleSpinBox();
    _n_frames_int = new QDoubleSpinBox();
    _run_integration = new QPushButton("Integrate");

    _integrator->setMaximumWidth(1000);
    _integrator->addItem("Pixel sum integrator");
    _integrator->addItem("Gaussian integrator");
    _integrator->addItem("I/Sigma integrator");
    _integrator->addItem("1d profile integrator");
    _integrator->addItem("3d profile integrator");

    _fit_center->setMaximumWidth(1000);
    _fit_center->setChecked(true);

    _fit_covariance->setMaximumWidth(1000);
    _fit_covariance->setChecked(true);

    _peak_end_int->setMaximumWidth(1000);
    _peak_end_int->setMaximum(100000);
    _peak_end_int->setDecimals(6);
    _peak_end_int->setValue(5);

    _bkg_start_int->setMaximumWidth(1000);
    _bkg_start_int->setMaximum(100000);
    _bkg_start_int->setDecimals(6);
    _bkg_start_int->setValue(3.0);

    _bkg_end_int->setMaximumWidth(1000);
    _bkg_end_int->setMaximum(100000);
    _bkg_end_int->setDecimals(6);
    _bkg_end_int->setValue(4.5);

    _d_min_int->setMaximumWidth(1000);
    _d_min_int->setMaximum(100000);
    _d_min_int->setDecimals(6);
    _d_min_int->setValue(1.5);

    _d_max_int->setMaximumWidth(1000);
    _d_max_int->setMaximum(100000);
    _d_max_int->setDecimals(6);
    _d_max_int->setValue(50);

    _radius_int->setMaximumWidth(1000);
    _radius_int->setMaximum(100000);
    _radius_int->setDecimals(6);
    _radius_int->setValue(400.);

    _n_frames_int->setMaximumWidth(1000);
    _n_frames_int->setMaximum(100000);
    _n_frames_int->setDecimals(6);
    _n_frames_int->setValue(10);

    _run_integration->setMaximumWidth(1000);

    _integrator->setSizePolicy(*_size_policy_widgets);
    _fit_center->setSizePolicy(*_size_policy_widgets);
    _fit_covariance->setSizePolicy(*_size_policy_widgets);
    _peak_end_int->setSizePolicy(*_size_policy_widgets);
    _bkg_start_int->setSizePolicy(*_size_policy_widgets);
    _bkg_end_int->setSizePolicy(*_size_policy_widgets);
    _d_min_int->setSizePolicy(*_size_policy_widgets);
    _d_max_int->setSizePolicy(*_size_policy_widgets);
    _radius_int->setSizePolicy(*_size_policy_widgets);
    _n_frames_int->setSizePolicy(*_size_policy_widgets);
    _run_integration->setSizePolicy(*_size_policy_widgets);

    integrate_grid->addWidget(_integrator, 0, 0, 1, 2);
    integrate_grid->addWidget(_fit_center, 1, 0, 1, 2);
    integrate_grid->addWidget(_fit_covariance, 2, 0, 1, 2);
    integrate_grid->addWidget(_peak_end_int, 3, 1, 1, 1);
    integrate_grid->addWidget(_bkg_start_int, 4, 1, 1, 1);
    integrate_grid->addWidget(_bkg_end_int, 5, 1, 1, 1);
    integrate_grid->addWidget(_d_min_int, 6, 1, 1, 1);
    integrate_grid->addWidget(_d_max_int, 7, 1, 1, 1);
    integrate_grid->addWidget(_radius_int, 8, 1, 1, 1);
    integrate_grid->addWidget(_n_frames_int, 9, 1, 1, 1);
    integrate_grid->addWidget(_run_integration, 10, 0, 1, 2);

    _integrate_box->setContentLayout(*integrate_grid, true);
    _integrate_box->setSizePolicy(*_size_policy_box);
    _integrate_box->contentArea.setSizePolicy(*_size_policy_box);

    connect(_run_integration, &QPushButton::clicked, this, &SubframePredictPeaks::runIntegration);

    _left_layout->addWidget(_integrate_box);
}
void SubframePredictPeaks::setPreviewUp()
{
    _preview_box = new Spoiler("4. Show/hide peaks");
    _peak_view_widget = new PeakViewWidget("Valid peaks", "Invalid peaks");

    connect(
        _peak_view_widget->drawPeaks1(), &QCheckBox::stateChanged, this,
        &SubframePredictPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->drawPeaks2(), &QCheckBox::stateChanged, this,
        &SubframePredictPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->drawBoxes1(), &QCheckBox::stateChanged, this,
        &SubframePredictPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->drawBoxes2(), &QCheckBox::stateChanged, this,
        &SubframePredictPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->drawBkg1(), &QCheckBox::stateChanged, this,
        &SubframePredictPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->drawBkg2(), &QCheckBox::stateChanged, this,
        &SubframePredictPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->peakSize1(),
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &SubframePredictPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->peakSize2(),
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &SubframePredictPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->peakColor1(), &ColorButton::colorChanged, this,
        &SubframePredictPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->peakColor2(), &ColorButton::colorChanged, this,
        &SubframePredictPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->boxColor1(), &ColorButton::colorChanged, this,
        &SubframePredictPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->boxColor2(), &ColorButton::colorChanged, this,
        &SubframePredictPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->bkgColor1(), &ColorButton::colorChanged, this,
        &SubframePredictPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->bkgColor2(), &ColorButton::colorChanged, this,
        &SubframePredictPeaks::refreshPeakVisual);

    _preview_box->setContentLayout(*_peak_view_widget);
    _preview_box->setSizePolicy(*_size_policy_box);
    _preview_box->contentArea.setSizePolicy(*_size_policy_box);

    _left_layout->addWidget(_preview_box);
}

void SubframePredictPeaks::setSaveUp()
{
    _save_button = new QPushButton("Create peak collection");
    _save_button->setMaximumWidth(1000);
    _save_button->setSizePolicy(*_size_policy_widgets);
    _left_layout->addWidget(_save_button);
    _left_layout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    connect(_save_button, &QPushButton::clicked, this, &SubframePredictPeaks::accept);
}

void SubframePredictPeaks::setFigureUp()
{
    QGroupBox* figure_group = new QGroupBox("Preview");
    QGridLayout* figure_grid = new QGridLayout(figure_group);

    figure_group->setSizePolicy(*_size_policy_right);

    _figure_view = new DetectorView(this);
    _figure_view->getScene()->linkPeakModel(&_peak_collection_model);
    _figure_view->scale(1, -1);
    figure_grid->addWidget(_figure_view, 0, 0, 1, 3);

    _data_combo = new QComboBox(this);
    _data_combo->setSizePolicy(*_size_policy_widgets);
    figure_grid->addWidget(_data_combo, 1, 0, 1, 1);

    _figure_scroll = new QScrollBar(this);
    _figure_scroll->setOrientation(Qt::Horizontal);
    _figure_scroll->setSizePolicy(*_size_policy_widgets);
    figure_grid->addWidget(_figure_scroll, 1, 1, 1, 1);

    _figure_spin = new QSpinBox(this);
    _figure_spin->setSizePolicy(*_size_policy_fixed);
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

    peak_group->setSizePolicy(*_size_policy_right);

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

    for (QString exp : gSession->experimentNames())
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

void SubframePredictPeaks::grabPredictorParameters() { }

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

        for (nsx::sptrDataSet d : data) {
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
        params.detector_range_min = _d_min_int->value();
        params.detector_range_max = _d_max_int->value();
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
            graphic->showArea(_peak_view_widget->drawPeaks1()->isChecked());
            graphic->showLabel(false);
            graphic->setSize(_peak_view_widget->peakSize1()->value());
            graphic->setColor(Qt::transparent);
            graphic->setCenterColor(_peak_view_widget->peakColor1()->getColor());
            graphic->showBox(_peak_view_widget->drawBoxes1()->isChecked());
            graphic->setBoxColor(_peak_view_widget->boxColor1()->getColor());
            graphic->showBkg(_peak_view_widget->drawBkg1()->isChecked());
            graphic->setBkgColor(_peak_view_widget->bkgColor1()->getColor());
        } else {
            graphic->showArea(_peak_view_widget->drawPeaks2()->isChecked());
            graphic->showLabel(false);
            graphic->setSize(_peak_view_widget->peakSize2()->value());
            graphic->setColor(Qt::transparent);
            graphic->setCenterColor(_peak_view_widget->peakColor2()->getColor());
            graphic->showBox(_peak_view_widget->drawBoxes2()->isChecked());
            graphic->setBoxColor(_peak_view_widget->boxColor2()->getColor());
            graphic->showBkg(_peak_view_widget->drawBkg2()->isChecked());
            graphic->setBkgColor(_peak_view_widget->bkgColor2()->getColor());
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

    std::unique_ptr<ShapeCollectionDialog> dialog(new ShapeCollectionDialog(peak_collection));

    dialog->exec();
    _d_min->setValue(dialog->getDMin());
    _d_max->setValue(dialog->getDMax());
    _d_min_int->setValue(dialog->getDMin());
    _d_max_int->setValue(dialog->getDMax());
    refreshPeakShapeStatus();
}
