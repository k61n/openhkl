//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_find/SubframeFindPeaks.cpp
//! @brief     Implements classes FoundPeaks, PeakFinderFrame
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_find/SubframeFindPeaks.h"

#include "core/analyse/PeakFinder.h"
#include "core/convolve/ConvolverFactory.h"
#include "core/experiment/DataSet.h"
#include "core/integration/PixelSumIntegrator.h"
#include "core/peak/Peak3D.h"
#include "core/raw/IDataReader.h"

#include "gui/dialogs/ListNameDialog.h"
#include "gui/frames/ProgressView.h"
#include "gui/graphics/DetectorScene.h"

#include "gui/items/PeakItem.h"
#include "gui/models/Meta.h"
#include "gui/models/Session.h"
#include "gui/utility/Spoiler.h"

#include <QCR/engine/mixin.h>

#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QItemDelegate>
#include <QSpacerItem>
#include <QTableWidgetItem>
#include <QScrollBar>
#include <QScrollArea>



PeakFinderFrame::PeakFinderFrame() 
    : QWidget(), 
    _pixmap(nullptr),
    _peak_collection("temp", nsx::listtype::FOUND),
    _peak_collection_item(),
    _peak_collection_model()
{
    setSizePolicies();
    _main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical , this);

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

void PeakFinderFrame::setSizePolicies()
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

void PeakFinderFrame::setDataUp()
{
    Spoiler* _data_box = new Spoiler(QString::fromStdString("1. Input Data"));

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
        _exp_combo, static_cast<void (QComboBox::*) (int) >(&QComboBox::currentIndexChanged), 
        this, &PeakFinderFrame::updateDatasetList);

    connect(
        _data_combo, static_cast<void (QComboBox::*) (int) >(&QComboBox::currentIndexChanged), 
        this, &PeakFinderFrame::updateDatasetParameters);

    _data_box->setContentLayout(*_data_grid);
    _data_box->setSizePolicy(*_size_policy_box);
    _data_box->contentArea.setSizePolicy(*_size_policy_box);

    _left_layout->addWidget(_data_box);
}

void PeakFinderFrame::setBlobUp()
{
    Spoiler* blob_para = new Spoiler(QString::fromStdString("2. Peak search parameters"));

    QGridLayout* blob_grid = new QGridLayout();

    QLabel* threshold_label = new QLabel("Threshold");
    threshold_label->setAlignment(Qt::AlignRight);
    blob_grid->addWidget(threshold_label, 0, 0, 1, 1);

    QLabel* scale_label = new QLabel("Merging scale");
    scale_label->setAlignment(Qt::AlignRight);
    blob_grid->addWidget(scale_label, 1, 0, 1, 1);

    QLabel* min_size_label = new QLabel("Minimum size");
    min_size_label->setAlignment(Qt::AlignRight);
    blob_grid->addWidget(min_size_label, 2, 0, 1, 1);

    QLabel* max_size_label = new QLabel("Maximum size");
    max_size_label->setAlignment(Qt::AlignRight);
    blob_grid->addWidget(max_size_label, 3, 0, 1, 1);

    QLabel* max_width_label = new QLabel("Maximum width");
    max_width_label->setAlignment(Qt::AlignRight);
    blob_grid->addWidget(max_width_label, 4, 0, 1, 1);

    QLabel* kernel_label = new QLabel("Kernel");
    kernel_label->setAlignment(Qt::AlignRight);
    blob_grid->addWidget(kernel_label, 5, 0, 1, 1);

    QLabel* kernel_para_label = new QLabel("Parameters");
    kernel_para_label->setAlignment(Qt::AlignRight | Qt::AlignTop);
    blob_grid->addWidget(kernel_para_label, 6, 0, 1, 1);

    QLabel* start_frame_label = new QLabel("Start frame");
    start_frame_label->setAlignment(Qt::AlignRight);
    blob_grid->addWidget(start_frame_label, 7, 0, 1, 1);

    QLabel* end_frame_label = new QLabel("End frame");
    end_frame_label->setAlignment(Qt::AlignRight);
    blob_grid->addWidget(end_frame_label, 8, 0, 1, 1);
    
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
    blob_grid->addWidget(_kernel_para_table, 6, 1, 1, 1);
    blob_grid->addWidget(_start_frame_spin, 7, 1, 1, 1);
    blob_grid->addWidget(_end_frame_spin, 8, 1, 1, 1);
    blob_grid->addWidget(_find_button, 9, 0, 1, 2);

    connect(
        _find_button, &QPushButton::clicked, 
        this, &PeakFinderFrame::find);

    blob_para->setContentLayout(*blob_grid, true);
    blob_para->setSizePolicy(*_size_policy_box);
    blob_para->contentArea.setSizePolicy(*_size_policy_box);

    _left_layout->addWidget(blob_para);
}

