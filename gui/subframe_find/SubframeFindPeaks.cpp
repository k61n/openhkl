//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_find/SubframeFindPeaks.cpp
//! @brief     Implements classes FoundPeaks, SubframeFindPeaks
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_find/SubframeFindPeaks.h"

#include "core/convolve/ConvolverFactory.h"
#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/PeakFinder.h"
#include "core/integration/PixelSumIntegrator.h"
#include "core/peak/Peak3D.h"
#include "core/raw/IDataReader.h"
#include "gui/dialogs/ListNameDialog.h"
#include "gui/frames/ProgressView.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/graphics/DetectorView.h"
#include "gui/items/PeakItem.h"
#include "gui/models/Meta.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/ColorButton.h"
#include "gui/utility/Spoiler.h"
#include "gui/views/PeakTableView.h"
#include "gui/widgets/PeakViewWidget.h"

#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QItemDelegate>
#include <QLabel>
#include <QMessageBox>
#include <QScrollArea>
#include <QScrollBar>
#include <QSpacerItem>
#include <QTableWidgetItem>

SubframeFindPeaks::SubframeFindPeaks()
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

    setDataUp();
    setBlobUp();
    setIntegrateUp();
    setPreviewUp();
    setSaveUp();
    setFigureUp();
    setPeakTableUp();

    _right_element->setSizePolicy(*_size_policy_right);

    _main_layout->addWidget(scroll_area);
    _main_layout->addWidget(_right_element);

    connect(_kernel_combo, &QComboBox::currentTextChanged, [=](QString) {
        updateConvolutionParameters();
        refreshPreview();
    });
}

void SubframeFindPeaks::setSizePolicies()
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

void SubframeFindPeaks::setDataUp()
{
    Spoiler* _data_box = new Spoiler("1. Input Data");

    QGridLayout* _data_grid = new QGridLayout();

    QLabel* exp_label = new QLabel("Experiment");
    exp_label->setAlignment(Qt::AlignRight);
    _data_grid->addWidget(exp_label, 0, 0, 1, 1);

    QLabel* data_label = new QLabel("Data-set");
    data_label->setAlignment(Qt::AlignRight);
    _data_grid->addWidget(data_label, 1, 0, 1, 1);

    _exp_combo = new QComboBox();
    _data_combo = new QComboBox();
    _all_data = new QCheckBox("Search all");

    _exp_combo->setMaximumWidth(1000);
    _data_combo->setMaximumWidth(1000);
    _all_data->setMaximumWidth(1000);

    _exp_combo->setSizePolicy(*_size_policy_widgets);
    _data_combo->setSizePolicy(*_size_policy_widgets);
    _all_data->setSizePolicy(*_size_policy_widgets);

    _data_grid->addWidget(_exp_combo, 0, 1, 1, 1);
    _data_grid->addWidget(_data_combo, 1, 1, 1, 1);
    _data_grid->addWidget(_all_data, 2, 1, 1, 2);

    connect(
        _exp_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        [=]() {
            grabFinderParameters();
            grabIntegrationParameters();
            updateDatasetList();
        });

    connect(
        _data_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeFindPeaks::updateDatasetParameters);

    _data_box->setContentLayout(*_data_grid);
    _data_box->setSizePolicy(*_size_policy_box);
    _data_box->contentArea.setSizePolicy(*_size_policy_box);

    _left_layout->addWidget(_data_box);
}

