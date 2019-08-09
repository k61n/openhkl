//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/PeakFinderFrame.cpp
//! @brief     Implements classes FoundPeaks, PeakFinderFrame
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/frames/PeakFinderFrame.h"

#include "core/analyse/PeakFinder.h"
#include "core/convolve/ConvolverFactory.h"
#include "core/experiment/DataSet.h"
#include "core/integration/PixelSumIntegrator.h"
#include "core/peak/Peak3D.h"
#include "core/raw/IDataReader.h"
#include "gui/dialogs/ListNameDialog.h"
#include "gui/frames/ProgressView.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/models/Meta.h"
#include "gui/models/Session.h"

#include <QCR/engine/mixin.h>

#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QItemDelegate>
#include <QSpacerItem>
#include <QTableWidgetItem>


PeakFinderFrame::PeakFinderFrame() : QcrFrame {"peakFinder"}, pixmap(nullptr)
{
    if (gSession->selectedExperimentNum() < 0) {
        gLogger->log("[ERROR] No experiment selected");
        return;
    }
    if (gSession->selectedExperiment()->getDataNames().empty()) {
        gLogger->log("[ERROR] No data loaded for selected experiment");
        return;
    }
    // Layout
    setAttribute(Qt::WA_DeleteOnClose);

    main_layout = new QHBoxLayout(this);
    left_layout = new QVBoxLayout;

    setSizePolicies();
    setBlobUp();
    setPreviewUp();
    setIntegrateUp();
    setExecuteUp();
    left_layout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    main_layout->addLayout(left_layout);
    setFigureUp();
    setParametersUp();

    // flip the image vertically to conform with DetectorScene
    preview->scale(1, -1);
    updateConvolutionParameters();
    _min_size_spin->setHook([=](int i) { _min_size_spin->setCellValue(std::min(i, _max_size_spin->value())); });
    _max_size_spin->setHook([=](int i) { _max_size_spin->setCellValue(std::max(i, _min_size_spin->value())); });
    peakArea->setHook(
        [=](double d) { peakArea->setCellValue(std::min(d, backgroundLowerLimit->value())); });
    backgroundLowerLimit->setHook([=](double d) {
        d = std::max(d, peakArea->value());
        backgroundLowerLimit->setCellValue(std::min(d, backgroundUpperLimit->value()));
    });
    backgroundUpperLimit->setHook([=](double d) {
        backgroundUpperLimit->setCellValue(std::max(d, backgroundLowerLimit->value()));
    });
    applyThreshold->setHook([=](bool) { refreshPreview(); });
    frame->setHook([=](int) { refreshPreview(); });
    connect(_kernel_combo, &QComboBox::currentTextChanged, [=](QString) {
        updateConvolutionParameters();
        refreshPreview();
    });

    show();
}

void PeakFinderFrame::setSizePolicies()
{
    _size_policy_widgets = new QSizePolicy();
    _size_policy_widgets->setHorizontalPolicy(QSizePolicy::Expanding);
    _size_policy_widgets->setVerticalPolicy(QSizePolicy::Fixed);
    
    _size_policy_box = new QSizePolicy();
    _size_policy_box->setHorizontalPolicy(QSizePolicy::Preferred);
    _size_policy_box->setVerticalPolicy(QSizePolicy::Preferred);
}

