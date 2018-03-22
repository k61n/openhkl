#include <QFileInfo>
#include <QImage>
#include <QLineEdit>
#include <QList>
#include <QListWidget>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QItemDelegate>
#include <QSortFilterProxyModel>
#include <QSpinBox>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTreeView>

#include <nsxlib/ConvolutionKernel.h>
#include <nsxlib/Convolver.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/DataTypes.h>
#include <nsxlib/KernelFactory.h>
#include <nsxlib/ImagingTypes.h>
#include <nsxlib/Logger.h>
#include <nsxlib/MathematicsTypes.h>
#include <nsxlib/PeakFinder.h>
#include <nsxlib/Threshold.h>
#include <nsxlib/ThresholdFactory.h>

#include "ColorMap.h"
#include "DialogPeakFind.h"

#include "ui_DialogPeakFind.h"

class DoubleDelegate : public QItemDelegate
{
public:
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem & option,
                      const QModelIndex & index) const
    {
        QLineEdit* lineEdit = new QLineEdit(parent);
        // Set validator
        QDoubleValidator *validator = new QDoubleValidator(lineEdit);
        lineEdit->setValidator(validator);
        return lineEdit;
    }
};

DialogPeakFind::DialogPeakFind(const nsx::DataList& data,
                               nsx::sptrPeakFinder peakFinder,
                               QWidget *parent):
    QDialog(parent),
    ui(new Ui::DialogPeakFind),
    _data(data),
    _pxmapPreview(nullptr),
    _colormap(new ColorMap)
{
    ui->setupUi(this);

    // disable resizing
    this->setFixedSize(this->size());

    for (auto d : _data) {
        QFileInfo fileinfo(QString::fromStdString(d->filename()));
        ui->dataList->addItem(fileinfo.baseName());
    }

    ui->dataList->setCurrentRow(0);

    ui->frameSlider->setMinimum(0);
    ui->frameSlider->setMaximum(_data[0]->nFrames());

    ui->frameIndex->setMinimum(0);
    ui->frameIndex->setMaximum(_data[0]->nFrames());

    _peakFinder = peakFinder;
    _scene = new QGraphicsScene(this);
    ui->preview->setScene(_scene);

    // flip image vertically to conform with DetectorScene
    ui->preview->scale(1, -1);

    // Set the delegate that will force a numeric input for the threshold parameters value column
    ui->thresholdParameters->setColumnCount(2);
    ui->thresholdParameters->horizontalHeader()->hide();
    ui->thresholdParameters->verticalHeader()->hide();
    DoubleDelegate* threshold_table_delegate = new DoubleDelegate();
    ui->thresholdParameters->setItemDelegateForColumn(1,threshold_table_delegate);

    // Set the threshold combo box and table
    ui->thresholdComboBox->clear();
    nsx::ThresholdFactory threshold_factory;
    for (auto& k : threshold_factory.callbacks()) {
        ui->thresholdComboBox->addItem(QString::fromStdString(k.first));
    }
    ui->thresholdComboBox->setCurrentText(_peakFinder->threshold()->name());

    changeThreshold(ui->thresholdComboBox->currentText());

    // Set the filter combo box and table
    ui->kernelParameters->setColumnCount(2);
    ui->kernelParameters->horizontalHeader()->hide();
    ui->kernelParameters->verticalHeader()->hide();

    // Set the delegate that will force a numeric input for the convolution kernel parameters value column
    DoubleDelegate* kernel_table_delegate = new DoubleDelegate();
    ui->kernelParameters->setItemDelegateForColumn(1,kernel_table_delegate);

    ui->kernelComboBox->clear();
    nsx::KernelFactory kernel_factory;
    for (auto& k : kernel_factory.callbacks()) {
        ui->kernelComboBox->addItem(QString::fromStdString(k.first));
    }
    ui->kernelComboBox->setCurrentText(_peakFinder->kernel()->name());

    changeKernel(ui->kernelComboBox->currentText());

    connect(ui->thresholdComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(changeThreshold(QString)));
    connect(ui->thresholdParameters,SIGNAL(cellChanged(int,int)),this,SLOT(changeThresholdParameters(int,int)));
    connect(ui->applyThreshold,SIGNAL(stateChanged(int)),this,SLOT(clipPreview(int)));

    connect(ui->kernelComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(changeKernel(QString)));
    connect(ui->kernelParameters,SIGNAL(cellChanged(int,int)),this,SLOT(changeKernelParameters(int,int)));

    connect(ui->minSize,SIGNAL(valueChanged(int)),this,SLOT(changeMinSize(int)));
    connect(ui->maxSize,SIGNAL(valueChanged(int)),this,SLOT(changeMaxSize(int)));
    connect(ui->maxFrames,SIGNAL(valueChanged(int)),this,SLOT(changeMaxFrames(int)));

    connect(ui->searchConfidence,SIGNAL(valueChanged(double)),this,SLOT(changeSearchConfidenceValue(double)));
    connect(ui->integrationConfidence,SIGNAL(valueChanged(double)),this,SLOT(changeIntegrationConfidenceValue(double)));

    connect(ui->dataList,SIGNAL(currentRowChanged(int)),this,SLOT(changeSelectedData(int)));
    connect(ui->frameSlider,SIGNAL(valueChanged(int)),this,SLOT(changeSelectedFrame(int)));
    connect(ui->frameIndex,SIGNAL(valueChanged(int)),this,SLOT(changeSelectedFrame(int)));
}

