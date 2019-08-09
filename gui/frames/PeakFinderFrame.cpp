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



PeakFinderFrame::PeakFinderFrame() : QcrFrame {"peakFinder"}, _pixmap(nullptr)
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

    _main_layout = new QHBoxLayout(this);
    _left_layout = new QVBoxLayout;
    _right_element = new QSplitter(Qt::Vertical , this);

    setSizePolicies();
    setBlobUp();
    setPreviewUp();
    setIntegrateUp();
    setExecuteUp();
    _left_layout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    setFigureUp();
    setPeakTableUp();
    _right_element->setSizePolicy(*_size_policy_right);
    setParametersUp();

    _main_layout->addLayout(_left_layout);
    _main_layout->addWidget(_right_element);

    // flip the image vertically to conform with DetectorScene
    _figure->scale(1, -1);
    updateConvolutionParameters();
    _min_size_spin->setHook([=](int i) { _min_size_spin->setCellValue(std::min(i, _max_size_spin->value())); });
    _max_size_spin->setHook([=](int i) { _max_size_spin->setCellValue(std::max(i, _min_size_spin->value())); });
    _peak_area->setHook(
        [=](double d) { _peak_area->setCellValue(std::min(d, _bkg_lower->value())); });
    _bkg_lower->setHook([=](double d) {
        d = std::max(d, _peak_area->value());
        _bkg_lower->setCellValue(std::min(d, _bkg_upper->value()));
    });
    _bkg_upper->setHook([=](double d) {
        _bkg_upper->setCellValue(std::max(d, _bkg_lower->value()));
    });
    _live_check->setHook([=](bool) { refreshPreview(); });
    _frame_spin->setHook([=](int) { refreshPreview(); });
    connect(_kernel_combo, &QComboBox::currentTextChanged, [=](QString) {
        updateConvolutionParameters();
        refreshPreview();
    });

    show();
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
}

void PeakFinderFrame::setBlobUp()
{
    QGroupBox* blob_para = new QGroupBox("Peak search parameter");
    QGridLayout* blob_grid = new QGridLayout(blob_para);

    blob_para->setSizePolicy(*_size_policy_box);

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

    blob_grid->addWidget(_threshold_spin, 0, 1, 1, 1);
    blob_grid->addWidget(_scale_spin, 1, 1, 1, 1);
    blob_grid->addWidget(_min_size_spin, 2, 1, 1, 1);
    blob_grid->addWidget(_max_size_spin, 3, 1, 1, 1);
    blob_grid->addWidget(_max_width_spin, 4, 1, 1, 1);
    blob_grid->addWidget(_kernel_combo, 5, 1, 1, 1);
    blob_grid->addWidget(_kernel_para_table, 6, 1, 1, 1);
    blob_grid->addWidget(_start_frame_spin, 7, 1, 1, 1);
    blob_grid->addWidget(_end_frame_spin, 8, 1, 1, 1);

    _left_layout->addWidget(blob_para);

}

void PeakFinderFrame::setPreviewUp()
{
    QGroupBox* preview_box = new QGroupBox("Preview");
    QGridLayout* _preview_grid = new QGridLayout(preview_box);

    preview_box->setSizePolicy(*_size_policy_box);

    QLabel* data_label = new QLabel("Data");
    data_label->setAlignment(Qt::AlignRight);
    _preview_grid->addWidget(data_label, 0, 0, 1, 1);

    QLabel* frame_label = new QLabel("Frame");
    frame_label->setAlignment(Qt::AlignRight);
    _preview_grid->addWidget(frame_label, 1, 0, 1, 1);

    _data_combo = new QComboBox;
    _frame_spin = new QcrSpinBox(
        "adhoc_frameNr", 
        new QcrCell<int>(0), 3);
    _live_check = new QcrCheckBox(
        "adhoc__live_check", 
        "Apply threshold to preview", 
        new QcrCell<bool>(false));

    _data_combo->setMaximumWidth(1000);
    _frame_spin->setMaximumWidth(1000);
    _live_check->setMaximumWidth(1000);
    
    _data_combo->setSizePolicy(*_size_policy_widgets);
    _frame_spin->setSizePolicy(*_size_policy_widgets);
    _live_check->setSizePolicy(*_size_policy_widgets);

    _preview_grid->addWidget(_data_combo, 0, 1, 1, 1);
    _preview_grid->addWidget(_frame_spin, 1, 1, 1, 1);
    _preview_grid->addWidget(_live_check, 2, 0, 1, 2);

    _left_layout->addWidget(preview_box);
}

