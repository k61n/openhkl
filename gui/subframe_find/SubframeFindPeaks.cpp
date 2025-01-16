//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_find/SubframeFindPeaks.cpp
//! @brief     Implements classes FoundPeaks, SubframeFindPeaks
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_find/SubframeFindPeaks.h"

#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/Integrator.h"
#include "core/experiment/PeakFinder.h"
#include "core/image/GradientFilter.h"
#include "core/integration/IIntegrator.h"
#include "core/peak/IntegrationRegion.h"
#include "core/peak/Peak3D.h"
#include "gui/MainWin.h" // gGui
#include "gui/connect/Sentinel.h"
#include "gui/dialogs/ListNameDialog.h"
#include "gui/frames/ProgressView.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/items/PeakItem.h"
#include "gui/models/Meta.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/ColorButton.h"
#include "gui/utility/DataComboBox.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/PropertyScrollArea.h"
#include "gui/utility/SafeSpinBox.h"
#include "gui/utility/SideBar.h"
#include "gui/utility/Spoiler.h"
#include "gui/views/PeakTableView.h"
#include "gui/widgets/DetectorWidget.h"
#include "gui/widgets/PeakViewWidget.h"

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollBar>
#include <QSpacerItem>
#include <QSplitter>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <qglobal.h>
#include <qspinbox.h>

SubframeFindPeaks::SubframeFindPeaks()
    : QWidget()
    , _peak_collection("temp", ohkl::PeakCollectionType::FOUND, nullptr)
    , _peak_collection_item()
    , _peak_collection_model()
    , _peaks_integrated(false)
    , _pixmap(nullptr)
{
    auto* main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);

    _left_layout = new QVBoxLayout();

    setDataUp();
    setBlobUp();
    setIntegrateUp();
    setPreviewUp();
    setSaveUp();
    setFigureUp();
    setPeakTableUp();
    toggleUnsafeWidgets();

    _right_element->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto* propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_left_layout);
    main_layout->addWidget(propertyScrollArea);
    main_layout->addWidget(_right_element);

    _right_element->setStretchFactor(0, 2);
    _right_element->setStretchFactor(1, 1);

    connect(
        _gradient_kernel, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &SubframeFindPeaks::onGradientSettingsChanged);
    connect(
        _compute_gradient, &QGroupBox::clicked, this, &SubframeFindPeaks::onGradientSettingsChanged);
    connect(
        this, &SubframeFindPeaks::signalGradient, _detector_widget->scene(),
        &DetectorScene::onGradientSetting);
    connect(
        _peak_view_widget, &PeakViewWidget::settingsChanged, _detector_widget,
        &DetectorWidget::refresh);

    connect(
        _integration_region_type,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), _detector_widget,
        &DetectorWidget::refresh);
}

void SubframeFindPeaks::setDataUp()
{
    Spoiler* _data_box = new Spoiler("Input");
    GridFiller f(_data_box);

    _data_combo = f.addDataCombo("Data set");

    connect(_data_combo, &QComboBox::currentTextChanged, this, &SubframeFindPeaks::refreshAll);

    connect(
        _data_combo, &QComboBox::currentTextChanged, this, &SubframeFindPeaks::toggleUnsafeWidgets);

    _left_layout->addWidget(_data_box);
    _data_box->setExpanded(true);
}

