#include <QFileInfo>
#include <QImage>
#include <QList>
#include <QListWidget>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
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

#include "ColorMap.h"
#include "DialogConvolve.h"

#include "ui_ConvolveDialog.h"

DialogConvolve::DialogConvolve(const nsx::DataList& data,
                               nsx::sptrPeakFinder peakFinder,
                               QWidget *parent):
    QDialog(parent),
    ui(new Ui::DialogConvolve),
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

    ui->frameIndex->setMinimum(0);
    ui->frameIndex->setMaximum(_data[0]->nFrames());

    _peakFinder = peakFinder;
    _scene = new QGraphicsScene(this);
    ui->preview->setScene(_scene);

    // flip image vertically to conform with DetectorScene
    ui->preview->scale(1, -1);

    ui->filterComboBox->clear();
    nsx::KernelFactory* kernelFactory=nsx::KernelFactory::Instance();

    for (const auto& k : kernelFactory->list())
        ui->filterComboBox->addItem(QString::fromStdString(k));

    ui->filterComboBox->addItem("none");

    QSortFilterProxyModel* proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(ui->filterComboBox->model());
    ui->filterComboBox->model()->setParent(proxy);
    ui->filterComboBox->setModel(proxy);
    ui->filterComboBox->model()->sort(0);

    changeConvolutionFilter(0);

    connect(ui->thresholdComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(changeThresholdType(int)));

    connect(ui->filterComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(changeConvolutionFilter(int)));

    connect(ui->minCompBox,SIGNAL(valueChanged(int)),this,SLOT(changeBlobMinSize(int)));

    connect(ui->maxCompBox,SIGNAL(valueChanged(int)),this,SLOT(changeBlobMaxSize(int)));

    connect(ui->thresholdSpinBox,SIGNAL(valueChanged(double)),this,SLOT(changeThresholdValue(double)));

    connect(ui->blobConfidenceSpinBox,SIGNAL(valueChanged(double)),this,SLOT(changeBlobConfidenceValue(double)));

    connect(ui->integrationConfidenceSpinBox,SIGNAL(valueChanged(double)),this,SLOT(changeIntegrationConfidenceValue(double)));

    connect(ui->dataList,SIGNAL(currentRowChanged(int)),this,SLOT(changeSelectedData(int)));

    connect(ui->frameIndex,SIGNAL(valueChanged(int)),this,SLOT(changeSelectedFrame(int)));
}

DialogConvolve::~DialogConvolve()
{
    delete ui;
    // this should be handled by Qt. check with valgrind?
    // delete _peakFindModel;
}

void DialogConvolve::changeSelectedData(int selected_data)
{
    auto data = _data[selected_data];

    ui->frameIndex->setMaximum(data->nFrames());

    ui->frameIndex->setValue(0);

    updatePreview();
}

void DialogConvolve::buildConvolutionParametersList()
{
    // reset tree
    QTreeView* treeView = ui->treeView;
    treeView->reset();
    treeView->header()->hide();

    // no peakfinder set?!
    if (!_peakFinder) {
        return;
    }

    // get the selected kernel (if any)
    auto kernel = _peakFinder->kernel();

    QStandardItemModel* model = new QStandardItemModel(this);

    // no kernel selected: do nothing
    if (kernel) {
        // get parameters
        std::map<std::string, double> parameters = kernel->parameters();

        // iterate through parameters to build the tree
        for (auto it : parameters) {
            QStandardItem* name = new QStandardItem();
            name->setText(it.first.c_str());
            name->setEditable(false);

            QStandardItem* value = new QStandardItem();

            name->setText(it.first.c_str());
            value->setData(QVariant(it.second), Qt::EditRole|Qt::DisplayRole);
            value->setData(QVariant(it.first.c_str()), Qt::UserRole);

            model->appendRow(QList<QStandardItem*>() << name << value);
        }
    }

    treeView->setModel(model);

    connect(model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(changeConvolutionParameters(QStandardItem*)));
}

void DialogConvolve::setColorMap(const std::string &name)
{
    _colormap = std::unique_ptr<ColorMap>(new ColorMap(name));
}

int DialogConvolve::exec()
{
//    on_previewButton_clicked();
    return QDialog::exec();
}

void DialogConvolve::changeSelectedFrame(int selected_frame)
{
    Q_UNUSED(selected_frame)

    updatePreview();
}