void PeakFinderFrame::setIntegrateUp()
{
    Spoiler* integration_para = new Spoiler(QString::fromStdString("3. Integration parameters"));

    QGridLayout* integGrid = new QGridLayout();

    QLabel* area_label = new QLabel("Peak area");
    area_label->setAlignment(Qt::AlignRight);
    integGrid->addWidget(area_label, 0, 0, 1, 1);

    QLabel* bck_label = new QLabel("Background:");
    bck_label->setAlignment(Qt::AlignRight);
    integGrid->addWidget(bck_label, 1, 0, 1, 1);

    QLabel* bck_lower_label = new QLabel("Lower limit");
    bck_lower_label->setAlignment(Qt::AlignRight);
    integGrid->addWidget(bck_lower_label, 2, 0, 1, 1);

    QLabel* bck_upper_label = new QLabel("Upper limit");
    bck_upper_label->setAlignment(Qt::AlignRight);
    integGrid->addWidget(bck_upper_label, 3, 0, 1, 1);

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
    integGrid->addWidget(_bkg_lower, 2, 1, 1, 1);
    integGrid->addWidget(_bkg_upper, 3, 1, 1, 1);
    integGrid->addWidget(_integrate_button, 4, 0, 1, 2);

    connect(
        _integrate_button, &QPushButton::clicked, 
        this, &PeakFinderFrame::integrate);

    integration_para->setContentLayout(*integGrid);
    integration_para->setSizePolicy(*_size_policy_box);
    integration_para->contentArea.setSizePolicy(*_size_policy_box);

    _left_layout->addWidget(integration_para);
    
}

