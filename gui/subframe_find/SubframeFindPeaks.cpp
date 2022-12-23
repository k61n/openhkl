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

#include "core/convolve/ConvolverFactory.h"
#include "core/data/DataSet.h"
#include "core/data/ImageGradient.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/PeakFinder.h"
#include "core/integration/IIntegrator.h"
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

#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QItemDelegate>
#include <QLabel>
#include <QMessageBox>
#include <QScrollBar>
#include <QSpacerItem>
#include <QTableWidgetItem>

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
    updateConvolutionParameters();

    _right_element->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto* propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_left_layout);
    main_layout->addWidget(propertyScrollArea);
    main_layout->addWidget(_right_element);

    _right_element->setStretchFactor(0, 2);
    _right_element->setStretchFactor(1, 1);

    connect(_kernel_combo, &QComboBox::currentTextChanged, [=](QString) {
        updateConvolutionParameters();
        refreshPreview();
    });
    connect(
        _gradient_kernel, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &SubframeFindPeaks::onGradientSettingsChanged);
    connect(
        _fft_gradient_check, &QCheckBox::stateChanged, this,
        &SubframeFindPeaks::onGradientSettingsChanged);
    connect(
        _gradient_check, &QCheckBox::clicked, this, &SubframeFindPeaks::onGradientSettingsChanged);
    connect(
        this, &SubframeFindPeaks::signalGradient, _detector_widget->scene(),
        &DetectorScene::onGradientSetting);
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

    _threshold_spin = f.addSpinBox(
        "Threshold", "(counts) - pixels with fewer counts than the threshold are discarded");

    _scale_spin = f.addDoubleSpinBox(
        "Merging scale",
        "(" + QString(QChar(0x03C3)) + ") - blob scaling factor to detect collisions");

    _min_size_spin = f.addSpinBox(
        "Minimum size", "(integer) - blobs containing fewer points than this count are discarded");

    _max_size_spin = f.addSpinBox(
        "Maximum size", "(integer) - blobs containing more points than this count are discarded");

    _max_width_spin = f.addSpinBox(
        "Maximum width", "(frames) - blob is discarded if it spans more frames than this value");

    _kernel_combo = f.addCombo("Convolution kernel", "Convolution kernel for peak search");

    ohkl::ConvolverFactory convolver_factory;
    for (const auto& convolution_kernel_combo : convolver_factory.callbacks())
        _kernel_combo->addItem(QString::fromStdString(convolution_kernel_combo.first));
    _kernel_combo->setCurrentText("annular");

    QLabel* kernel_para_label = new QLabel("Convolver parameters:");
    kernel_para_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    f.addWidget(kernel_para_label, 0);

    _kernel_para_table = new QTableWidget(this);
    f.addWidget(_kernel_para_table, 0);

    _start_frame_spin = f.addSpinBox(
        "First detector image", "(detector image number) - starting image for peak finding");

    _end_frame_spin =
        f.addSpinBox("Last detector image", "(detector image number) - end image for peak finding");

    _live_check = f.addCheckBox("Apply threshold to preview", "Only show pixels above threshold");

    _find_button = f.addButton("Find peaks");

    _threshold_spin->setMaximum(1000);
    _scale_spin->setMaximum(10);
    _min_size_spin->setMaximum(1000);
    _max_size_spin->setMaximum(100000);
    _max_width_spin->setMaximum(20);

    connect(_find_button, &QPushButton::clicked, this, &SubframeFindPeaks::find);
    connect(_live_check, &QCheckBox::stateChanged, this, &SubframeFindPeaks::refreshPreview);
    connect(
        gGui->sideBar(), &SideBar::subframeChanged, this,
        &SubframeFindPeaks::setIntegrationParameters);

    _left_layout->addWidget(blob_para);
}