void SubframeFindPeaks::setBlobUp()
{
    Spoiler* blob_para = new Spoiler("Peak search parameters");
    GridFiller f(blob_para, true);

    _threshold_spin = f.addDoubleSpinBox(
        "Threshold", "(counts) - pixels with fewer counts than the threshold are discarded");

    _scale_spin = f.addDoubleSpinBox(
        "Merging scale",
        "(" + QString(QChar(0x03C3)) + ") - blob scaling factor to detect collisions");

    std::tie(_min_size_spin, _max_size_spin) = f.addSpinBoxPair(
        "Blob size range", "(counts) - only blobs containing counts in this range will be kept");

    _max_width_spin = f.addSpinBox(
        "Maximum width", "(frames) - blob is discarded if it spans more frames than this value");

    _kernel_combo = f.addCombo("Convolution kernel", "Convolution kernel for peak search");

    auto kernel_types = ohkl::ImageFilterStrings;
    for (auto it = kernel_types.begin(); it != kernel_types.end(); ++it)
        _kernel_combo->addItem(QString::fromStdString(it->second));
    _kernel_combo->setCurrentIndex(1);

    QLabel* kernel_para_label = new QLabel("Filter parameters:");
    kernel_para_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    f.addWidget(kernel_para_label, 0);

    _r1 = f.addDoubleSpinBox("r1", "Upper bound for positive region of filter kernel");
    _r2 = f.addDoubleSpinBox("r2", "Lower bound for negative region of filter kernel");
    _r3 = f.addDoubleSpinBox("r3", "Upper bound for negative region of filter kernel");

    std::tie(_start_frame_spin, _end_frame_spin) =
        f.addSpinBoxPair("Image range", "start and end image of range in which to find peaks");

    _threshold_check =
        f.addCheckBox("Apply threshold to preview", "Only show pixels above threshold");

    _find_button = f.addButton("Find peaks");

    _threshold_spin->setMaximum(1000);
    _threshold_spin->setValue(1);
    _scale_spin->setMaximum(10);
    _min_size_spin->setMaximum(1000);
    _max_size_spin->setMaximum(100000);
    _max_width_spin->setMaximum(20);
    _r1->setMaximum(20);
    _r2->setMaximum(20);
    _r3->setMaximum(20);
    _r1->setMinimum(1);
    _r2->setMinimum(1);
    _r3->setMinimum(1);
    _r1->setSingleStep(1);
    _r2->setSingleStep(1);
    _r3->setSingleStep(1);
    _r1->setValue(5);
    _r2->setValue(10);
    _r3->setValue(15);
    _start_frame_spin->setMinimum(1);
    _end_frame_spin->setMinimum(1);

    connect(_find_button, &QPushButton::clicked, this, &SubframeFindPeaks::find);
    connect(
        _threshold_check, &QCheckBox::stateChanged, this, &SubframeFindPeaks::showFilteredImage);
    connect(
        _threshold_spin,
        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        this, &SubframeFindPeaks::showFilteredImage);
    connect(
        _r1, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
        &SubframeFindPeaks::showFilteredImage);
    connect(
        _r2, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
        &SubframeFindPeaks::showFilteredImage);
    connect(
        _r3, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
        &SubframeFindPeaks::showFilteredImage);

    _left_layout->addWidget(blob_para);
}

