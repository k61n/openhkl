
#include "nsxgui/gui/frames/peakfinder.h"
#include "nsxgui/gui/models/experimentmodel.h"
#include "nsxgui/gui/models/session.h"
#include "nsxgui/gui/models/peakstable.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QSpacerItem>
#include <QItemDelegate>
#include <QFileInfo>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <core/ConvolverFactory.h>
#include <core/DataSet.h>
#include <core/Peak3D.h>
#include <core/PeakFinder.h>
#include <core/PixelSumIntegrator.h>
#include "apps/models/MetaTypes.h"
#include "apps/views/ProgressView.h"

class ItemDelegate : public QItemDelegate {
public:
  virtual QWidget *createEditor(QWidget *parent,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index) const override;
};

QWidget* ItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                                    const QModelIndex& index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    QLineEdit *lineEdit = new QLineEdit(parent);

    // Set validator
    QDoubleValidator *validator = new QDoubleValidator(lineEdit);
    lineEdit->setValidator(validator);

    return lineEdit;
}

//  ***********************************************************************************************

FoundPeaks::FoundPeaks(nsx::PeakList peaks)
    : QcrWidget{"foundPeaksTab"}
{
    tableModel =
            new PeaksTableModel("foundPeaksTable",
                                gSession->selectedExperiment()->experiment(),
                                peaks);
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
    auto& foundPeaks = tableModel->peaks();
    gLogger->log("selectedPeaks");
    nsx::PeakList peaks;

    if (!foundPeaks.empty()) {
        peaks.reserve(foundPeaks.size());
        for (auto peak : foundPeaks) {
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

PeakFinder::PeakFinder()
    : QcrFrame{"peakFinder"}
{
    setAttribute(Qt::WA_DeleteOnClose);
    if (gSession->selectedExperimentNum() >= 0){
        if (gSession->selectedExperiment()->data()->allData().size() != 0){
            //Layout
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
            threshold = new QcrSpinBox("adhoc_threshold", new QcrCell<int>(2), 3);
            mergingScale = new QcrDoubleSpinBox("adhoc_mergingScale", new QcrCell<double>(1.000), 5, 3);
            minSize = new QcrSpinBox("adhoc_minSize", new QcrCell<int>(30), 5);
            maxSize = new QcrSpinBox("adhoc_maxSize", new QcrCell<int>(10000), 5);
            maxWidth = new QcrSpinBox("adhoc_maxWidth", new QcrCell<int>(10), 5);
            convolutionKernel = new QComboBox;
            convolutionParams = new QTableWidget(this);
            blobGrid->addWidget(threshold, 0, 1, 1, 1);
            blobGrid->addWidget(mergingScale, 1, 1, 1, 1);
            blobGrid->addWidget(minSize, 2, 1, 1, 1);
            blobGrid->addWidget(maxSize, 3, 1, 1, 1);
            blobGrid->addWidget(maxWidth, 4, 1, 1, 1);
            blobGrid->addWidget(convolutionKernel, 5, 1, 1, 1);
            blobGrid->addWidget(convolutionParams, 6, 1, 1, 1);
            leftTabLayout->addWidget(blobParams);
            QGroupBox* previewBox = new QGroupBox("Preview");
            QGridLayout* previewGrid = new QGridLayout(previewBox);
            previewGrid->addWidget(new QLabel("data"), 0, 0, 1, 1);
            previewGrid->addWidget(new QLabel("frame"), 1, 0, 1, 1);
            applyThreshold = new QcrCheckBox("adhoc_applyThreshold", "apply threshold to preview",
                                             new QcrCell<bool>(false));
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
            preview = new QGraphicsView(this);
            tabLayout->addWidget(preview);
            tab->addTab(settings, "Settings");
            whole->addWidget(tab);
            buttons = new QDialogButtonBox(QDialogButtonBox::Ok|
                                                             QDialogButtonBox::Cancel|
                                                             QDialogButtonBox::Apply,
                                                             Qt::Horizontal, this);
            connect(buttons, &QDialogButtonBox::clicked, this,
                    &PeakFinder::doActions);
            whole->addWidget(buttons);

            //else

            //tab->tabBar()->tabButton(0, QTabBar::RightSide)->hide();

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

            convolutionKernel->clear();
            nsx::ConvolverFactory convolver_factory;
            for (auto &&convolution_kernel : convolver_factory.callbacks()) {
                convolutionKernel->addItem(
                            QString::fromStdString(convolution_kernel.first));
            }
            convolutionKernel->setCurrentText("annular");

            QGraphicsScene *scene = new QGraphicsScene();
            preview->setScene(scene);
            // flip the image vertically to conform with DetectorScene
            preview->scale(1, -1);
            updateConvolutionParameters();

            show();
        } else
            breakUp();
    } else
        breakUp();
}

void PeakFinder::breakUp()
{
    gLogger->log("## Peak finding broken due to no selected Experiment or no data");
    close();
}

void PeakFinder::updateConvolutionParameters()
{
    std::string kernelName = convolutionKernel->currentText().toStdString();
    nsx::ConvolverFactory convolutionKernelFactory;
    nsx::Convolver* kernel = convolutionKernelFactory.create(kernelName, {});
    const std::map<std::string, double>& params = kernel->parameters();
    convolutionParams->clear();
    convolutionParams->setColumnCount(2);
    int currentRow=0;
    typedef std::map<std::string, double>::const_iterator mapIterator;
    for (mapIterator it = params.begin(); it!=params.end(); ++it) {
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
    //convolutionParams->horizontalHeader()->setStretchLastSection(true);
}

void PeakFinder::run()
{
    nsx::sptrProgressHandler progHandler = nsx::sptrProgressHandler(new nsx::ProgressHandler);
    nsx::DataList datalist;
    for (int i=0; i<data->count(); ++i)
        datalist.push_back(data->itemData(i, Qt::UserRole).value<nsx::sptrDataSet>());
    nsx::PeakFinder finder;
    ProgressView progressView(nullptr);
    progressView.watch(progHandler);
    finder.setHandler(progHandler);
    finder.setMinSize(minSize->value());
    finder.setMaxSize(maxSize->value());
    finder.setMaxFrames(maxWidth->value());
    std::string convolverType = convolutionKernel->currentText().toStdString();
    auto&& parameters = convolutionParameters();
    nsx::ConvolverFactory factory;
    nsx::Convolver* convolver = factory.create(convolverType, {});
    finder.setConvolver(std::unique_ptr<nsx::Convolver>(convolver));
    nsx::PeakList peaks;
    try {
        peaks = finder.find(datalist);
    } catch (std::exception& e) {
        return;
    }
    for (auto d : datalist) {
        nsx::PixelSumIntegrator integrator(true, true);
        integrator.integrate(peaks, d, peakArea->value(),
                             backgroundLowerLimit->value(),
                             backgroundUpperLimit->value());
    }

    //add Tab WidgetFoundPeaks
    tab->addTab(new FoundPeaks(peaks), "Peaks");
}

std::map<std::string, double> PeakFinder::convolutionParameters()
{
   std::map<std::string, double> parameters;
   for (int i=0; i<convolutionParams->rowCount(); ++i) {
       std::string pname = convolutionParams->item(i,0)->text().toStdString();
       double pvalue = convolutionParams->item(i,1)->text().toDouble();
       parameters.insert(std::make_pair(pname, pvalue));
   }
   return parameters;
}

void PeakFinder::doActions(QAbstractButton* button)
{
    auto buttonRole = buttons->standardButton(button);
    switch (buttonRole) {
    case QDialogButtonBox::StandardButton::Apply:
        run();
        break;
    case QDialogButtonBox::StandardButton::Cancel:
        close();
        break;
    case QDialogButtonBox::StandardButton::Ok:
        accept();
        break;
    default: { return; }
    }
}

void PeakFinder::accept()
{
    gLogger->log("@accept");
    for (auto i = 0; i < tab->count(); ++i) {

      auto widget_found_peaks =
          dynamic_cast<FoundPeaks *>(tab->widget(i));
      if (!widget_found_peaks) {
        continue;
      }

      auto &&found_peaks = widget_found_peaks->selectedPeaks();

      gLogger->log("peaksWidget found...");

      if (found_peaks.empty()) {
        continue;
      }

//      auto checkbox = dynamic_cast<QCheckBox *>(
//          tab->tabBar()->tabButton(i, QTabBar::LeftSide));

//      if (!checkbox->isChecked()) {
//        continue;
//      }

      gSession->selectedExperiment()->peaks()->appendPeaks(found_peaks);
    }

    close();
}