void DialogConvolve::updatePreview()
{
    int selected_data = ui->dataList->currentRow();

    auto data = _data[selected_data];

    int selected_frame = ui->frameIndex->value();

    auto frame = data->frame(selected_frame);

    int nrows = data->nRows();

    int ncols = data->nCols();

    auto kernel = _peakFinder->kernel();

    Eigen::MatrixXi clamped_result = data->convolvedFrame(selected_frame,kernel);

//    // apply threshold in preview
//    if (ui->thresholdCheckBox->isChecked()) {
//        double avgData = std::ceil(frame.sum() / double(nrows*ncols));
//        double threshold = _peakFinder->getThresholdValue();
//        bool relativeThreshold = _peakFinder->getThresholdType() == 0;
//        threshold = relativeThreshold ? threshold*avgData : threshold;
//
//        for (int i = 0; i < nrows; ++i)
//            for (int j = 0; j < ncols; ++j)
//                result(i, j) = result(i, j) < threshold ? 0 : maxData-1;
//    }

    // clamp the result for the preview window
//    double minVal = result.minCoeff();
//    double maxVal = result.maxCoeff();
//    result.array() -= minVal;
//    result.array() *= static_cast<double>(maxData)/(maxVal-minVal);
//    clamped_result = result.cast<int>();

    QRect rect(0, 0, ncols, nrows);

    QImage image = _colormap->matToImage(clamped_result.cast<double>(), rect, clamped_result.maxCoeff());

    if (!_pxmapPreview)
        _pxmapPreview = _scene->addPixmap(QPixmap::fromImage(image));
    else
        _pxmapPreview->setPixmap(QPixmap::fromImage(image));
}

void DialogConvolve::changeConvolutionFilter(int selected_filter)
{
    Q_UNUSED(selected_filter)

    nsx::sptrConvolutionKernel kernel;

    if (QString::compare(ui->filterComboBox->currentText(),"none") == 0)
        kernel.reset();
    else {
        int selected_data = ui->dataList->currentRow();
        auto data = _data[selected_data];
        int nrows = data->nRows();
        int ncols = data->nCols();

        std::string kernelName = ui->filterComboBox->currentText().toStdString();
        auto kernelFactory = nsx::KernelFactory::Instance();
        kernel.reset(kernelFactory->create(kernelName, nrows, ncols));
    }

    // propagate changes to peak finder
    _peakFinder->setKernel(kernel);

    // need to update widgets with appropriate values
    ui->thresholdSpinBox->setValue(_peakFinder->getThresholdValue());
    ui->thresholdComboBox->setCurrentIndex(_peakFinder->getThresholdType());
    ui->integrationConfidenceSpinBox->setValue(_peakFinder->integrationConfidence());
    ui->blobConfidenceSpinBox->setValue(_peakFinder->searchConfidence());
    ui->minCompBox->setValue(_peakFinder->getMinComponents());
    ui->maxCompBox->setValue(_peakFinder->getMaxComponents());

    // update dialog with list of parameters
    buildConvolutionParametersList();

    updatePreview();
}


void DialogConvolve::changeThresholdValue(double value)
{
    _peakFinder->setThresholdValue(value);

    updatePreview();
}

void DialogConvolve::changeBlobConfidenceValue(double value)
{
    _peakFinder->setSearchConfidence(value);

    updatePreview();
}

void DialogConvolve::changeIntegrationConfidenceValue(double value)
{
    _peakFinder->setIntegrationConfidence(value);

    updatePreview();
}

void DialogConvolve::changeBlobMinSize(int size)
{
    _peakFinder->setMinComponents(size);

    updatePreview();
}

void DialogConvolve::changeBlobMaxSize(int size)
{
    _peakFinder->setMaxComponents(size);

    updatePreview();
}

void DialogConvolve::changeThresholdType(int index)
{
    _peakFinder->setThresholdType(index);

    updatePreview();
}

void DialogConvolve::changeConvolutionParameters(QStandardItem *item)
{
    // nothing to do
    if (!item || !_peakFinder)
        return;

    auto kernel = _peakFinder->kernel();

    // still nothing to do
    if (!kernel)
        return;

    auto& parameters = kernel->parameters();

    // extract name and value
    auto&& name = item->data(Qt::UserRole).toString().toStdString();

    auto&& value = item->data(Qt::EditRole).toDouble();

    // update
    parameters[name] = value;

    updatePreview();
}