void PeakFinderFrame::setBlobUp()
{
    QGroupBox* blob_para = new QGroupBox("Peak search parameter");
    QGridLayout* blobGrid = new QGridLayout(blob_para);

    blob_para->setSizePolicy(*_size_policy_box);

    QLabel* threshold_label = new QLabel("Threshold");
    threshold_label->setAlignment(Qt::AlignRight);
    blobGrid->addWidget(threshold_label, 0, 0, 1, 1);

    QLabel* scale_label = new QLabel("Merging scale");
    scale_label->setAlignment(Qt::AlignRight);
    blobGrid->addWidget(scale_label, 1, 0, 1, 1);

    QLabel* min_size_label = new QLabel("Minimum size");
    min_size_label->setAlignment(Qt::AlignRight);
    blobGrid->addWidget(min_size_label, 2, 0, 1, 1);

    QLabel* max_size_label = new QLabel("Maximum size");
    max_size_label->setAlignment(Qt::AlignRight);
    blobGrid->addWidget(max_size_label, 3, 0, 1, 1);

    QLabel* max_width_label = new QLabel("Maximum width");
    max_width_label->setAlignment(Qt::AlignRight);
    blobGrid->addWidget(max_width_label, 4, 0, 1, 1);

    QLabel* kernel_label = new QLabel("Kernel");
    kernel_label->setAlignment(Qt::AlignRight);
    blobGrid->addWidget(kernel_label, 5, 0, 1, 1);

    QLabel* kernel_para_label = new QLabel("Parameters");
    kernel_para_label->setAlignment(Qt::AlignRight | Qt::AlignTop);
    blobGrid->addWidget(kernel_para_label, 6, 0, 1, 1);

    QLabel* start_frame_label = new QLabel("Start frame");
    start_frame_label->setAlignment(Qt::AlignRight);
    blobGrid->addWidget(start_frame_label, 7, 0, 1, 1);

    QLabel* end_frame_label = new QLabel("End frame");
    end_frame_label->setAlignment(Qt::AlignRight);
    blobGrid->addWidget(end_frame_label, 8, 0, 1, 1);
    
    _threshold_spin = new QcrSpinBox(
        "adhoc__threshold_spin", 
        new QcrCell<int>(80), 5);
    _scale_spin = new QcrDoubleSpinBox(
        "adhoc__scale_spin", 
        new QcrCell<double>(1.000), 5, 5);
    _min_size_spin = new QcrSpinBox(
        "adhoc__min_size_spin", 
        new QcrCell<int>(30), 5);
    _max_size_spin = new QcrSpinBox(
        "adhoc__max_size_spin", 
        new QcrCell<int>(10000), 5);
    _max_width_spin = new QcrSpinBox(
        "adhoc__max_width_spin", 
        new QcrCell<int>(10), 5);
    _kernel_combo = new QComboBox;
    _kernel_para_table = new QTableWidget(this);
    _start_frame_spin = new QcrSpinBox(
        "adhoc_beginFrame", 
        new QcrCell<int>(0), 3);
    _end_frame_spin = new QcrSpinBox(
        "adhoc_endFrame", 
        new QcrCell<int>(0), 3);

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

    blobGrid->addWidget(_threshold_spin, 0, 1, 1, 1);
    blobGrid->addWidget(_scale_spin, 1, 1, 1, 1);
    blobGrid->addWidget(_min_size_spin, 2, 1, 1, 1);
    blobGrid->addWidget(_max_size_spin, 3, 1, 1, 1);
    blobGrid->addWidget(_max_width_spin, 4, 1, 1, 1);
    blobGrid->addWidget(_kernel_combo, 5, 1, 1, 1);
    blobGrid->addWidget(_kernel_para_table, 6, 1, 1, 1);
    blobGrid->addWidget(_start_frame_spin, 7, 1, 1, 1);
    blobGrid->addWidget(_end_frame_spin, 8, 1, 1, 1);

    left_layout->addWidget(blob_para);

}

void PeakFinderFrame::setPreviewUp()
{
    QGroupBox* preview_box = new QGroupBox("Preview");
    QGridLayout* previewGrid = new QGridLayout(preview_box);

    preview_box->setSizePolicy(*_size_policy_box);

    previewGrid->addWidget(new QLabel("Data"), 0, 0, 1, 1);
    previewGrid->addWidget(new QLabel("Frame"), 1, 0, 1, 1);
    applyThreshold = new QcrCheckBox(
        "adhoc_applyThreshold", "apply threshold to preview", new QcrCell<bool>(false));
    previewGrid->addWidget(applyThreshold, 2, 0, 1, 1);
    data = new QComboBox;
    frame = new QcrSpinBox("adhoc_frameNr", new QcrCell<int>(0), 3);
    previewGrid->addWidget(data, 0, 1, 1, 1);
    previewGrid->addWidget(frame, 1, 1, 1, 1);
    left_layout->addWidget(preview_box);
}

