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
#include "gui/models/ExperimentModel.h"
#include "gui/models/Meta.h"
#include "gui/models/PeaksTable.h"
#include "gui/models/Session.h"
#include <QCR/engine/mixin.h>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemDelegate>
#include <QSpacerItem>
#include <QTableWidgetItem>
#include <QVBoxLayout>

class ItemDelegate : public QItemDelegate {
 public:
    virtual QWidget* createEditor(
        QWidget* parent, const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;
};

QWidget* ItemDelegate::createEditor(
    QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    QLineEdit* lineEdit = new QLineEdit(parent);

    // Set validator
    QDoubleValidator* validator = new QDoubleValidator(lineEdit);
    lineEdit->setValidator(validator);

    return lineEdit;
}

//  ***********************************************************************************************

FoundPeaks::FoundPeaks(nsx::PeakList peaks, const QString& name) : QcrWidget {name}
{
    tableModel =
        new PeaksTableModel("foundPeaksTable", gSession->selectedExperiment()->experiment(), peaks);
    QVBoxLayout* vertical = new QVBoxLayout(this);
    PeaksTableView* peaksTable = new PeaksTableView(this);
    peaksTable->setModel(tableModel);
    vertical->addWidget(peaksTable);
    keepSelectedPeaks =
        new QcrCheckBox("adhoc_keepPeaks", "keep selected peaks", new QcrCell<bool>(false));
    vertical->addWidget(keepSelectedPeaks);
}

nsx::PeakList FoundPeaks::selectedPeaks()
{
    const nsx::PeakList& foundPeaks = tableModel->peaks();
    gLogger->log("selectedPeaks");
    nsx::PeakList peaks;

    if (!foundPeaks.empty()) {
        peaks.reserve(foundPeaks.size());
        for (nsx::sptrPeak3D peak : foundPeaks) {
            if (keepSelectedPeaks->isChecked()) {
                if (peak->selected())
                    peaks.push_back(peak);
            } else
                peaks.push_back(peak);
        }
    }
    return peaks;
}

//  ***********************************************************************************************

PeakFinderFrame::PeakFinderFrame() : QcrFrame {"peakFinder"}, pixmap(nullptr)
{
    if (gSession->selectedExperimentNum() < 0) {
        gLogger->log("[ERROR] No experiment selected");
        return;
    }
    if (gSession->selectedExperiment()->data()->allData().size() == 0) {
        gLogger->log("[ERROR] No data loaded for selected experiment");
        return;
    }
    // Layout
    setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout* whole = new QVBoxLayout(this);

    tab = new QcrTabWidget("adhoc_peakFinderSettings");
    QcrWidget* settings = new QcrWidget("peakFinderTab");
    QHBoxLayout* tabLayout = new QHBoxLayout(settings);
    QVBoxLayout* leftTabLayout = new QVBoxLayout;
    QGroupBox* blobParams = new QGroupBox("Blob parameter");
    QGridLayout* blobGrid = new QGridLayout(blobParams);
    blobGrid->addWidget(new QLabel("threshold"), 0, 0, 1, 1);
    blobGrid->addWidget(new QLabel("merging scale"), 1, 0, 1, 1);
    blobGrid->addWidget(new QLabel("minimum size"), 2, 0, 1, 1);
    blobGrid->addWidget(new QLabel("maximum size"), 3, 0, 1, 1);
    blobGrid->addWidget(new QLabel("maximum width"), 4, 0, 1, 1);
    blobGrid->addWidget(new QLabel("convolution kernel"), 5, 0, 1, 1);
    blobGrid->addWidget(new QLabel("convolution parameters"), 6, 0, 1, 1);
    blobGrid->addWidget(new QLabel("begin finding blobs in frame"), 7, 0, 1, 1);
    blobGrid->addWidget(new QLabel("end finding blobs in frame"), 8, 0, 1, 1);
    threshold = new QcrSpinBox("adhoc_threshold", new QcrCell<int>(2), 3);
    mergingScale = new QcrDoubleSpinBox("adhoc_mergingScale", new QcrCell<double>(1.000), 5, 3);
    minSize = new QcrSpinBox("adhoc_minSize", new QcrCell<int>(30), 5);
    maxSize = new QcrSpinBox("adhoc_maxSize", new QcrCell<int>(10000), 5);
    maxWidth = new QcrSpinBox("adhoc_maxWidth", new QcrCell<int>(10), 5);
    convolutionKernel = new QComboBox;
    convolutionParams = new QTableWidget(this);
    framesBegin = new QcrSpinBox("adhoc_beginFrame", new QcrCell<int>(0), 3);
    framesEnd = new QcrSpinBox("adhoc_endFrame", new QcrCell<int>(0), 3);
    blobGrid->addWidget(threshold, 0, 1, 1, 1);
    blobGrid->addWidget(mergingScale, 1, 1, 1, 1);
    blobGrid->addWidget(minSize, 2, 1, 1, 1);
    blobGrid->addWidget(maxSize, 3, 1, 1, 1);
    blobGrid->addWidget(maxWidth, 4, 1, 1, 1);
    blobGrid->addWidget(convolutionKernel, 5, 1, 1, 1);
    blobGrid->addWidget(convolutionParams, 6, 1, 1, 1);
    blobGrid->addWidget(framesBegin, 7, 1, 1, 1);
    blobGrid->addWidget(framesEnd, 8, 1, 1, 1);
    leftTabLayout->addWidget(blobParams);
    QGroupBox* previewBox = new QGroupBox("Preview");
    QGridLayout* previewGrid = new QGridLayout(previewBox);
    previewGrid->addWidget(new QLabel("data"), 0, 0, 1, 1);
    previewGrid->addWidget(new QLabel("frame"), 1, 0, 1, 1);
    applyThreshold = new QcrCheckBox(
        "adhoc_applyThreshold", "apply threshold to preview", new QcrCell<bool>(false));
    previewGrid->addWidget(applyThreshold, 2, 0, 1, 1);
    data = new QComboBox;
    frame = new QcrSpinBox("adhoc_frameNr", new QcrCell<int>(0), 3);
    previewGrid->addWidget(data, 0, 1, 1, 1);
    previewGrid->addWidget(frame, 1, 1, 1, 1);
    leftTabLayout->addWidget(previewBox);
    QGroupBox* integrationParams = new QGroupBox("Integration parameters");
    QGridLayout* integGrid = new QGridLayout(integrationParams);
    integGrid->addWidget(new QLabel("peak area"), 0, 0, 1, 1);
    integGrid->addWidget(new QLabel("backgroung lower limit"), 1, 0, 1, 1);
    integGrid->addWidget(new QLabel("background upper limit"), 2, 0, 1, 1);
    peakArea = new QcrDoubleSpinBox("adhoc_area", new QcrCell<double>(3.0), 5, 2);
    backgroundLowerLimit = new QcrDoubleSpinBox("adhoc_lowLimit", new QcrCell<double>(4.0), 5, 2);
    backgroundUpperLimit = new QcrDoubleSpinBox("adhoc_upLimit", new QcrCell<double>(4.5), 5, 2);
    integGrid->addWidget(peakArea, 0, 1, 1, 1);
    integGrid->addWidget(backgroundLowerLimit, 1, 1, 1, 1);
    integGrid->addWidget(backgroundUpperLimit, 2, 1, 1, 1);
    leftTabLayout->addWidget(integrationParams);
    leftTabLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    tabLayout->addLayout(leftTabLayout);
    preview = new DetectorView(this);
    tabLayout->addWidget(preview);
    tab->addTab(settings, "Settings");
    whole->addWidget(tab);
    buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply, Qt::Horizontal,
        this);
    connect(buttons, &QDialogButtonBox::clicked, this, &PeakFinderFrame::doActions);
    whole->addWidget(buttons);