void SubframeFindPeaks::setBlobUp()
{
    Spoiler* blob_para = new Spoiler("2. Peak search parameters");

    QGridLayout* blob_grid = new QGridLayout();
    QString tooltip;

    QLabel* threshold_label = new QLabel("Threshold");
    threshold_label->setAlignment(Qt::AlignRight);
    blob_grid->addWidget(threshold_label, 0, 0, 1, 1);
    tooltip = "(counts) - pixels with fewer counts than the threshold are discarded";
    threshold_label->setToolTip(tooltip);

    QLabel* scale_label = new QLabel("Merging scale");
    scale_label->setAlignment(Qt::AlignRight);
    blob_grid->addWidget(scale_label, 1, 0, 1, 1);
    tooltip = "(sigmas) - blob scaling factor to detect collisions";
    scale_label->setToolTip(tooltip);

    QLabel* min_size_label = new QLabel("Minimum size");
    min_size_label->setAlignment(Qt::AlignRight);
    blob_grid->addWidget(min_size_label, 2, 0, 1, 1);
    tooltip = "(integer) - blobs containing fewer points than this count are discarded";
    min_size_label->setToolTip(tooltip);

    QLabel* max_size_label = new QLabel("Maximum size");
    max_size_label->setAlignment(Qt::AlignRight);
    blob_grid->addWidget(max_size_label, 3, 0, 1, 1);
    tooltip = "(integer) - blobs containing more points than this count are discarded";
    max_size_label->setToolTip(tooltip);

    QLabel* max_width_label = new QLabel("Maximum width");
    max_width_label->setAlignment(Qt::AlignRight);
    blob_grid->addWidget(max_width_label, 4, 0, 1, 1);
    tooltip = "(frames) - blob is discarded if it spans more frames than this value";
    max_width_label->setToolTip(tooltip);

    QLabel* kernel_label = new QLabel("Kernel");
    kernel_label->setAlignment(Qt::AlignRight);
    blob_grid->addWidget(kernel_label, 5, 0, 1, 1);
    tooltip = "Convolution kernel for peak search";
    kernel_label->setToolTip(tooltip);

    QLabel* kernel_para_label = new QLabel("Parameters");
    kernel_para_label->setAlignment(Qt::AlignRight | Qt::AlignTop);
    blob_grid->addWidget(kernel_para_label, 6, 0, 1, 1);
    tooltip = "r1, r2, r3 parameters for pixel sum integration";
    kernel_para_label->setToolTip(tooltip);

    QLabel* start_frame_label = new QLabel("Start frame");
    start_frame_label->setAlignment(Qt::AlignRight);
    blob_grid->addWidget(start_frame_label, 8, 0, 1, 1);
    tooltip = "(frame) - start frame for peak finding";
    start_frame_label->setToolTip(tooltip);

    QLabel* end_frame_label = new QLabel("End frame");
    end_frame_label->setAlignment(Qt::AlignRight);
    blob_grid->addWidget(end_frame_label, 9, 0, 1, 1);
    tooltip = "(frame) - end frame for peak finding";
    end_frame_label->setToolTip(tooltip);

    _threshold_spin = new QSpinBox();
    _scale_spin = new QDoubleSpinBox();
    _min_size_spin = new QSpinBox();
    _max_size_spin = new QSpinBox();
    _max_width_spin = new QSpinBox();
    _kernel_combo = new QComboBox;
    _kernel_para_table = new QTableWidget(this);
    _start_frame_spin = new QSpinBox();
    _end_frame_spin = new QSpinBox();
    _find_button = new QPushButton("Find peaks");

    _threshold_spin->setMaximum(10000000);
    _scale_spin->setMaximum(10000000);
    _min_size_spin->setMaximum(10000000);
    _max_size_spin->setMaximum(10000000);
    _max_width_spin->setMaximum(10000000);

    _threshold_spin->setMaximumWidth(1000);
    _scale_spin->setMaximumWidth(1000);
    _min_size_spin->setMaximumWidth(1000);
    _max_size_spin->setMaximumWidth(1000);
    _max_width_spin->setMaximumWidth(1000);
    _kernel_combo->setMaximumWidth(1000);
    _kernel_para_table->setMaximumWidth(1000);
    _start_frame_spin->setMaximumWidth(1000);
    _end_frame_spin->setMaximumWidth(1000);

    _threshold_spin->setAlignment(Qt::AlignLeft);
    _scale_spin->setAlignment(Qt::AlignLeft);
    _min_size_spin->setAlignment(Qt::AlignLeft);
    _max_size_spin->setAlignment(Qt::AlignLeft);
    _max_width_spin->setAlignment(Qt::AlignLeft);
    _start_frame_spin->setAlignment(Qt::AlignLeft);
    _end_frame_spin->setAlignment(Qt::AlignLeft);

    _threshold_spin->setSizePolicy(*_size_policy_widgets);
    _scale_spin->setSizePolicy(*_size_policy_widgets);
    _min_size_spin->setSizePolicy(*_size_policy_widgets);
    _max_size_spin->setSizePolicy(*_size_policy_widgets);
    _max_width_spin->setSizePolicy(*_size_policy_widgets);
    _kernel_combo->setSizePolicy(*_size_policy_widgets);
    _kernel_para_table->setSizePolicy(*_size_policy_widgets);
    _start_frame_spin->setSizePolicy(*_size_policy_widgets);
    _end_frame_spin->setSizePolicy(*_size_policy_widgets);

    blob_grid->addWidget(_threshold_spin, 0, 1, 1, 1);
    blob_grid->addWidget(_scale_spin, 1, 1, 1, 1);
    blob_grid->addWidget(_min_size_spin, 2, 1, 1, 1);
    blob_grid->addWidget(_max_size_spin, 3, 1, 1, 1);
    blob_grid->addWidget(_max_width_spin, 4, 1, 1, 1);
    blob_grid->addWidget(_kernel_combo, 5, 1, 1, 1);
    blob_grid->addWidget(_kernel_para_table, 7, 0, 1, 2);
    blob_grid->addWidget(_start_frame_spin, 8, 1, 1, 1);
    blob_grid->addWidget(_end_frame_spin, 9, 1, 1, 1);
    blob_grid->addWidget(_find_button, 10, 0, 1, 2);

    connect(_find_button, &QPushButton::clicked, this, &SubframeFindPeaks::find);

    blob_para->setContentLayout(*blob_grid, true);
    blob_para->setSizePolicy(*_size_policy_box);
    blob_para->contentArea.setSizePolicy(*_size_policy_box);

    _left_layout->addWidget(blob_para);
}

