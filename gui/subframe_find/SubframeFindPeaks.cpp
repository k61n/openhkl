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
#include "core/peak/Peak3D.h"
#include "core/shape/IPeakIntegrator.h"
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
#include "gui/utility/GridFiller.h"
#include "gui/utility/LinkedComboBox.h"
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
    , _peak_collection("temp", nsx::listtype::FOUND)
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

    _right_element->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto* propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_left_layout);
    main_layout->addWidget(propertyScrollArea);
    main_layout->addWidget(_right_element);

    connect(_kernel_combo, &QComboBox::currentTextChanged, [=](QString) {
        updateConvolutionParameters();
        refreshPreview();
    });
}

void SubframeFindPeaks::setDataUp()
{
    Spoiler* _data_box = new Spoiler("Input");
    GridFiller f(_data_box);

    _data_combo = f.addLinkedCombo(ComboType::DataSet, "Data set");
    _all_data = f.addCheckBox("Search all", "Find peaks in all data sets", 1);

    connect(
        _data_combo, &QComboBox::currentTextChanged, this,
        &SubframeFindPeaks::updateDatasetParameters);

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

    _scale_spin =
        f.addDoubleSpinBox("Merging scale", "(sigmas) - blob scaling factor to detect collisions");

    _min_size_spin = f.addSpinBox(
        "Minimum size", "(integer) - blobs containing fewer points than this count are discarded");

    _max_size_spin = f.addSpinBox(
        "Maximum size", "(integer) - blobs containing more points than this count are discarded");

    _max_width_spin = f.addSpinBox(
        "Maximum width", "(frames) - blob is discarded if it spans more frames than this value");

    _kernel_combo = f.addCombo("Kernel", "Convolution kernel for peak search");

    nsx::ConvolverFactory convolver_factory;
    for (const auto& convolution_kernel_combo : convolver_factory.callbacks())
        _kernel_combo->addItem(QString::fromStdString(convolution_kernel_combo.first));
    _kernel_combo->setCurrentText("annular");

    QLabel* kernel_para_label = new QLabel("Parameters:");
    kernel_para_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    kernel_para_label->setToolTip("r1, r2, r3 parameters for pixel sum integration");
    f.addWidget(kernel_para_label, 0);

    _kernel_para_table = new QTableWidget(this);
    f.addWidget(_kernel_para_table, 0);

    _start_frame_spin = f.addSpinBox("Start frame", "(frame) - start frame for peak finding");

    _end_frame_spin = f.addSpinBox("End frame", "(frame) - end frame for peak finding");

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

    _peak_area = f.addDoubleSpinBox("Peak end", "(sigmas) - scaling factor for peak region");

    _bkg_lower =
        f.addDoubleSpinBox("Bkg. begin", "(sigmas) - scaling factor for lower limit of background");

    _bkg_upper =
        f.addDoubleSpinBox("Bkg. end", "(sigmas) - scaling factor for upper limit of background");

    _integrate_button = f.addButton("Integrate");

    _peak_area->setMaximum(10);
    _bkg_lower->setMaximum(10);
    _bkg_upper->setMaximum(10);

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
    QGroupBox* figure_group = new QGroupBox("Preview");
    figure_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _detector_widget = new DetectorWidget(true, false, true, figure_group);
    _detector_widget->modeCombo()->addItems(
        QStringList{"Zoom", "Selection", "Rectangular mask", "Elliptical mask"});
    _detector_widget->linkPeakModel(&_peak_collection_model);

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

    _peak_table->setColumnHidden(0, true);
    _peak_table->setColumnHidden(1, true);
    _peak_table->setColumnHidden(2, true);

    peak_grid->addWidget(_peak_table, 0, 0, 0, 0);

    _right_element->addWidget(peak_group);
}

void SubframeFindPeaks::refreshAll()
{
    setParametersUp();
    if (gSession->hasProject())
        _detector_widget->updateDatasetList(gSession->currentProject()->allData());
    toggleUnsafeWidgets();
}

void SubframeFindPeaks::setParametersUp()
{
    if (!gSession->hasProject())
        return;

    grabFinderParameters();
    grabIntegrationParameters();
    updateDatasetList();
    refreshPeakTable();
}

void SubframeFindPeaks::updateDatasetList()
{
    Project* project = gSession->currentProject();
    if (!project->hasDataSet())
        return;

    QSignalBlocker blocker(_data_combo);
    QString current_data = _data_combo->currentText();
    _data_combo->clear();
    
    const QStringList& datanames{project->getDataNames()};
    if (current_data.isEmpty()) current_data = datanames.at(0);
    _data_combo->addItems(datanames);
    _data_combo->setCurrentText(current_data);
    updateDatasetParameters(_data_combo->currentText());
}

void SubframeFindPeaks::updateDatasetParameters(const QString& dataname)
{
    // to be update on the experiment/project list if a new
    // experiment is added on SubframeHome
    auto* exp = gSession->currentProject()->experiment();
    if (!exp->hasData(dataname.toStdString())) {
        QMessageBox::warning(
            nullptr, "Dataset does not exist",
            "The given dataset " + dataname + " could not be found!");
        return;
    }
    nsx::sptrDataSet data = exp->getData(dataname.toStdString());

    _end_frame_spin->setMaximum(data->nFrames());
    _end_frame_spin->setValue(data->nFrames());
    _start_frame_spin->setMaximum(data->nFrames());
    _start_frame_spin->setValue(1);
}