    // else

    // tab->tabBar()->tabButton(0, QTabBar::RightSide)->hide();

    //            ItemDelegate *convolution_parameters_delegate =
    //                    new ItemDelegate();
    //            convolutionParams->setItemDelegateForColumn(
    //                        1, convolution_parameters_delegate);

    nsx::DataList datalist = gSession->selectedExperiment()->data()->allDataVector();
    for (nsx::sptrDataSet d : datalist) {
        QFileInfo fileinfo(QString::fromStdString(d->filename()));
        data->addItem(fileinfo.baseName(), QVariant::fromValue(d));
    }

    data->setCurrentIndex(0);
    framesEnd->setCellValue(datalist.at(0)->nFrames());
    framesEnd->setMaximum(datalist.at(0)->nFrames());
    framesBegin->setMaximum(datalist.at(0)->nFrames());
    preview->getScene()->slotChangeSelectedData(datalist.at(0), 0);
    preview->getScene()->setMaxIntensity(3000);

    convolutionKernel->clear();
    nsx::ConvolverFactory convolver_factory;
    for (auto&& convolution_kernel : convolver_factory.callbacks())
        convolutionKernel->addItem(QString::fromStdString(convolution_kernel.first));
    convolutionKernel->setCurrentText("annular");

    // flip the image vertically to conform with DetectorScene
    preview->scale(1, -1);
    updateConvolutionParameters();
    minSize->setHook([=](int i) { minSize->setCellValue(std::min(i, maxSize->value())); });
    maxSize->setHook([=](int i) { maxSize->setCellValue(std::max(i, minSize->value())); });
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
    connect(convolutionKernel, &QComboBox::currentTextChanged, [=](QString) {
        updateConvolutionParameters();
        refreshPreview();
    });