void SubframeFindPeaks::setIntegrateUp()
{
    Spoiler* integration_para = new Spoiler("3. Integration parameters");

    QGridLayout* integGrid = new QGridLayout();
    QString tooltip;

    QLabel* area_label = new QLabel("Peak end");
    area_label->setAlignment(Qt::AlignRight);
    integGrid->addWidget(area_label, 0, 0, 1, 1);
    tooltip = "(sigmas) - scaling factor for peak region";
    area_label->setToolTip(tooltip);

    QLabel* bck_lower_label = new QLabel("Bkg. begin");
    bck_lower_label->setAlignment(Qt::AlignRight);
    integGrid->addWidget(bck_lower_label, 1, 0, 1, 1);
    tooltip = "(sigmas) - scaling factor for lower limit of background";
    bck_lower_label->setToolTip(tooltip);

    QLabel* bck_upper_label = new QLabel("Bkg. end");
    bck_upper_label->setAlignment(Qt::AlignRight);
    integGrid->addWidget(bck_upper_label, 2, 0, 1, 1);
    tooltip = "(sigmas) - scaling factor for upper limit of background";
    bck_upper_label->setToolTip(tooltip);

    _peak_area = new QDoubleSpinBox();
    _bkg_lower = new QDoubleSpinBox();
    _bkg_upper = new QDoubleSpinBox();
    _integrate_button = new QPushButton("Integrate");

    _peak_area->setMaximum(10000000);
    _bkg_lower->setMaximum(10000000);
    _bkg_upper->setMaximum(10000000);

    _peak_area->setMaximumWidth(1000);
    _bkg_lower->setMaximumWidth(1000);
    _bkg_upper->setMaximumWidth(1000);

    _peak_area->setAlignment(Qt::AlignLeft);
    _bkg_lower->setAlignment(Qt::AlignLeft);
    _bkg_upper->setAlignment(Qt::AlignLeft);

    _peak_area->setSizePolicy(*_size_policy_widgets);
    _bkg_lower->setSizePolicy(*_size_policy_widgets);
    _bkg_upper->setSizePolicy(*_size_policy_widgets);

    integGrid->addWidget(_peak_area, 0, 1, 1, 1);
    integGrid->addWidget(_bkg_lower, 1, 1, 1, 1);
    integGrid->addWidget(_bkg_upper, 2, 1, 1, 1);
    integGrid->addWidget(_integrate_button, 3, 0, 1, 2);

    connect(_integrate_button, &QPushButton::clicked, this, &SubframeFindPeaks::integrate);

    integration_para->setContentLayout(*integGrid);
    integration_para->setSizePolicy(*_size_policy_box);
    integration_para->contentArea.setSizePolicy(*_size_policy_box);

    _left_layout->addWidget(integration_para);
}