void SubframeFindPeaks::setIntegrateUp()
{
    Spoiler* integration_para = new Spoiler("Integration parameters");
    GridFiller f(integration_para);

    _integration_region_type = f.addCombo("Integration region type");
    for (int i = 0; i < static_cast<int>(ohkl::RegionType::Count); ++i)
        _integration_region_type->addItem(QString::fromStdString(
            ohkl::regionTypeDescription.at(static_cast<ohkl::RegionType>(i))));

    _peak_end = f.addDoubleSpinBox(
        "Peak end", "(" + QString(QChar(0x03C3)) + ") - scaling factor for peak region");

    _bkg_begin = f.addDoubleSpinBox(
        "Background begin",
        "(" + QString(QChar(0x03C3)) + ") - scaling factor for lower limit of background");

    _bkg_end = f.addDoubleSpinBox(
        "Background end",
        "(" + QString(QChar(0x03C3)) + ") - scaling factor for upper limit of background");

    _use_max_width = new QGroupBox("Maximum width for integration");
    _use_max_width->setAlignment(Qt::AlignLeft);
    _use_max_width->setCheckable(true);
    _use_max_width->setChecked(false);
    _use_max_width->setToolTip("Skip integration of peaks spanning too many images");

    _max_width = new SafeSpinBox();
    _max_width->setMaximum(100);

    QLabel* label = new QLabel("Maximum width");
    label->setToolTip("Maximum width for peak to be integrated");
    QGridLayout* grid = new QGridLayout();
    _use_max_width->setLayout(grid);
    grid->addWidget(label, 0, 0, 1, 1);
    grid->addWidget(_max_width, 0, 1, 1, 1);
    f.addWidget(_use_max_width);

    _compute_gradient = new QGroupBox("Compute gradient");
    _compute_gradient->setAlignment(Qt::AlignLeft);
    _compute_gradient->setCheckable(true);
    _compute_gradient->setChecked(false);
    _compute_gradient->setToolTip("Compute image gradient");

    _gradient_kernel = new QComboBox();

    grid = new QGridLayout();
    _compute_gradient->setLayout(grid);
    label = new QLabel("Kernel");
    grid->addWidget(label, 0, 0, 1, 1);
    grid->addWidget(_gradient_kernel, 0, 1, 1, 1);
    f.addWidget(_compute_gradient);

    _integrate_button = f.addButton("Integrate");

    _peak_end->setMaximum(50);
    _bkg_begin->setMaximum(50);
    _bkg_end->setMaximum(50);

    for (const auto& [kernel, description] : ohkl::GradientFilterStrings)
        _gradient_kernel->addItem(QString::fromStdString(description));
    _gradient_kernel->setCurrentIndex(0);

    connect(_integrate_button, &QPushButton::clicked, this, &SubframeFindPeaks::integrate);

    integration_para->setExpanded(true);
    _left_layout->addWidget(integration_para);
}

void SubframeFindPeaks::setPreviewUp()
{
    Spoiler* preview_spoiler = new Spoiler("Show/hide peaks");
    _peak_view_widget = new PeakViewWidget("Valid peaks", "Invalid Peaks");

    connect(
        _peak_view_widget->set1.peakEnd, qOverload<double>(&QDoubleSpinBox::valueChanged),
        _peak_end, &QDoubleSpinBox::setValue);

    connect(
        _peak_view_widget->set1.bkgBegin, qOverload<double>(&QDoubleSpinBox::valueChanged),
        _bkg_begin, &QDoubleSpinBox::setValue);

    connect(
        _peak_view_widget->set1.bkgEnd, qOverload<double>(&QDoubleSpinBox::valueChanged), _bkg_end,
        &QDoubleSpinBox::setValue);

    connect(
        _peak_view_widget->set1.regionType, &QComboBox::currentTextChanged,
        _integration_region_type, &QComboBox::setCurrentText);

    connect(
        _peak_end, qOverload<double>(&QDoubleSpinBox::valueChanged),
        _peak_view_widget->set1.peakEnd, &QDoubleSpinBox::setValue);

    connect(
        _bkg_begin, qOverload<double>(&QDoubleSpinBox::valueChanged),
        _peak_view_widget->set1.bkgBegin, &QDoubleSpinBox::setValue);

    connect(
        _bkg_end, qOverload<double>(&QDoubleSpinBox::valueChanged), _peak_view_widget->set1.bkgEnd,
        &QDoubleSpinBox::setValue);

    connect(
        _integration_region_type, &QComboBox::currentTextChanged,
        _peak_view_widget->set1.regionType, &QComboBox::setCurrentText);

    preview_spoiler->setContentLayout(*_peak_view_widget);

    _left_layout->addWidget(preview_spoiler);
}

void SubframeFindPeaks::setSaveUp()
{
    _save_button = new QPushButton("Create peak collection");
    _left_layout->addWidget(_save_button);
    connect(_save_button, &QPushButton::clicked, this, &SubframeFindPeaks::accept);
}