void SubframeFindPeaks::setIntegrateUp()
{
    Spoiler* integration_para = new Spoiler("Integration parameters");
    GridFiller f(integration_para);

    _peak_area = f.addDoubleSpinBox(
        "Peak end", "(" + QString(QChar(0x03C3)) + ") - scaling factor for peak region");

    _bkg_lower = f.addDoubleSpinBox(
        "Background begin",
        "(" + QString(QChar(0x03C3)) + ") - scaling factor for lower limit of background");

    _bkg_upper = f.addDoubleSpinBox(
        "Background end",
        "(" + QString(QChar(0x03C3)) + ") - scaling factor for upper limit of background");

    _gradient_check = f.addCheckBox(
        "Compute gradient", "Compute mean gradient and sigma of background region", 1);

    _fft_gradient_check = f.addCheckBox("FFT gradient", "Use FFT to compute gradient", 1);

    _gradient_kernel = f.addCombo("Gradient kernel", "Convolution kernel used to compute gradient");

    _integrate_button = f.addButton("Integrate");

    _peak_area->setMaximum(10);
    _bkg_lower->setMaximum(10);
    _bkg_upper->setMaximum(10);

    for (const auto& [kernel, description] : _kernel_description)
        _gradient_kernel->addItem(description);
    _gradient_kernel->setCurrentIndex(1);

    connect(_integrate_button, &QPushButton::clicked, this, &SubframeFindPeaks::integrate);
    connect(
        gGui->sideBar(), &SideBar::subframeChanged, this, &SubframeFindPeaks::setFinderParameters);

    integration_para->setExpanded(true);
    _left_layout->addWidget(integration_para);
}

void SubframeFindPeaks::setPreviewUp()
{
    Spoiler* preview_spoiler = new Spoiler("Show/hide peaks");
    _peak_view_widget = new PeakViewWidget("Valid peaks", "Invalid Peaks");

    connect(
        _peak_view_widget, &PeakViewWidget::settingsChanged, this,
        &SubframeFindPeaks::refreshPeakVisual);

    connect(
        _peak_view_widget->set1.peakEnd, qOverload<double>(&QDoubleSpinBox::valueChanged),
        _peak_area, &QDoubleSpinBox::setValue);

    connect(
        _peak_view_widget->set1.bkgBegin, qOverload<double>(&QDoubleSpinBox::valueChanged),
        _bkg_lower, &QDoubleSpinBox::setValue);

    connect(
        _peak_view_widget->set1.bkgEnd, qOverload<double>(&QDoubleSpinBox::valueChanged),
        _bkg_upper, &QDoubleSpinBox::setValue);

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
        _detector_widget->spin(), static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this, &SubframeFindPeaks::refreshPreview);
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
    _peak_table->setColumnHidden(PeakColumn::Selected, true);
    _peak_table->setColumnHidden(PeakColumn::Count, true);

    peak_grid->addWidget(_peak_table, 0, 0, 0, 0);

    _right_element->addWidget(peak_group);
}