void SubframeFindPeaks::setPreviewUp()
{
    Spoiler* preview_spoiler = new Spoiler("4. Show/hide peaks");
    _peak_view_widget = new PeakViewWidget("Valid peaks", "Invalid Peaks");

    connect(
        _peak_view_widget->drawPeaks1(), &QCheckBox::stateChanged, this,
        &SubframeFindPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->drawPeaks2(), &QCheckBox::stateChanged, this,
        &SubframeFindPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->drawBoxes1(), &QCheckBox::stateChanged, this,
        &SubframeFindPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->drawBoxes2(), &QCheckBox::stateChanged, this,
        &SubframeFindPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->drawBkg1(), &QCheckBox::stateChanged, this,
        &SubframeFindPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->drawBkg2(), &QCheckBox::stateChanged, this,
        &SubframeFindPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->peakSize1(),
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &SubframeFindPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->peakSize2(),
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &SubframeFindPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->peakColor1(), &ColorButton::colorChanged, this,
        &SubframeFindPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->peakColor2(), &ColorButton::colorChanged, this,
        &SubframeFindPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->boxColor1(), &ColorButton::colorChanged, this,
        &SubframeFindPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->boxColor2(), &ColorButton::colorChanged, this,
        &SubframeFindPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->bkgColor1(), &ColorButton::colorChanged, this,
        &SubframeFindPeaks::refreshPeakVisual);
    connect(
        _peak_view_widget->bkgColor2(), &ColorButton::colorChanged, this,
        &SubframeFindPeaks::refreshPeakVisual);

    preview_spoiler->setContentLayout(*_peak_view_widget);
    preview_spoiler->setSizePolicy(*_size_policy_box);
    preview_spoiler->contentArea.setSizePolicy(*_size_policy_box);

    _live_check = new QCheckBox("Apply threshold to preview");
    _live_check->setMaximumWidth(1000);
    _live_check->setSizePolicy(*_size_policy_widgets);
    // _peak_view_widget->addWidget(_live_check, 8, 0, 1, 3);
    // Not sure what the _live_check widget does - zamaan

    _left_layout->addWidget(preview_spoiler);
}

void SubframeFindPeaks::setSaveUp()
{
    _save_button = new QPushButton("Create peak collection");
    _save_button->setMaximumWidth(1000);
    _save_button->setSizePolicy(*_size_policy_widgets);
    _left_layout->addWidget(_save_button);
    _left_layout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    connect(_save_button, &QPushButton::clicked, this, &SubframeFindPeaks::accept);
}

void SubframeFindPeaks::setFigureUp()
{
    QGroupBox* figure_group = new QGroupBox("Preview");
    QGridLayout* figure_grid = new QGridLayout(figure_group);

    figure_group->setSizePolicy(*_size_policy_right);

    _figure_view = new DetectorView(this);
    _figure_view->getScene()->linkPeakModel(&_peak_collection_model);
    _figure_view->scale(1, -1);
    figure_grid->addWidget(_figure_view, 0, 0, 1, 2);

    _figure_scroll = new QScrollBar(this);
    _figure_scroll->setOrientation(Qt::Horizontal);
    _figure_scroll->setSizePolicy(*_size_policy_widgets);
    figure_grid->addWidget(_figure_scroll, 1, 0, 1, 1);

    _figure_spin = new QSpinBox(this);
    _figure_spin->setSizePolicy(*_size_policy_fixed);
    figure_grid->addWidget(_figure_spin, 1, 1, 1, 1);

    connect(
        _figure_scroll, SIGNAL(valueChanged(int)), _figure_view->getScene(),
        SLOT(slotChangeSelectedFrame(int)));

    connect(_figure_scroll, SIGNAL(valueChanged(int)), _figure_spin, SLOT(setValue(int)));

    connect(_figure_spin, SIGNAL(valueChanged(int)), _figure_scroll, SLOT(setValue(int)));

    connect(
        _figure_view->getScene(), &DetectorScene::signalSelectedPeakItemChanged, this,
        &SubframeFindPeaks::changeSelected);

    _right_element->addWidget(figure_group);
}

void SubframeFindPeaks::setPeakTableUp()
{
    QGroupBox* peak_group = new QGroupBox("Peaks");
    QGridLayout* peak_grid = new QGridLayout(peak_group);

    peak_group->setSizePolicy(*_size_policy_right);

    _peak_table = new PeakTableView(this);
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->setModel(&_peak_collection_model);

    _peak_table->setColumnHidden(0, true);
    _peak_table->setColumnHidden(1, true);
    _peak_table->setColumnHidden(2, true);

    peak_grid->addWidget(_peak_table, 0, 0, 0, 0);

    _right_element->addWidget(peak_group);
}