void SubframeFindPeaks::grabFinderParameters()
{
    nsx::PeakFinder* finder =
        gSession->currentProject()->experiment()->peakFinder();

    auto* params = gSession->currentProject()->experiment()->peakFinder()->parameters();

    _min_size_spin->setValue(params->minimum_size);
    _max_size_spin->setValue(params->maximum_size);
    _scale_spin->setValue(params->peak_end);
    _max_width_spin->setValue(params->maximum_frames);
    _start_frame_spin->setValue(params->frames_begin + 1);
    _end_frame_spin->setValue(params->frames_end);
    _threshold_spin->setValue(params->threshold);

    nsx::Convolver* convolver = finder->convolver();
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

    nsx::PeakFinder* finder =
        gSession->currentProject()->experiment()->peakFinder();

    auto* params = gSession->currentProject()->experiment()->peakFinder()->parameters();
    params->minimum_size = _min_size_spin->value();
    params->maximum_size = _max_size_spin->value();
    params->peak_end = _scale_spin->value();
    params->maximum_frames = _max_width_spin->value();
    params->frames_begin = _start_frame_spin->value();
    params->frames_end = _end_frame_spin->value();
    params->threshold = _threshold_spin->value();

    std::string convolverType = _kernel_combo->currentText().toStdString();
    nsx::ConvolverFactory factory;
    nsx::Convolver* convolver = factory.create(convolverType, {});
    convolver->setParameters(convolutionParameters());
    finder->setConvolver(std::unique_ptr<nsx::Convolver>(convolver));
}

void SubframeFindPeaks::grabIntegrationParameters()
{
    auto* params = gSession->currentProject()->experiment()->integrator()->parameters();

    _peak_area->setValue(params->peak_end);
    _bkg_lower->setValue(params->bkg_begin);
    _bkg_upper->setValue(params->bkg_end);
}

void SubframeFindPeaks::setIntegrationParameters()
{
    if (!gSession->hasProject())
        return;

    auto* params = gSession->currentProject()->experiment()->integrator()->parameters();

    params->peak_end = _peak_area->value();
    params->bkg_begin = _bkg_lower->value();
    params->bkg_end = _bkg_upper->value();
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
    gGui->setReady(false);
    nsx::DataList data_list;
    const nsx::DataList all_data = gSession->currentProject()->allData();

    int idx = _data_combo->currentIndex();
    
    if (idx >= all_data.size() || idx == -1){
        _data_combo->setCurrentIndex(0);
    }

    if (_all_data->isChecked()) {
        for (int i = 0; i < all_data.size(); ++i)
            data_list.push_back(all_data.at(i));
    } else {
        int idx = _data_combo->currentIndex();
        if (idx < all_data.size()){
            data_list.push_back(all_data.at(idx));
        }
    }

    nsx::PeakFinder* finder =
        gSession->currentProject()->experiment()->peakFinder();
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
    gGui->statusBar()->showMessage(QString::number(finder->numberFound()) + " peaks found");
    gGui->setReady(true);
}

void SubframeFindPeaks::integrate()
{
    gGui->setReady(false);
    auto* experiment = gSession->currentProject()->experiment();
    auto* integrator = experiment->integrator();
    auto* finder = experiment->peakFinder();

    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
    ProgressView progressView(nullptr);
    progressView.watch(handler);

    setIntegrationParameters();
    integrator->getIntegrator(nsx::IntegratorType::PixelSum)->setHandler(handler);

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
    nsx::PeakFinder* finder = expt->peakFinder();

    if (finder->currentPeaks().empty())
        return;
    std::unique_ptr<ListNameDialog> dlg(
        new ListNameDialog(QString::fromStdString(expt->generatePeakCollectionName())));
    dlg->exec();
    if (dlg->listName().isEmpty())
        return;
    if (dlg->result() == QDialog::Rejected)
        return;
    if (!gSession->currentProject()->experiment()
             ->acceptFoundPeaks(dlg->listName().toStdString(), _peak_collection)) {
        QMessageBox::warning(
            this, "Unable to add PeakCollection", "Collection with this name already exists!");
        return;
    }
    gSession->currentProject()->generatePeakModel(dlg->listName());
    gGui->sentinel->addLinkedComboItem(ComboType::FoundPeaks, dlg->listName());
    gGui->sentinel->addLinkedComboItem(ComboType::PeakCollection, dlg->listName());
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

    nsx::sptrDataSet data = gSession->currentProject()->experiment()
                            ->getData(_data_combo->currentText().toStdString());
    int nrows = data->nRows();
    int ncols = data->nCols();

    std::string convolvertype = _kernel_combo->currentText().toStdString();
    std::map<std::string, double> convolverParams = convolutionParameters();
    Eigen::MatrixXd convolvedFrame = nsx::convolvedFrame(
        data->reader()->data(_detector_widget->spin()->value()), convolvertype, convolverParams);
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
    std::vector<nsx::Peak3D*> peaks =
        gSession->currentProject()->experiment()->peakFinder()->currentPeaks();

    _peak_collection.populate(peaks);
    _peak_collection_item.setPeakCollection(&_peak_collection);
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->resizeColumnsToContents();

    _peak_table->setColumnHidden(0, true);
    _peak_table->setColumnHidden(1, true);
    _peak_table->setColumnHidden(2, true);

    refreshPeakVisual();
}

void SubframeFindPeaks::refreshPeakVisual()
{
    if (_peak_collection.numberOfPeaks() == 0)
        return;

    for (int i = 0; i < _peak_collection_item.childCount(); i++) {
        PeakItem* peak = _peak_collection_item.peakItemAt(i);
        auto graphic = peak->peakGraphic();

        graphic->showLabel(false);
        graphic->setColor(Qt::transparent);
        graphic->initFromPeakViewWidget(
            peak->peak()->enabled() ? _peak_view_widget->set1 : _peak_view_widget->set2);
        _detector_widget->scene()->initIntRegionFromPeakWidget(_peak_view_widget->set1);
    }
    _detector_widget->scene()->initIntRegionFromPeakWidget(_peak_view_widget->set1);
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