void PeakFinderFrame::setIntegrateUp()
{
    QGroupBox* integration_para = new QGroupBox("Integration parameters");
    QGridLayout* integGrid = new QGridLayout(integration_para);

    integration_para->setSizePolicy(*_size_policy_box);

    integGrid->addWidget(new QLabel("Peak area"), 0, 0, 1, 1);
    integGrid->addWidget(new QLabel("Background lower limit"), 1, 0, 1, 1);
    integGrid->addWidget(new QLabel("Background upper limit"), 2, 0, 1, 1);

    peakArea = new QcrDoubleSpinBox("adhoc_area", new QcrCell<double>(3.0), 5, 2);
    backgroundLowerLimit = new QcrDoubleSpinBox("adhoc_lowLimit", new QcrCell<double>(4.0), 5, 2);
    backgroundUpperLimit = new QcrDoubleSpinBox("adhoc_upLimit", new QcrCell<double>(4.5), 5, 2);

    integGrid->addWidget(peakArea, 0, 1, 1, 1);
    integGrid->addWidget(backgroundLowerLimit, 1, 1, 1, 1);
    integGrid->addWidget(backgroundUpperLimit, 2, 1, 1, 1);

    left_layout->addWidget(integration_para);
    
}

void PeakFinderFrame::setFigureUp()
{
    preview = new DetectorView(this);
    preview->setSizePolicy(*_size_policy_figure);
    main_layout->addWidget(preview);
}

void PeakFinderFrame::setExecuteUp()
{
    QHBoxLayout* button_grid = new QHBoxLayout();

    _find_button = new QPushButton("Find peaks");
    _integrate_button = new QPushButton("Interate");
    _save_button = new QPushButton("Save");

    button_grid->addWidget(_find_button);
    button_grid->addWidget(_integrate_button);
    button_grid->addWidget(_save_button);

    connect(
        _find_button, &QPushButton::clicked, 
        this, &PeakFinderFrame::find);
    connect(
        _integrate_button, &QPushButton::clicked, 
        this, &PeakFinderFrame::integrate);
    connect(
        _save_button, &QPushButton::clicked, 
        this, &PeakFinderFrame::accept);

    left_layout->addLayout(button_grid);

}

void PeakFinderFrame::setParametersUp()
{
    QList<nsx::sptrDataSet> datalist = gSession->selectedExperiment()->allData();
    for (nsx::sptrDataSet d : datalist) {
        QFileInfo fileinfo(QString::fromStdString(d->filename()));
        data->addItem(fileinfo.baseName(), QVariant::fromValue(d));
    }

    data->setCurrentIndex(0);
    _end_frame_spin->setCellValue(datalist.at(0)->nFrames());
    _end_frame_spin->setMaximum(datalist.at(0)->nFrames());
    _start_frame_spin->setMaximum(datalist.at(0)->nFrames());
    preview->getScene()->slotChangeSelectedData(datalist.at(0), 0);
    preview->getScene()->setMaxIntensity(3000);

    _kernel_combo->clear();
    nsx::ConvolverFactory convolver_factory;
    for (auto&& convolution_kernel_combo : convolver_factory.callbacks())
        _kernel_combo->addItem(QString::fromStdString(convolution_kernel_combo.first));
    _kernel_combo->setCurrentText("annular");

}

void PeakFinderFrame::updateConvolutionParameters()
{
    std::string kernelName = _kernel_combo->currentText().toStdString();
    nsx::ConvolverFactory _kernel_comboFactory;
    nsx::Convolver* kernel = _kernel_comboFactory.create(kernelName, {});
    const std::map<std::string, double>& params = kernel->parameters();
    _kernel_para_table->clear();
    _kernel_para_table->setColumnCount(2);
    int currentRow = 0;
    typedef std::map<std::string, double>::const_iterator mapIterator;
    for (mapIterator it = params.begin(); it != params.end(); ++it) {
        _kernel_para_table->insertRow(currentRow);
        QString name = QString::fromStdString(it->first);
        QTableWidgetItem* pname = new QTableWidgetItem();
        pname->setData(Qt::DisplayRole, name);
        pname->setFlags(pname->flags() ^ Qt::ItemIsEditable);
        _kernel_para_table->setItem(currentRow, 0, pname);
        QString val = QString::number(it->second);
        QTableWidgetItem* pvalue = new QTableWidgetItem(val);
        _kernel_para_table->setItem(currentRow, 1, pvalue);
        QString row = QString::number(currentRow);
        gLogger->log("Name: " + name + " value: " + val + " row: " + row);
        currentRow++;
    }
}