DialogPeakFind::~DialogPeakFind()
{
    delete ui;
}

void DialogPeakFind::changeSelectedData(int selected_data)
{
    auto data = _data[selected_data];

    ui->frameIndex->setMinimum(0);
    ui->frameIndex->setMaximum(data->nFrames());
    ui->frameIndex->setValue(0);

    ui->frameSlider->setMinimum(0);
    ui->frameSlider->setMaximum(data->nFrames());
    ui->frameSlider->setValue(0);

    updatePreview();
}

void DialogPeakFind::setColorMap(const std::string &name)
{
    _colormap = std::unique_ptr<ColorMap>(new ColorMap(name));
}

int DialogPeakFind::exec()
{
    return QDialog::exec();
}

void DialogPeakFind::changeSelectedFrame(int selected_frame)
{
    ui->frameIndex->setValue(selected_frame);
    ui->frameSlider->setValue(selected_frame);

    updatePreview();
}

void DialogPeakFind::updatePreview()
{
    int selected_data = ui->dataList->currentRow();

    auto data = _data[selected_data];

    int selected_frame = ui->frameSlider->value();

    auto frame = data->frame(selected_frame);

    int max_data = frame.maxCoeff();

    int nrows = data->nRows();

    int ncols = data->nCols();

    auto kernel = _peakFinder->kernel();

    Eigen::MatrixXi convolved_frame = data->convolvedFrame(selected_frame,kernel);

    // apply threshold in preview
    if (ui->applyThreshold->isChecked()) {
        nsx::ThresholdFactory threshold_factory;
        std::string threshold_type = ui->thresholdComboBox->currentText().toStdString();
        auto parameters = getThresholdParameters();
        auto threshold = threshold_factory.create(threshold_type,parameters);
        double threshold_value = threshold->value(data,selected_frame);

        for (int i = 0; i < nrows; ++i) {
            for (int j = 0; j < ncols; ++j) {
                convolved_frame(i, j) = convolved_frame(i, j) < threshold_value ? 0 : max_data-1;
            }
        }
    }

    // clamp the result for the preview window
    double minVal = convolved_frame.minCoeff();
    double maxVal = convolved_frame.maxCoeff();
    convolved_frame.array() -= minVal;
    convolved_frame.array() *= static_cast<double>(max_data)/(maxVal-minVal);
    QRect rect(0, 0, ncols, nrows);
    QImage image = _colormap->matToImage(convolved_frame.cast<double>(), rect, convolved_frame.maxCoeff());

    if (!_pxmapPreview) {
        _pxmapPreview = _scene->addPixmap(QPixmap::fromImage(image));
    } else {
        _pxmapPreview->setPixmap(QPixmap::fromImage(image));
    }
}

void DialogPeakFind::clipPreview(int state) {
    updatePreview();
}

void DialogPeakFind::changeSearchConfidenceValue(double value)
{
    _peakFinder->setSearchConfidence(value);
}

void DialogPeakFind::changeIntegrationConfidenceValue(double value)
{
    _peakFinder->setIntegrationConfidence(value);
}

void DialogPeakFind::changeMinSize(int size)
{
    _peakFinder->setMinSize(size);
}

void DialogPeakFind::changeMaxSize(int size)
{
    _peakFinder->setMaxSize(size);
}

void DialogPeakFind::changeMaxFrames(int size)
{
    _peakFinder->setMaxFrames(size);
}

void DialogPeakFind::changeThreshold(QString threshold)
{
    _peakFinder->setThreshold(threshold.toStdString(),{});

    // update dialog with list of parameters
    buildThresholdParametersList();

    updatePreview();
}