void PeakFinderFrame::setPreviewUp()
{
    Spoiler* preview_box = new Spoiler(QString::fromStdString("4. View and save"));

    QGridLayout* _preview_grid = new QGridLayout();

    QLabel* active_label = new QLabel("View behaviour valid:");
    active_label->setAlignment(Qt::AlignLeft);
    _preview_grid->addWidget(active_label, 0, 0, 1, 2);

    QLabel* active_size_label = new QLabel("Size:");
    active_size_label->setAlignment(Qt::AlignRight);
    _preview_grid->addWidget(active_size_label, 2, 0, 1, 1);

    QLabel* active_color_label = new QLabel("Color:");
    active_color_label->setAlignment(Qt::AlignRight);
    _preview_grid->addWidget(active_color_label, 3, 0, 1, 1);

    QLabel* inactive_label = new QLabel("View behaviour invalid:");
    inactive_label->setAlignment(Qt::AlignLeft);
    _preview_grid->addWidget(inactive_label, 4, 0, 1, 2);

    QLabel* inactive_size_label = new QLabel("Size:");
    inactive_size_label->setAlignment(Qt::AlignRight);
    _preview_grid->addWidget(inactive_size_label, 6, 0, 1, 1);

    QLabel* inactive_color_label = new QLabel("Color:");
    inactive_color_label->setAlignment(Qt::AlignRight);
    _preview_grid->addWidget(inactive_color_label, 7, 0, 1, 1);

    // QLabel* frame_label = new QLabel("Current data");
    // frame_label->setAlignment(Qt::AlignRight);
    // _preview_grid->addWidget(frame_label, 8, 0, 1, 1);

    _draw_active = new QCheckBox("Show");
    _draw_active->setCheckState(Qt::CheckState::Checked);

    _width_active = new QSpinBox();
    _width_active->setValue(10);

    _color_active = new ColorButton();
    _color_active->changeColor(Qt::black);

    _draw_inactive = new QCheckBox("show");
    _draw_inactive->setCheckState(Qt::CheckState::Checked);

    _width_inactive = new QSpinBox();
    _width_inactive->setValue(10);

    _color_inactive = new ColorButton();
    _color_inactive->changeColor(Qt::red);

    _live_check = new QCheckBox("Apply threshold to preview");
    _save_button = new QPushButton("Save");

    _draw_active->setMaximumWidth(1000);
    _width_active->setMaximumWidth(1000);
    _color_active->setMaximumWidth(1000);
    _draw_inactive->setMaximumWidth(1000);    
    _width_inactive->setMaximumWidth(1000);
    _color_inactive->setMaximumWidth(1000);
    _live_check->setMaximumWidth(1000);
    _save_button->setMaximumWidth(1000);

    _draw_active->setSizePolicy(*_size_policy_widgets);
    _width_active->setSizePolicy(*_size_policy_widgets);
    _color_active->setSizePolicy(*_size_policy_widgets);
    _draw_inactive->setSizePolicy(*_size_policy_widgets);
    _width_inactive->setSizePolicy(*_size_policy_widgets);
    _color_inactive->setSizePolicy(*_size_policy_widgets);
    _live_check->setSizePolicy(*_size_policy_widgets);
    _save_button->setSizePolicy(*_size_policy_widgets);

    _preview_grid->addWidget(_draw_active, 1, 1, 1, 1);
    _preview_grid->addWidget(_width_active, 2, 1, 1, 1);
    _preview_grid->addWidget(_color_active, 3, 1, 1, 1);
    _preview_grid->addWidget(_draw_inactive, 5, 1, 1, 1);
    _preview_grid->addWidget(_width_inactive, 6, 1, 1, 1);
    _preview_grid->addWidget(_color_inactive, 7, 1, 1, 1);
    _preview_grid->addWidget(_live_check, 8, 0, 1, 2);
    _preview_grid->addWidget(_save_button, 9, 0, 1, 2);

    connect(
        _save_button, &QPushButton::clicked, 
        this, &PeakFinderFrame::accept);

    connect(
        _draw_active, &QCheckBox::stateChanged, 
        this, &PeakFinderFrame::refreshPeakVisual);

    connect(
        _width_active, static_cast<void (QSpinBox::*) (int) >(&QSpinBox::valueChanged), 
        this, &PeakFinderFrame::refreshPeakVisual);

    connect(
        _color_active, &ColorButton::colorChanged, 
        this, &PeakFinderFrame::refreshPeakVisual);

    connect(
        _draw_inactive, &QCheckBox::stateChanged, 
        this, &PeakFinderFrame::refreshPeakVisual);

    connect(
        _width_inactive, static_cast<void (QSpinBox::*) (int) >(&QSpinBox::valueChanged), 
        this, &PeakFinderFrame::refreshPeakVisual);

    connect(
        _color_inactive, &ColorButton::colorChanged, 
        this, &PeakFinderFrame::refreshPeakVisual);

    preview_box->setContentLayout(*_preview_grid);
    preview_box->setSizePolicy(*_size_policy_box);
    preview_box->contentArea.setSizePolicy(*_size_policy_box);

    _left_layout->addWidget(preview_box);
    _left_layout->addItem(
        new QSpacerItem(
            20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

void PeakFinderFrame::setFigureUp()
{
    QGroupBox* figure_group = new QGroupBox("Preview");
    QGridLayout* figure_grid = new QGridLayout(figure_group);

    figure_group->setSizePolicy(*_size_policy_right);

    _figure_view = new DetectorView(this);
    _figure_view->getScene()->linkPeakModel(&_peak_collection_model);
    _figure_view->scale(1, -1);
    figure_grid->addWidget(_figure_view, 0,0,1,2);
    
    _figure_scroll = new QScrollBar(this);
    _figure_scroll->setOrientation(Qt::Horizontal);
    _figure_scroll->setSizePolicy(*_size_policy_widgets);
    figure_grid->addWidget(_figure_scroll, 1,0,1,1);

    _figure_spin = new QSpinBox(this);
    _figure_spin->setSizePolicy(*_size_policy_fixed);
    figure_grid->addWidget(_figure_spin, 1,1,1,1);

    connect(
        _figure_scroll, SIGNAL(valueChanged(int)), 
        _figure_view->getScene(), SLOT(slotChangeSelectedFrame(int)));

    connect(
        _figure_scroll, SIGNAL(valueChanged(int)), 
        _figure_spin, SLOT(setValue(int)));

    connect(
        _figure_view->getScene(), &DetectorScene::signalSelectedPeakItemChanged, 
        this, &PeakFinderFrame::changeSelected);

    _right_element->addWidget(figure_group);
}

void PeakFinderFrame::setPeakTableUp()
{
    QGroupBox* peak_group = new QGroupBox("Peaks");
    QGridLayout* peak_grid = new QGridLayout(peak_group);

    peak_group->setSizePolicy(*_size_policy_right);

    _peak_table = new PeaksTableView(this);
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->setModel(&_peak_collection_model);

    _peak_table->setColumnHidden(0, true);
    _peak_table->setColumnHidden(1, true);
    _peak_table->setColumnHidden(2, true);

    peak_grid->addWidget(_peak_table, 0,0,0,0);

    _right_element->addWidget(peak_group);
}

void PeakFinderFrame::refreshAll()
{
    setParametersUp();
}

void PeakFinderFrame::setParametersUp()
{
    QList<QString> exp_list = gSession->experimentNames();
    if (exp_list.isEmpty()){
        return;
    }

    setExperimentsUp();
    refreshPeakTable();

    _kernel_combo->blockSignals(true);

    _kernel_combo->clear();
    nsx::ConvolverFactory convolver_factory;
    for (auto&& convolution_kernel_combo : convolver_factory.callbacks())
        _kernel_combo->addItem(QString::fromStdString(convolution_kernel_combo.first));

    _kernel_combo->blockSignals(false);

    _kernel_combo->setCurrentText("annular");
}

void PeakFinderFrame::setExperimentsUp()
{
    _exp_combo->blockSignals(true);
    
    _exp_combo->clear();
    QList<QString> exp_list = gSession->experimentNames();

    if (!exp_list.isEmpty()){
        for (QString exp : exp_list) {
            _exp_combo->addItem(exp);
        }
        grabFinderParameters();
        grabIntegrationParameters();
        updateDatasetList();
    }
    _exp_combo->blockSignals(false);
}

void PeakFinderFrame::updateDatasetList()
{
    _data_combo->blockSignals(true);
    _data_combo->clear();
    _data_list = gSession->experimentAt(_exp_combo->currentIndex())->allData();

    if (!_data_list.isEmpty()){
        for (nsx::sptrDataSet data : _data_list) {
            QFileInfo fileinfo(QString::fromStdString(data->filename()));
            _data_combo->addItem(fileinfo.baseName());
        }
        _data_combo->setCurrentIndex(0);
        updateDatasetParameters(0);
    }
    _data_combo->blockSignals(false);
}

void PeakFinderFrame::updateDatasetParameters(int idx)
{
    if (_data_list.isEmpty() || idx < 0)
        return;

    nsx::sptrDataSet data = _data_list.at(idx);

    _end_frame_spin->setMaximum(data->nFrames());
    _end_frame_spin->setValue(data->nFrames());
    _start_frame_spin->setMaximum(data->nFrames());

    _figure_view->getScene()->slotChangeSelectedData(_data_list.at(idx), 0);
    _figure_view->getScene()->setMaxIntensity(3000);
    _figure_view->getScene()->update();

    _figure_scroll->setMaximum(data->nFrames());
    _figure_scroll->setMinimum(0);
    
    _figure_spin->setMaximum(data->nFrames());
    _figure_spin->setMinimum(0);
} 

void PeakFinderFrame::grabFinderParameters()
{
    nsx::PeakFinder* finder = gSession->experimentAt(_exp_combo->currentIndex())->experiment()->peakFinder();

    _min_size_spin->setValue(finder->minSize());
    _max_size_spin->setValue(finder->maxSize());
    _scale_spin->setValue(finder->peakScale());
    _max_width_spin->setValue(finder->maxFrames());
    _start_frame_spin->setValue(finder->framesBegin());
    _end_frame_spin->setValue(finder->framesEnd());
    _threshold_spin->setValue(finder->threshold());

    nsx::Convolver* convolver = finder->convolver();
    std::string convolverType = convolver->type();
    _kernel_combo->setCurrentText(QString::fromStdString(convolverType));

    const std::map<std::string, double>& params = convolver->parameters();
    typedef std::map<std::string, double>::const_iterator mapIterator;

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

void PeakFinderFrame::setFinderParameters()
{
    nsx::PeakFinder* finder = gSession->experimentAt(_exp_combo->currentIndex())->experiment()->peakFinder();

    finder->setMinSize(_min_size_spin->value());
    finder->setMaxSize(_max_size_spin->value());
    finder->setPeakScale(_scale_spin->value());
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

void PeakFinderFrame::grabIntegrationParameters()
{
    nsx::PixelSumIntegrator* integrator = gSession->experimentAt(_exp_combo->currentIndex())->experiment()->peakFoundIntegrator();

    _peak_area->setValue(integrator->peakEnd());
    _bkg_lower->setValue(integrator->backBegin());
    _bkg_upper->setValue(integrator->backEnd());
}

void PeakFinderFrame::updateConvolutionParameters()
{
    std::string kernelName = _kernel_combo->currentText().toStdString();
    nsx::ConvolverFactory _kernel_comboFactory;
    nsx::Convolver* kernel = _kernel_comboFactory.create(kernelName, {});

    const std::map<std::string, double>& params = kernel->parameters();
    typedef std::map<std::string, double>::const_iterator mapIterator;

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

void PeakFinderFrame::find()
{
    nsx::DataList data_list;

    if (_all_data->isChecked())
    {
        for (int i = 0; i < _data_list.size(); ++i)
            data_list.push_back(_data_list.at(i));
    }else{
        data_list.push_back(_data_list.at(_data_combo->currentIndex()));
    }

    nsx::PeakFinder* finder = gSession->experimentAt(_exp_combo->currentIndex())->experiment()->peakFinder();
    nsx::sptrProgressHandler progHandler = nsx::sptrProgressHandler(new nsx::ProgressHandler);
    ProgressView progressView(nullptr);
    progressView.watch(progHandler);
    finder->setHandler(progHandler);

    setFinderParameters();

    try {
        finder->find(data_list);
        refreshPeakTable();

    } catch (std::exception& e) {
        return;
    }
}

void PeakFinderFrame::integrate()
{
    nsx::sptrExperiment experiment = gSession->experimentAt(_exp_combo->currentIndex())->experiment();
    experiment->integrateFoundPeaks(
        _peak_area->value(), 
        _bkg_lower->value(),
        _bkg_upper->value()
    );

    refreshPeakTable();
}

std::map<std::string, double> PeakFinderFrame::convolutionParameters()
{
    std::map<std::string, double> parameters;
    for (int i = 0; i < _kernel_para_table->rowCount(); ++i) {
        std::string pname = _kernel_para_table->item(i, 0)->text().toStdString();
        double pvalue = _kernel_para_table->item(i, 1)->text().toDouble();
        parameters.insert(std::make_pair(pname, pvalue));
        qDebug() << "5." << i;
    }
    return parameters;
}

void PeakFinderFrame::accept()
{
    nsx::PeakFinder* finder = gSession->experimentAt(_exp_combo->currentIndex())->experiment()->peakFinder();

    if (!finder->currentPeaks().empty()){
        gLogger->log("@accept");
        std::unique_ptr<ListNameDialog> dlg(new ListNameDialog());
        dlg->exec();
        if (!dlg->listName().isEmpty()){
            gSession->experimentAt(_exp_combo->currentIndex())->experiment()->acceptFoundPeaks(dlg->listName().toStdString());
            gSession->experimentAt(_exp_combo->currentIndex())->generatePeakModel(dlg->listName());
        }
    }
}

void PeakFinderFrame::refreshPreview()
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

void PeakFinderFrame::refreshPeakTable()
{
    std::vector<nsx::Peak3D*> peaks = 
        gSession->experimentAt(_exp_combo->currentIndex())->experiment()->peakFinder()->currentPeaks();

    _figure_view->getScene()->clearPeakItems();
    _peak_collection.populate(&peaks);
    _peak_collection_item.setPeakCollection(&_peak_collection);
    _peak_collection_model.setRoot(&_peak_collection_item);

    _peak_table->setColumnHidden(0, true);
    _peak_table->setColumnHidden(1, true);
    _peak_table->setColumnHidden(2, true);

    refreshPeakVisual();
}

void PeakFinderFrame::refreshPeakVisual()
{   
    if (_peak_collection_item.childCount()==0)
        return;

    bool valid;
    PeakItemGraphic* graphic;

    for (int i = 0; i < _peak_collection_item.childCount(); i++){
        PeakItem* peak = _peak_collection_item.peakItemAt(i);
        graphic = peak->peakGraphic();
        valid = peak->peak()->enabled();

        if (valid){
            graphic->showArea((_draw_active->checkState() == Qt::CheckState::Checked));
            graphic->showLabel(false);
            graphic->setSize(_width_active->value());
            graphic->setColor(_color_active->getColor());
        }else{
            graphic->showArea((_draw_inactive->checkState() == Qt::CheckState::Checked));
            graphic->showLabel(false);
            graphic->setSize(_width_inactive->value());
            graphic->setColor(_color_inactive->getColor());
        }
    }
    _figure_view->getScene()->update();
    _figure_view->getScene()->drawPeakitems();
}

void PeakFinderFrame::changeSelected(PeakItemGraphic* peak_graphic)
{
    int row = _peak_collection_item.returnRowOfVisualItem(peak_graphic);
    QModelIndex index = _peak_collection_model.index(row, 0);
    _peak_table->selectRow(row);
    _peak_table->scrollTo(index, QAbstractItemView::PositionAtTop);
}