void SubframeFindPeaks::refreshAll()
{
    if (!gSession->hasProject())
        return;

    _data_combo->refresh();
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
    ohkl::PeakFinder* finder = gSession->currentProject()->experiment()->peakFinder();

    auto* params = gSession->currentProject()->experiment()->peakFinder()->parameters();

    _min_size_spin->setValue(params->minimum_size);
    _max_size_spin->setValue(params->maximum_size);
    _scale_spin->setValue(params->peak_end);
    _max_width_spin->setValue(params->maximum_frames);
    _start_frame_spin->setValue(params->frames_begin + 1);
    _end_frame_spin->setValue(params->frames_end);
    _threshold_spin->setValue(params->threshold);

    ohkl::Convolver* convolver = finder->convolver();
    std::string convolverType = convolver->type();
    _kernel_combo->setCurrentText(QString::fromStdString(convolverType));

    const std::map<std::string, double>& convolver_params = convolver->parameters();
    using mapIterator = std::map<std::string, double>::const_iterator;

    _kernel_para_table->clear();
    _kernel_para_table->setRowCount(0);
    _kernel_para_table->setColumnCount(2);
    int currentRow = 0;
    for (mapIterator it = convolver_params.begin(); it != convolver_params.end(); ++it) {
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
    if (!gSession->hasProject())
        return;

    ohkl::PeakFinder* finder = gSession->currentProject()->experiment()->peakFinder();

    auto* params = gSession->currentProject()->experiment()->peakFinder()->parameters();
    params->minimum_size = _min_size_spin->value();
    params->maximum_size = _max_size_spin->value();
    params->peak_end = _scale_spin->value();
    params->maximum_frames = _max_width_spin->value();
    params->frames_begin = _start_frame_spin->value() - 1;
    params->frames_end = _end_frame_spin->value() - 1;
    params->threshold = _threshold_spin->value();

    std::string convolverType = _kernel_combo->currentText().toStdString();
    ohkl::ConvolverFactory factory;
    ohkl::Convolver* convolver = factory.create(convolverType, {});
    convolver->setParameters(convolutionParameters());
    finder->setConvolver(std::unique_ptr<ohkl::Convolver>(convolver));
}

void SubframeFindPeaks::grabIntegrationParameters()
{
    auto* params = gSession->currentProject()->experiment()->integrator()->parameters();

    _peak_area->setValue(params->peak_end);
    _bkg_lower->setValue(params->bkg_begin);
    _bkg_upper->setValue(params->bkg_end);
    _gradient_check->setChecked(params->use_gradient);
    _fft_gradient_check->setChecked(params->fft_gradient);
    _gradient_kernel->setCurrentIndex(static_cast<int>(params->gradient_type));
}

void SubframeFindPeaks::setIntegrationParameters()
{
    if (!gSession->hasProject())
        return;

    auto* params = gSession->currentProject()->experiment()->integrator()->parameters();

    params->peak_end = _peak_area->value();
    params->bkg_begin = _bkg_lower->value();
    params->bkg_end = _bkg_upper->value();
    params->use_gradient = _gradient_check->isChecked();
    params->fft_gradient = _fft_gradient_check->isChecked();
    params->gradient_type = static_cast<ohkl::GradientKernel>(_gradient_kernel->currentIndex());
}

void SubframeFindPeaks::updateConvolutionParameters()
{
    std::string kernelName = _kernel_combo->currentText().toStdString();
    ohkl::ConvolverFactory _kernel_comboFactory;
    ohkl::Convolver* kernel = _kernel_comboFactory.create(kernelName, {});

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

void SubframeFindPeaks::refreshPreview()
{
    if (!_live_check->isChecked()) {
        if (_pixmap) {
            _detector_widget->scene()->removeItem(_pixmap);
            delete _pixmap;
            _pixmap = nullptr;
            _detector_widget->scene()->loadCurrentImage();
        }
        return;
    }

    ohkl::sptrDataSet data = _data_combo->currentData();
    int nrows = data->nRows();
    int ncols = data->nCols();

    std::string convolvertype = _kernel_combo->currentText().toStdString();
    std::map<std::string, double> convolverParams = convolutionParameters();
    Eigen::MatrixXd convolvedFrame = ohkl::convolvedFrame(
        data->reader()->data(_detector_widget->spin()->value() - 1), convolvertype,
        convolverParams);
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
        _pixmap = _detector_widget->scene()->addPixmap(QPixmap::fromImage(image));
    else
        _pixmap->setPixmap(QPixmap::fromImage(image));
}

void SubframeFindPeaks::refreshPeakTable()
{
    std::vector<ohkl::Peak3D*> peaks =
        gSession->currentProject()->experiment()->peakFinder()->currentPeaks();

    _peak_collection.populate(peaks);
    _peak_collection_item.setPeakCollection(&_peak_collection);
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->resizeColumnsToContents();

    _peak_table->setColumnHidden(PeakColumn::h, true);
    _peak_table->setColumnHidden(PeakColumn::k, true);
    _peak_table->setColumnHidden(PeakColumn::l, true);
    _peak_table->setColumnHidden(PeakColumn::Selected, true);
    _peak_table->setColumnHidden(PeakColumn::Count, true);

    refreshPeakVisual();
}

void SubframeFindPeaks::refreshPeakVisual()
{
    if (_peak_collection.numberOfPeaks() == 0)
        return;

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
    _find_button->setEnabled(true);
    _integrate_button->setEnabled(true);
    _save_button->setEnabled(true);
    _save_button->setToolTip("");
    if (!gSession->hasProject() || !gSession->currentProject()->hasDataSet()) {
        _find_button->setEnabled(false);
        _integrate_button->setEnabled(false);
        _save_button->setEnabled(false);
    }
    if (!_peaks_integrated) {
        _save_button->setEnabled(false);
        _save_button->setToolTip("Peaks must be integrated in order to create a peak collection");
    }
}

DetectorWidget* SubframeFindPeaks::detectorWidget()
{
    return _detector_widget;
}

void SubframeFindPeaks::onGradientSettingsChanged()
{
    emit signalGradient(_gradient_kernel->currentIndex(), _fft_gradient_check->isChecked());
}