void SubframeFindPeaks::setFigureUp()
{
    QGroupBox* figure_group = new QGroupBox("Detector image");
    figure_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _detector_widget = new DetectorWidget(1, true, true, figure_group);
    _detector_widget->linkPeakModel(&_peak_collection_model, _peak_view_widget);

    connect(
        _data_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        _detector_widget->dataCombo(), &QComboBox::setCurrentIndex);
    connect(
        _detector_widget->dataCombo(), QOverload<int>::of(&QComboBox::currentIndexChanged),
        _data_combo, &QComboBox::setCurrentIndex);
    connect(
        _detector_widget->scene(), &DetectorScene::signalUpdateDetectorScene, this,
        &SubframeFindPeaks::refreshPeakTable);
    connect(
        _detector_widget->scene(), &DetectorScene::signalSelectedPeakItemChanged, this,
        &SubframeFindPeaks::changeSelected);

    _right_element->addWidget(figure_group);
}

void SubframeFindPeaks::setPeakTableUp()
{
    QGroupBox* peak_group = new QGroupBox("Peaks");
    QGridLayout* peak_grid = new QGridLayout(peak_group);

    peak_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    _peak_table = new PeakTableView(this);
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->setModel(&_peak_collection_model);
    _peak_table->resizeColumnsToContents();

    _peak_table->setColumnHidden(PeakColumn::h, true);
    _peak_table->setColumnHidden(PeakColumn::k, true);
    _peak_table->setColumnHidden(PeakColumn::l, true);
    _peak_table->setColumnHidden(PeakColumn::Enabled, true);
    _peak_table->setColumnHidden(PeakColumn::Count, true);
    _peak_table->setColumnHidden(PeakColumn::ProfileIntensity, true);
    _peak_table->setColumnHidden(PeakColumn::ProfileSigma, true);
    _peak_table->setColumnHidden(PeakColumn::ProfileStrength, true);
    _peak_table->setColumnHidden(PeakColumn::ProfileBkg, true);
    _peak_table->setColumnHidden(PeakColumn::ProfileBkgSigma, true);

    peak_grid->addWidget(_peak_table, 0, 0, 0, 0);

    _right_element->addWidget(peak_group);
}

void SubframeFindPeaks::refreshAll()
{
    if (!gSession->hasProject())
        return;

    _detector_widget->refresh();
    grabFinderParameters();
    if (gSession->currentProject()->hasDataSet()) {
        auto data = _data_combo->currentData();
        _end_frame_spin->setMinimum(1);
        _end_frame_spin->setMaximum(data->nFrames());
        _end_frame_spin->setValue(data->nFrames());
        _start_frame_spin->setMinimum(1);
        _start_frame_spin->setMaximum(data->nFrames());
        _start_frame_spin->setValue(1);
    }

    grabIntegrationParameters();
    refreshPeakTable();
    toggleUnsafeWidgets();
}

void SubframeFindPeaks::grabFinderParameters()
{
    if (!gSession->hasProject())
        return;

    auto* params = gSession->currentProject()->experiment()->peakFinder()->parameters();

    _min_size_spin->setValue(params->minimum_size);
    _max_size_spin->setValue(params->maximum_size);
    _scale_spin->setValue(params->peak_end);
    _max_width_spin->setValue(params->maximum_frames);
    _start_frame_spin->setValue(params->first_frame + 1);
    _end_frame_spin->setValue(params->last_frame + 1);
    _r1->setValue(params->r1);
    _r2->setValue(params->r2);
    _r3->setValue(params->r3);
    _threshold_spin->setValue(params->threshold);
    _kernel_combo->setCurrentText(QString::fromStdString(params->filter));
}

void SubframeFindPeaks::setFinderParameters()
{
    if (!gSession->hasProject())
        return;

    ohkl::PeakFinder* finder = gSession->currentProject()->experiment()->peakFinder();

    auto* params = gSession->currentProject()->experiment()->peakFinder()->parameters();
    params->minimum_size = _min_size_spin->value();
    params->maximum_size = _max_size_spin->value();
    params->peak_end = _scale_spin->value();
    params->maximum_frames = _max_width_spin->value();
    params->first_frame = _start_frame_spin->value() - 1;
    params->last_frame = _end_frame_spin->value() - 1;
    params->r1 = _r1->value();
    params->r2 = _r2->value();
    params->r3 = _r3->value();
    params->threshold = _threshold_spin->value();
    params->filter = _kernel_combo->currentText().toStdString();

    _detector_widget->scene()->params()->convolver_params = finder->filterParameters();
}