    show();
}

void PeakFinderFrame::updateConvolutionParameters()
{
    std::string kernelName = convolutionKernel->currentText().toStdString();
    nsx::ConvolverFactory convolutionKernelFactory;
    nsx::Convolver* kernel = convolutionKernelFactory.create(kernelName, {});
    const std::map<std::string, double>& params = kernel->parameters();
    convolutionParams->clear();
    convolutionParams->setColumnCount(2);
    int currentRow = 0;
    typedef std::map<std::string, double>::const_iterator mapIterator;
    for (mapIterator it = params.begin(); it != params.end(); ++it) {
        convolutionParams->insertRow(currentRow);
        QString name = QString::fromStdString(it->first);
        QTableWidgetItem* pname = new QTableWidgetItem();
        pname->setData(Qt::DisplayRole, name);
        pname->setFlags(pname->flags() ^ Qt::ItemIsEditable);
        convolutionParams->setItem(currentRow, 0, pname);
        QString val = QString::number(it->second);
        QTableWidgetItem* pvalue = new QTableWidgetItem(val);
        convolutionParams->setItem(currentRow, 1, pvalue);
        QString row = QString::number(currentRow);
        gLogger->log("Name: " + name + " value: " + val + " row: " + row);
        currentRow++;
    }
    // convolutionParams->horizontalHeader()->setStretchLastSection(true);
}