void PeakFinderFrame::find()
{
    nsx::sptrProgressHandler progHandler = nsx::sptrProgressHandler(new nsx::ProgressHandler);
    nsx::DataList datalist;
    for (int i = 0; i < data->count(); ++i)
        datalist.push_back(data->itemData(i, Qt::UserRole).value<nsx::sptrDataSet>());

    nsx::PeakFinder* finder = gSession->selectedExperiment()->experiment()->peakFinder();

    ProgressView progressView(nullptr);
    progressView.watch(progHandler);
    finder->setHandler(progHandler);

    finder->setMinSize(_min_size_spin->value());
    finder->setMaxSize(_max_size_spin->value());
    finder->setMaxFrames(_max_width_spin->value());
    finder->setFramesBegin(_start_frame_spin->value());
    finder->setFramesEnd(_end_frame_spin->value());
    finder->setThreshold(_threshold_spin->value());

    std::string convolverType = _kernel_combo->currentText().toStdString();
    nsx::ConvolverFactory factory;
    nsx::Convolver* convolver = factory.create(convolverType, {});
    convolver->setParameters(convolutionParameters());
    finder->setConvolver(std::unique_ptr<nsx::Convolver>(convolver));

    try {
        finder->find(datalist);

    } catch (std::exception& e) {
        return;
    }

}

void PeakFinderFrame::integrate()
{
    nsx::sptrProgressHandler progHandler = nsx::sptrProgressHandler(new nsx::ProgressHandler);
    nsx::DataList datalist;
    for (int i = 0; i < data->count(); ++i)
        datalist.push_back(data->itemData(i, Qt::UserRole).value<nsx::sptrDataSet>());

    nsx::PeakFinder* finder = gSession->selectedExperiment()->experiment()->peakFinder();
    nsx::PeakList* peaks_ptr = finder->currentPeaks();
    nsx::PeakList peaks = *peaks_ptr;

    if ((peaks.size() == 0))
        return;

    for (nsx::sptrDataSet d : datalist) {
        nsx::PixelSumIntegrator integrator(true, true);
        integrator.integrate(
            peaks, d, peakArea->value(), backgroundLowerLimit->value(),
            backgroundUpperLimit->value());
    }
}

std::map<std::string, double> PeakFinderFrame::convolutionParameters()
{
    std::map<std::string, double> parameters;
    for (int i = 0; i < _kernel_para_table->rowCount(); ++i) {
        std::string pname = _kernel_para_table->item(i, 0)->text().toStdString();
        double pvalue = _kernel_para_table->item(i, 1)->text().toDouble();
        parameters.insert(std::make_pair(pname, pvalue));
    }
    return parameters;
}

void PeakFinderFrame::doActions(QAbstractButton* button)
{
    auto buttonRole = buttons->standardButton(button);
    switch (buttonRole) {
        case QDialogButtonBox::StandardButton::Cancel: close(); break;
        case QDialogButtonBox::StandardButton::Ok: accept(); break;
        default: {
            return;
        }
    }
}

void PeakFinderFrame::accept()
{
    nsx::PeakFinder* finder = gSession->selectedExperiment()->experiment()->peakFinder();

    if (!finder->currentPeaks()->empty()){
        gLogger->log("@accept");
        std::unique_ptr<ListNameDialog> dlg(new ListNameDialog());
        dlg->exec();
        if (!dlg->listName().isEmpty()){
            gSession->selectedExperiment()->experiment()->acceptFoundPeaks(dlg->listName().toStdString());
            gSession->selectedExperiment()->generatePeakModel(dlg->listName());
        }

    }
}

void PeakFinderFrame::refreshPreview()
{
    nsx::sptrDataSet dataset = data->currentData().value<nsx::sptrDataSet>();
    int selected = frame->value();
    int nrows = dataset->nRows();
    int ncols = dataset->nCols();
    std::string convolvertype = _kernel_combo->currentText().toStdString();
    std::map<std::string, double> convolverParams = convolutionParameters();
    Eigen::MatrixXd convolvedFrame =
        nsx::convolvedFrame(dataset->reader()->data(selected), convolvertype, convolverParams);
    if (applyThreshold->isChecked()) {
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
    if (!pixmap)
        pixmap = preview->scene()->addPixmap(QPixmap::fromImage(image));
    else
        pixmap->setPixmap(QPixmap::fromImage(image));
    preview->fitInView(preview->scene()->sceneRect());
}