void SubframeFindPeaks::refreshAll()
{
    setParametersUp();
}

void SubframeFindPeaks::setParametersUp()
{
    if (gSession->experimentNames().empty())
        return;

    setExperimentsUp();
    refreshPeakTable();

    _kernel_combo->blockSignals(true);

    _kernel_combo->clear();
    nsx::ConvolverFactory convolver_factory;
    for (const auto& convolution_kernel_combo : convolver_factory.callbacks())
        _kernel_combo->addItem(QString::fromStdString(convolution_kernel_combo.first));

    _kernel_combo->blockSignals(false);

    _kernel_combo->setCurrentText("annular");
}

void SubframeFindPeaks::setExperimentsUp()
{
    _exp_combo->blockSignals(true);
    _exp_combo->clear();

    if (!gSession->experimentNames().empty()) {
        for (const QString& exp : gSession->experimentNames())
            _exp_combo->addItem(exp);
        grabFinderParameters();
        grabIntegrationParameters();
        updateDatasetList();
    }
    _exp_combo->blockSignals(false);
}

void SubframeFindPeaks::updateDatasetList()
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

void SubframeFindPeaks::updateDatasetParameters(int idx)
{
    if (_data_list.empty() || idx < 0)
        return;

    nsx::sptrDataSet data = _data_list.at(idx);

    _end_frame_spin->setMaximum(data->nFrames());
    _end_frame_spin->setValue(data->nFrames());
    _start_frame_spin->setMaximum(data->nFrames());

    _figure_view->getScene()->slotChangeSelectedData(_data_list.at(idx), 0);
    //_figure_view->getScene()->setMaxIntensity(3000);
    emit _figure_view->getScene()->dataChanged();
    _figure_view->getScene()->update();

    _figure_scroll->setMaximum(data->nFrames() - 1);
    _figure_scroll->setMinimum(0);

    _figure_spin->setMaximum(data->nFrames() - 1);
    _figure_spin->setMinimum(0);
}

void SubframeFindPeaks::grabFinderParameters()
{
    nsx::PeakFinder* finder =
        gSession->experimentAt(_exp_combo->currentIndex())->experiment()->peakFinder();

    _min_size_spin->setValue(finder->minSize());
    _max_size_spin->setValue(finder->maxSize());
    _scale_spin->setValue(finder->peakEnd());
    _max_width_spin->setValue(finder->maxFrames());
    _start_frame_spin->setValue(finder->framesBegin());
    _end_frame_spin->setValue(finder->framesEnd());
    _threshold_spin->setValue(finder->threshold());

    nsx::Convolver* convolver = finder->convolver();
    std::string convolverType = convolver->type();
    _kernel_combo->setCurrentText(QString::fromStdString(convolverType));

    const std::map<std::string, double>& params = convolver->parameters();
    using mapIterator = std::map<std::string, double>::const_iterator;

    _kernel_para_table->clear();
    _kernel_para_table->setRowCount(0);
    _kernel_para_table->setColumnCount(2);
    int currentRow = 0;
    for (mapIterator it = params.begin(); it != params.end(); ++it) {
        _kernel_para_table->insertRow(currentRow);

        QString name = QString::fromStdString(it->first);
        QTableWidgetItem* pname = new QTableWidgetItem();
        pname->setData(Qt::DisplayRole, name);
        pname->setFlags(pname->flags() ^ Qt::ItemIsEditable);


        double val = it->second;
        QTableWidgetItem* pvalue = new QTableWidgetItem();
        pvalue->setData(Qt::DisplayRole, val);

        _kernel_para_table->setItem(currentRow, 0, pname);
        _kernel_para_table->setItem(currentRow, 1, pvalue);

        currentRow++;
    }
    _kernel_para_table->resizeColumnsToContents();
}

void SubframeFindPeaks::setFinderParameters()
{
    nsx::PeakFinder* finder =
        gSession->experimentAt(_exp_combo->currentIndex())->experiment()->peakFinder();

    finder->setMinSize(_min_size_spin->value());
    finder->setMaxSize(_max_size_spin->value());
    finder->setPeakEnd(_scale_spin->value());
    finder->setMaxFrames(_max_width_spin->value());
    finder->setFramesBegin(_start_frame_spin->value());
    finder->setFramesEnd(_end_frame_spin->value());
    finder->setThreshold(_threshold_spin->value());

    std::string convolverType = _kernel_combo->currentText().toStdString();
    nsx::ConvolverFactory factory;
    nsx::Convolver* convolver = factory.create(convolverType, {});
    convolver->setParameters(convolutionParameters());
    finder->setConvolver(std::unique_ptr<nsx::Convolver>(convolver));
}