void PeakFinderFrame::run()
{
    nsx::sptrProgressHandler progHandler = nsx::sptrProgressHandler(new nsx::ProgressHandler);
    nsx::DataList datalist;
    for (int i = 0; i < data->count(); ++i)
        datalist.push_back(data->itemData(i, Qt::UserRole).value<nsx::sptrDataSet>());
    nsx::PeakFinder finder;
    ProgressView progressView(nullptr);
    progressView.watch(progHandler);
    finder.setHandler(progHandler);
    finder.setMinSize(minSize->value());
    finder.setMaxSize(maxSize->value());
    finder.setMaxFrames(maxWidth->value());
    finder.setFramesBegin(framesBegin->value());
    finder.setFramesEnd(framesEnd->value());
    finder.setThreshold(threshold->value());
    std::string convolverType = convolutionKernel->currentText().toStdString();
    nsx::ConvolverFactory factory;
    nsx::Convolver* convolver = factory.create(convolverType, {});
    convolver->setParameters(convolutionParameters());
    finder.setConvolver(std::unique_ptr<nsx::Convolver>(convolver));
    nsx::PeakList peaks;
    try {
        peaks = finder.find(datalist);
    } catch (std::exception& e) {
        return;
    }
    for (nsx::sptrDataSet d : datalist) {
        nsx::PixelSumIntegrator integrator(true, true);
        integrator.integrate(
            peaks, d, peakArea->value(), backgroundLowerLimit->value(),
            backgroundUpperLimit->value());
    }

    // add Tab WidgetFoundPeaks
    tab->addTab(new FoundPeaks(peaks, "adhoc_findNum" + QString::number(tab->count())), "Peaks");
}

std::map<std::string, double> PeakFinderFrame::convolutionParameters()
{
    std::map<std::string, double> parameters;
    for (int i = 0; i < convolutionParams->rowCount(); ++i) {
        std::string pname = convolutionParams->item(i, 0)->text().toStdString();
        double pvalue = convolutionParams->item(i, 1)->text().toDouble();
        parameters.insert(std::make_pair(pname, pvalue));
    }
    return parameters;
}

void PeakFinderFrame::doActions(QAbstractButton* button)
{
    auto buttonRole = buttons->standardButton(button);
    switch (buttonRole) {
        case QDialogButtonBox::StandardButton::Apply: run(); break;
        case QDialogButtonBox::StandardButton::Cancel: close(); break;
        case QDialogButtonBox::StandardButton::Ok: accept(); break;
        default: {
            return;
        }
    }
}

void PeakFinderFrame::accept()
{
    gLogger->log("@accept");
    for (int i = 0; i < tab->count(); ++i) {
        FoundPeaks* widget_found_peaks = dynamic_cast<FoundPeaks*>(tab->widget(i));
        if (!widget_found_peaks)
            continue;

        nsx::PeakList found_peaks = widget_found_peaks->selectedPeaks();

        if (found_peaks.empty())
            continue;

        //      auto checkbox = dynamic_cast<QCheckBox *>(
        //          tab->tabBar()->tabButton(i, QTabBar::LeftSide));

        //      if (!checkbox->isChecked()) {
        //        continue;
        //      }

        // listname dialog
        std::unique_ptr<ListNameDialog> dlg(new ListNameDialog(found_peaks));
        if (!dlg->exec())
            continue;
        QString peaklistname = dlg->listName();
        int numValid = 0;
        int numPeaks = found_peaks.size();
        for (nsx::sptrPeak3D peak : found_peaks) {
            if (peak->enabled())
                numValid++;
        }
        int numNotValid = numPeaks - numValid;
        peaklistname += " (" + QString::number(numPeaks) + " | valid: " + QString::number(numValid);
        peaklistname += " | not valid: " + QString::number(numNotValid) + ")";
        gSession->selectedExperiment()->peaks()->addPeakListsModel(peaklistname, found_peaks);
    }

    close();
}

void PeakFinderFrame::refreshPreview()
{
    nsx::sptrDataSet dataset = data->currentData().value<nsx::sptrDataSet>();
    int selected = frame->value();
    int nrows = dataset->nRows();
    int ncols = dataset->nCols();
    std::string convolvertype = convolutionKernel->currentText().toStdString();
    std::map<std::string, double> convolverParams = convolutionParameters();
    Eigen::MatrixXd convolvedFrame =
        nsx::convolvedFrame(dataset->reader()->data(selected), convolvertype, convolverParams);
    if (applyThreshold->isChecked()) {
        double thresholdVal = threshold->value();
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