void SubframeFindPeaks::grabIntegrationParameters()
{
    auto* params = gSession->currentProject()->experiment()->integrator()->parameters();

    QSignalBlocker blocker1(_integration_region_type);
    QSignalBlocker blocker2(_peak_end);
    QSignalBlocker blocker3(_bkg_begin);
    QSignalBlocker blocker4(_bkg_end);
    _integration_region_type->setCurrentIndex(static_cast<int>(params->region_type));

    if (params->region_type == ohkl::RegionType::VariableEllipsoid) {
        _peak_end->setValue(params->peak_end);
        _bkg_begin->setValue(params->bkg_begin);
        _bkg_end->setValue(params->bkg_end);
    } else {
        _peak_end->setValue(params->fixed_peak_end);
        _bkg_begin->setValue(params->fixed_bkg_begin);
        _bkg_end->setValue(params->fixed_bkg_end);
    }

    _use_max_width->setChecked(params->use_max_width);
    _max_width->setValue(params->max_width);
    _compute_gradient->setChecked(params->use_gradient);
    _gradient_kernel->setCurrentIndex(static_cast<int>(params->gradient_type));
}

void SubframeFindPeaks::setIntegrationParameters()
{
    if (!gSession->hasProject())
        return;

    auto* params = gSession->currentProject()->experiment()->integrator()->parameters();

    params->region_type = static_cast<ohkl::RegionType>(_integration_region_type->currentIndex());
    if (params->region_type == ohkl::RegionType::VariableEllipsoid) {
        params->peak_end = _peak_end->value();
        params->bkg_begin = _bkg_begin->value();
        params->bkg_end = _bkg_end->value();
    } else {
        params->fixed_peak_end = _peak_end->value();
        params->fixed_bkg_begin = _bkg_begin->value();
        params->fixed_bkg_end = _bkg_end->value();
    }
    params->use_max_width = _use_max_width->isChecked();
    params->max_width = _max_width->value();
    params->use_gradient = _compute_gradient->isChecked();
    params->gradient_type = static_cast<ohkl::GradientFilterType>(_gradient_kernel->currentIndex());
}

void SubframeFindPeaks::find()
{
    gGui->setReady(false);

    ohkl::sptrDataSet data = _data_combo->currentData();
    ohkl::PeakFinder* finder = gSession->currentProject()->experiment()->peakFinder();
    ohkl::sptrProgressHandler progHandler = ohkl::sptrProgressHandler(new ohkl::ProgressHandler);
    ProgressView progressView(nullptr);
    progressView.watch(progHandler);
    finder->setHandler(progHandler);

    setFinderParameters();

    try {
        finder->find(data);
        refreshPeakTable();
    } catch (std::exception& e) {
        QMessageBox::critical(this, "Error", QString(e.what()));
    }
    gGui->statusBar()->showMessage(QString::number(finder->numberFound()) + " peaks found");
    toggleUnsafeWidgets();
    gGui->setReady(true);
}

void SubframeFindPeaks::integrate()
{
    gGui->setReady(false);
    auto* experiment = gSession->currentProject()->experiment();
    auto* integrator = experiment->integrator();
    auto* finder = experiment->peakFinder();

    ohkl::sptrProgressHandler handler(new ohkl::ProgressHandler);
    ProgressView progressView(nullptr);
    progressView.watch(handler);

    setIntegrationParameters();
    integrator->getIntegrator(ohkl::IntegratorType::PixelSum)->setHandler(handler);

    integrator->integrateFoundPeaks(finder);
    refreshPeakTable();
    _peaks_integrated = true;
    toggleUnsafeWidgets();
    gGui->statusBar()->showMessage(
        QString::number(integrator->numberOfValidPeaks()) + "/"
        + QString::number(integrator->numberOfPeaks()) + " peaks integrated");
    gGui->setReady(true);
}