void SubframeFindPeaks::grabIntegrationParameters()
{
    nsx::IPeakIntegrator* integrator = gSession->experimentAt(_exp_combo->currentIndex())
                                           ->experiment()
                                           ->getIntegrator(std::string("Pixel sum integrator"));

    _peak_area->setValue(integrator->peakEnd());
    _bkg_lower->setValue(integrator->backBegin());
    _bkg_upper->setValue(integrator->backEnd());
}

void SubframeFindPeaks::updateConvolutionParameters()
{
    std::string kernelName = _kernel_combo->currentText().toStdString();
    nsx::ConvolverFactory _kernel_comboFactory;
    nsx::Convolver* kernel = _kernel_comboFactory.create(kernelName, {});

    const std::map<std::string, double>& params = kernel->parameters();
    using mapIterator = std::map<std::string, double>::const_iterator;

    _kernel_para_table->setRowCount(0);
    _kernel_para_table->setColumnCount(2);
    int currentRow = 0;
    for (mapIterator it = params.begin(); it != params.end(); ++it) {
        _kernel_para_table->insertRow(currentRow);

        QString name = QString::fromStdString(it->first);
        QTableWidgetItem* pname = new QTableWidgetItem();
        pname->setData(Qt::DisplayRole, name);
        pname->setFlags(pname->flags() ^ Qt::ItemIsEditable);


        QString val = QString::number(it->second);
        QTableWidgetItem* pvalue = new QTableWidgetItem();
        pvalue->setData(Qt::DisplayRole, val);

        _kernel_para_table->setItem(currentRow, 0, pname);
        _kernel_para_table->setItem(currentRow, 1, pvalue);

        currentRow++;
    }
    _kernel_para_table->resizeColumnsToContents();
}

void SubframeFindPeaks::find()
{
    nsx::DataList data_list;

    if (_all_data->isChecked()) {
        for (int i = 0; i < _data_list.size(); ++i)
            data_list.push_back(_data_list.at(i));
    } else {
        data_list.push_back(_data_list.at(_data_combo->currentIndex()));
    }

    nsx::PeakFinder* finder =
        gSession->experimentAt(_exp_combo->currentIndex())->experiment()->peakFinder();
    nsx::sptrProgressHandler progHandler = nsx::sptrProgressHandler(new nsx::ProgressHandler);
    ProgressView progressView(nullptr);
    progressView.watch(progHandler);
    finder->setHandler(progHandler);

    setFinderParameters();

    try {
        finder->find(data_list);
        refreshPeakTable();
    } catch (std::exception& e) {
        QMessageBox::critical(this, "Error", QString(e.what()));
    }
}

void SubframeFindPeaks::integrate()
{
    nsx::Experiment* experiment = gSession->experimentAt(_exp_combo->currentIndex())->experiment();

    nsx::IPeakIntegrator* integrator = experiment->getIntegrator("Pixel sum integrator");

    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
    ProgressView progressView(nullptr);
    progressView.watch(handler);

    nsx::IntegrationParameters params{};
    params.peak_end = _peak_area->value();
    params.bkg_begin = _bkg_lower->value();
    params.bkg_end = _bkg_upper->value();
    integrator->setParameters(params);
    integrator->setHandler(handler);

    experiment->integrateFoundPeaks("Pixel sum integrator");

    refreshPeakTable();
}

std::map<std::string, double> SubframeFindPeaks::convolutionParameters()
{
    std::map<std::string, double> parameters;
    for (int i = 0; i < _kernel_para_table->rowCount(); ++i) {
        std::string pname = _kernel_para_table->item(i, 0)->text().toStdString();
        double pvalue = _kernel_para_table->item(i, 1)->text().toDouble();
        parameters.insert(std::make_pair(pname, pvalue));
    }
    return parameters;
}