void PeakFinderFrame::setIntegrateUp()
{
    QGroupBox* integration_para = new QGroupBox("Integration parameters");
    QGridLayout* integGrid = new QGridLayout(integration_para);

    integration_para->setSizePolicy(*_size_policy_box);

    integGrid->addWidget(new QLabel("Peak area"), 0, 0, 1, 1);
    integGrid->addWidget(new QLabel("Background lower limit"), 1, 0, 1, 1);
    integGrid->addWidget(new QLabel("Background upper limit"), 2, 0, 1, 1);

    _peak_area = new QcrDoubleSpinBox("adhoc_area", new QcrCell<double>(3.0), 5, 2);
    _bkg_lower = new QcrDoubleSpinBox("adhoc_lowLimit", new QcrCell<double>(4.0), 5, 2);
    _bkg_upper = new QcrDoubleSpinBox("adhoc_upLimit", new QcrCell<double>(4.5), 5, 2);

    integGrid->addWidget(_peak_area, 0, 1, 1, 1);
    integGrid->addWidget(_bkg_lower, 1, 1, 1, 1);
    integGrid->addWidget(_bkg_upper, 2, 1, 1, 1);

    _left_layout->addWidget(integration_para);
    
}

void PeakFinderFrame::setFigureUp()
{
    QGroupBox* figure_group = new QGroupBox("Peaks");
    QGridLayout* figure_grid = new QGridLayout(figure_group);

    figure_group->setSizePolicy(*_size_policy_right);

    _figure = new DetectorView(this);
    figure_grid->addWidget(_figure, 0,0,0,0);

    _right_element->addWidget(figure_group);
}

void PeakFinderFrame::setPeakTableUp()
{
    QGroupBox* peak_group = new QGroupBox("Peaks");
    QGridLayout* peak_grid = new QGridLayout(peak_group);

    peak_group->setSizePolicy(*_size_policy_right);

    _peak_table = new PeaksTableView(this);
    peak_grid->addWidget(_peak_table, 0,0,0,0);

    _right_element->addWidget(peak_group);
}

void PeakFinderFrame::setExecuteUp()
{
    QHBoxLayout* button_grid = new QHBoxLayout();

    _find_button = new QPushButton("Find peaks");
    _integrate_button = new QPushButton("Integrate");
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

    _left_layout->addLayout(button_grid);

}

void PeakFinderFrame::setParametersUp()
{
    QList<nsx::sptrDataSet> datalist = gSession->selectedExperiment()->allData();
    for (nsx::sptrDataSet d : datalist) {
        QFileInfo fileinfo(QString::fromStdString(d->filename()));
        _data_combo->addItem(fileinfo.baseName(), QVariant::fromValue(d));
    }

    _data_combo->setCurrentIndex(0);
    _end_frame_spin->setCellValue(datalist.at(0)->nFrames());
    _end_frame_spin->setMaximum(datalist.at(0)->nFrames());
    _start_frame_spin->setMaximum(datalist.at(0)->nFrames());
    _figure->getScene()->slotChangeSelectedData(datalist.at(0), 0);
    _figure->getScene()->setMaxIntensity(3000);

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
    for (int i = 0; i < _data_combo->count(); ++i)
        datalist.push_back(_data_combo->itemData(i, Qt::UserRole).value<nsx::sptrDataSet>());

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
        refreshPeakTable();

    } catch (std::exception& e) {
        return;
    }
        
}

void PeakFinderFrame::integrate()
{
    nsx::sptrExperiment experiment = gSession->selectedExperiment()->experiment();
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
    }
    return parameters;
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
    close();
}

void PeakFinderFrame::refreshPreview()
{
    nsx::sptrDataSet dataset = _data_combo->currentData().value<nsx::sptrDataSet>();
    int selected = _frame_spin->value();
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
        _pixmap = _figure->scene()->addPixmap(QPixmap::fromImage(image));
    else
        _pixmap->setPixmap(QPixmap::fromImage(image));
    _figure->fitInView(_figure->scene()->sceneRect());
}

void PeakFinderFrame::refreshPeakTable()
{
    std::vector<std::shared_ptr<nsx::Peak3D>>* peaks = 
        gSession->selectedExperiment()->experiment()->peakFinder()->currentPeaks();
    
    if (!(peaks->size()>0)){
        std::cout << peaks->size()<<std::endl;
        return;
    }

    _peak_collection->populate(peaks);
    _peak_collection_item = new PeakCollectionItem(_peak_collection);
    _peak_collection_model->setRoot(_peak_collection_item);
    _peak_table->setModel(_peak_collection_model);

}