void SubframeFindPeaks::accept()
{
    auto expt = gSession->currentProject()->experiment();
    ohkl::PeakFinder* finder = expt->peakFinder();

    if (finder->currentPeaks().empty())
        return;
    std::unique_ptr<ListNameDialog> dlg(
        new ListNameDialog(QString::fromStdString(expt->generatePeakCollectionName())));
    dlg->exec();
    if (dlg->listName().isEmpty())
        return;
    if (dlg->result() == QDialog::Rejected)
        return;
    if (!gSession->currentProject()->experiment()->acceptFoundPeaks(
            dlg->listName().toStdString(), _peak_collection)) {
        QMessageBox::warning(
            this, "Unable to add PeakCollection", "Collection with this name already exists!");
        return;
    }
    gSession->currentProject()->generatePeakModel(dlg->listName());
    gSession->onPeaksChanged();
    gGui->refreshMenu();
}

void SubframeFindPeaks::refreshPeakTable()
{
    if (!gSession->currentProject()->hasDataSet())
        return;

    std::vector<ohkl::Peak3D*> peaks =
        gSession->currentProject()->experiment()->peakFinder()->currentPeaks();

    _peak_collection.populate(peaks);
    _peak_collection.setData(_data_combo->currentData());
    _peak_collection_item.setPeakCollection(&_peak_collection);
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->resizeColumnsToContents();

    _peak_table->setColumnHidden(PeakColumn::h, true);
    _peak_table->setColumnHidden(PeakColumn::k, true);
    _peak_table->setColumnHidden(PeakColumn::l, true);
    _peak_table->setColumnHidden(PeakColumn::Enabled, true);
    _peak_table->setColumnHidden(PeakColumn::Count, true);

    ohkl::PeakFinder* finder = gSession->currentProject()->experiment()->peakFinder();
    _detector_widget->scene()->params()->convolver_params = finder->filterParameters();
    _detector_widget->scene()->params()->filter =
        static_cast<ohkl::ImageFilterType>(_kernel_combo->currentIndex());
    _detector_widget->refresh();
}

void SubframeFindPeaks::changeSelected(PeakItemGraphic* peak_graphic)
{
    int row = _peak_collection_item.returnRowOfVisualItem(peak_graphic);
    QModelIndex index = _peak_collection_model.index(row, 0);
    _peak_table->selectRow(row);
    _peak_table->scrollTo(index, QAbstractItemView::PositionAtTop);
}

void SubframeFindPeaks::toggleUnsafeWidgets()
{
    _find_button->setEnabled(false);
    _integrate_button->setEnabled(false);
    _save_button->setEnabled(false);
    _save_button->setToolTip("");

    if (!gSession->hasProject())
        return;

    auto* expt = gSession->currentProject()->experiment();
    auto* finder = expt->peakFinder();

    _find_button->setEnabled(gSession->currentProject()->hasDataSet());
    _integrate_button->setEnabled(finder->foundPeaks());
    _save_button->setEnabled(_peaks_integrated);

    if (!_peaks_integrated)
        _save_button->setToolTip("Peaks must be integrated in order to create a peak collection");
}

DetectorWidget* SubframeFindPeaks::detectorWidget()
{
    return _detector_widget;
}

void SubframeFindPeaks::onGradientSettingsChanged()
{
    emit signalGradient(_gradient_kernel->currentIndex());
}

void SubframeFindPeaks::showFilteredImage()
{
    setFinderParameters();
    _detector_widget->scene()->params()->filteredImage = _threshold_check->isChecked();
    _detector_widget->scene()->params()->threshold = _threshold_spin->value();
    if (_threshold_check->isChecked()) {
        _detector_widget->scene()->params()->filter =
            static_cast<ohkl::ImageFilterType>(_kernel_combo->currentIndex());
    }
    _detector_widget->scene()->loadCurrentImage();
}