void SubframeFindPeaks::accept()
{
    nsx::PeakFinder* finder =
        gSession->experimentAt(_exp_combo->currentIndex())->experiment()->peakFinder();

    if (!finder->currentPeaks().empty()) {
        std::unique_ptr<ListNameDialog> dlg(new ListNameDialog());
        dlg->exec();
        if (!dlg->listName().isEmpty()) {
            gSession->experimentAt(_exp_combo->currentIndex())
                ->experiment()
                ->acceptFoundPeaks(dlg->listName().toStdString());
            gSession->experimentAt(_exp_combo->currentIndex())->generatePeakModel(dlg->listName());
        }
    }
}

void SubframeFindPeaks::refreshPreview()
{
    nsx::sptrDataSet dataset = _data_combo->currentData().value<nsx::sptrDataSet>();
    int selected = 0;
    int nrows = dataset->nRows();
    int ncols = dataset->nCols();

    std::string convolvertype = _kernel_combo->currentText().toStdString();
    std::map<std::string, double> convolverParams = convolutionParameters();
    Eigen::MatrixXd convolvedFrame =
        nsx::convolvedFrame(dataset->reader()->data(selected), convolvertype, convolverParams);
    if (_live_check->isChecked()) {
        double thresholdVal = _threshold_spin->value();
        for (int i = 0; i < nrows; ++i) {
            for (int j = 0; j < ncols; ++j)
                convolvedFrame(i, j) = convolvedFrame(i, j) < thresholdVal ? 0 : 1;
        }
    }
    double minVal = convolvedFrame.minCoeff();
    double maxVal = convolvedFrame.maxCoeff();
    if (maxVal - minVal <= 0.0)
        maxVal = minVal + 1.0;
    convolvedFrame.array() -= minVal;
    convolvedFrame.array() /= maxVal - minVal;
    QRect rect(0, 0, ncols, nrows);
    ColorMap* m = new ColorMap;
    QImage image = m->matToImage(convolvedFrame.cast<double>(), rect, maxVal);
    if (!_pixmap)
        _pixmap = _figure_view->scene()->addPixmap(QPixmap::fromImage(image));
    else
        _pixmap->setPixmap(QPixmap::fromImage(image));
    _figure_view->fitInView(_figure_view->scene()->sceneRect());
}

void SubframeFindPeaks::refreshPeakTable()
{
    std::vector<nsx::Peak3D*> peaks = gSession->experimentAt(_exp_combo->currentIndex())
                                          ->experiment()
                                          ->peakFinder()
                                          ->currentPeaks();

    _figure_view->getScene()->clearPeakItems();
    _peak_collection.populate(peaks);
    _peak_collection_item.setPeakCollection(&_peak_collection);
    _peak_collection_model.setRoot(&_peak_collection_item);

    _peak_table->setColumnHidden(0, true);
    _peak_table->setColumnHidden(1, true);
    _peak_table->setColumnHidden(2, true);

    refreshPeakVisual();
}

void SubframeFindPeaks::refreshPeakVisual()
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
            graphic->setCenterColor(_peak_view_widget->peakColor1()->getColor());
            graphic->showBox(_peak_view_widget->drawBoxes1()->isChecked());
            graphic->setBoxColor(_peak_view_widget->boxColor1()->getColor());
            graphic->showBkg(_peak_view_widget->drawBoxes1()->isChecked());
            graphic->setBkgColor(_peak_view_widget->bkgColor1()->getColor());
        } else {
            graphic->showLabel(false);
            graphic->showArea(_peak_view_widget->drawPeaks2()->isChecked());
            graphic->setSize(_peak_view_widget->peakSize2()->value());
            graphic->setColor(Qt::transparent);
            graphic->setCenterColor(_peak_view_widget->peakColor2()->getColor());
            graphic->showBox(_peak_view_widget->drawBoxes2()->isChecked());
            graphic->setBoxColor(_peak_view_widget->boxColor2()->getColor());
            graphic->showBkg(_peak_view_widget->drawBoxes2()->isChecked());
            graphic->setBkgColor(_peak_view_widget->bkgColor2()->getColor());
        }
    }
    _figure_view->getScene()->update();
    _figure_view->getScene()->drawPeakitems();
}

void SubframeFindPeaks::changeSelected(PeakItemGraphic* peak_graphic)
{
    int row = _peak_collection_item.returnRowOfVisualItem(peak_graphic);
    QModelIndex index = _peak_collection_model.index(row, 0);
    _peak_table->selectRow(row);
    _peak_table->scrollTo(index, QAbstractItemView::PositionAtTop);
}