void DialogPeakFind::changeThresholdParameters(int row, int col)
{
    Q_UNUSED(row)
    Q_UNUSED(col)

    // Get the current threshold type
    std::string threshold_type = ui->thresholdComboBox->currentText().toStdString();

    // Get the updated parameters
    auto parameters = getThresholdParameters();

    // propagate changes to peak finder
    _peakFinder->setThreshold(threshold_type,parameters);

    // update the preview
    updatePreview();
}

std::map<std::string,double> DialogPeakFind::getThresholdParameters() const
{
    std::map<std::string,double> parameters;

    for (int i = 0; i < ui->thresholdParameters->rowCount(); ++i) {
        std::string pname = ui->thresholdParameters->item(i,0)->text().toStdString();
        double pvalue = ui->thresholdParameters->item(i,1)->text().toDouble();
        parameters.insert(std::make_pair(pname,pvalue));
    }

    return parameters;
}

void DialogPeakFind::buildThresholdParametersList()
{
    // get the selected threshold
    auto threshold = _peakFinder->threshold();

    // get parameters
    const std::map<std::string, double>& parameters = threshold->parameters();

    disconnect(ui->thresholdParameters,SIGNAL(cellChanged(int,int)),this,SLOT(changeThresholdParameters(int,int)));

    ui->thresholdParameters->setRowCount(0);

    // iterate through parameters to build the tree
    for (auto p : parameters) {
        int current_row = ui->thresholdParameters->rowCount();

        ui->thresholdParameters->insertRow(current_row);

        QTableWidgetItem* pname = new QTableWidgetItem();
        pname->setText(QString::fromStdString(p.first));
        ui->thresholdParameters->setItem(current_row,0,pname);

        pname->setFlags(pname->flags() ^ Qt::ItemIsEditable);

        QTableWidgetItem* pvalue = new QTableWidgetItem();
        pvalue->setText(QString::number(p.second));
        ui->thresholdParameters->setItem(current_row,1,pvalue);
    }

    connect(ui->thresholdParameters,SIGNAL(cellChanged(int,int)),this,SLOT(changeThresholdParameters(int,int)));
}

void DialogPeakFind::changeKernel(QString kernel)
{
    // Set the new kernel with default parameters to the peak finder
    _peakFinder->setKernel(kernel.toStdString(),{});

    // Update dialog with the selected kernel parameters
    buildKernelParametersList();

    // Update the preview
    updatePreview();
}

void DialogPeakFind::changeKernelParameters(int row, int col)
{
    Q_UNUSED(row)
    Q_UNUSED(col)

    // Get the current kernel type
    std::string kernel_type = ui->kernelComboBox->currentText().toStdString();

    // Get the corresponding parameters
    auto parameters = getKernelParameters();

    // Propagate changes to peak finder
    _peakFinder->setKernel(kernel_type,parameters);

    // Update the preview
    updatePreview();
}

std::map<std::string,double> DialogPeakFind::getKernelParameters() const
{
    std::map<std::string,double> parameters;

    for (int i = 0; i < ui->kernelParameters->rowCount(); ++i) {
        std::string pname = ui->kernelParameters->item(i,0)->text().toStdString();
        double pvalue = ui->kernelParameters->item(i,1)->text().toDouble();
        parameters.insert(std::make_pair(pname,pvalue));
    }

    return parameters;
}

void DialogPeakFind::buildKernelParametersList()
{
    // Get the selected kernel
    auto kernel = _peakFinder->kernel();

    // Get its corresponding parameters
    const std::map<std::string, double>& parameters = kernel->parameters();

    disconnect(ui->kernelParameters,SIGNAL(cellChanged(int,int)),this,SLOT(changeKernelParameters(int,int)));

    ui->kernelParameters->setRowCount(0);

    // Iterate through parameters to build the tree
    for (auto p : parameters) {
        int current_row = ui->kernelParameters->rowCount();

        ui->kernelParameters->insertRow(current_row);

        int currow = ui->kernelParameters->rowCount();
        int curcol = ui->kernelParameters->columnCount();

        QTableWidgetItem* pname = new QTableWidgetItem();
        pname->setText(QString::fromStdString(p.first));
        ui->kernelParameters->setItem(current_row,0,pname);

        pname->setFlags(pname->flags() ^ Qt::ItemIsEditable);

        QTableWidgetItem* pvalue = new QTableWidgetItem();
        pvalue->setText(QString::number(p.second));
        ui->kernelParameters->setItem(current_row,1,pvalue);
    }

    connect(ui->kernelParameters,SIGNAL(cellChanged(int,int)),this,SLOT(changeKernelParameters(int,int)));
}
