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
#include "DialogPeakFind.h"

#include "ui_DialogPeakFind.h"

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

    ui->filterComboBox->clear();
    nsx::KernelFactory kernel_factory;

    for (auto& k : kernel_factory.callbacks())
        ui->filterComboBox->addItem(QString::fromStdString(k.first));

    ui->filterComboBox->addItem("none");

    QSortFilterProxyModel* proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(ui->filterComboBox->model());
    ui->filterComboBox->model()->setParent(proxy);
    ui->filterComboBox->setModel(proxy);
    ui->filterComboBox->model()->sort(0);

    changeConvolutionFilter(0);

    connect(ui->applyThreshold,SIGNAL(stateChanged(int)),this,SLOT(clipPreview(int)));

    connect(ui->thresholdComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(changeThresholdType(int)));

    connect(ui->filterComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(changeConvolutionFilter(int)));

    connect(ui->minCompBox,SIGNAL(valueChanged(int)),this,SLOT(changeBlobMinSize(int)));

    connect(ui->maxCompBox,SIGNAL(valueChanged(int)),this,SLOT(changeBlobMaxSize(int)));

    connect(ui->thresholdSpinBox,SIGNAL(valueChanged(double)),this,SLOT(changeThresholdValue(double)));

    connect(ui->blobConfidenceSpinBox,SIGNAL(valueChanged(double)),this,SLOT(changeBlobConfidenceValue(double)));

    connect(ui->integrationConfidenceSpinBox,SIGNAL(valueChanged(double)),this,SLOT(changeIntegrationConfidenceValue(double)));

    connect(ui->dataList,SIGNAL(currentRowChanged(int)),this,SLOT(changeSelectedData(int)));

    connect(ui->frameSlider,SIGNAL(valueChanged(int)),this,SLOT(changeSelectedFrame(int)));

    connect(ui->frameIndex,SIGNAL(valueChanged(int)),this,SLOT(changeSelectedFrame(int)));
}

DialogPeakFind::~DialogPeakFind()
{
    delete ui;
    // this should be handled by Qt. check with valgrind?
    // delete _peakFindModel;
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

void DialogPeakFind::buildConvolutionParametersList()
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

void DialogPeakFind::setColorMap(const std::string &name)
{
    _colormap = std::unique_ptr<ColorMap>(new ColorMap(name));
}

int DialogPeakFind::exec()
{
//    on_previewButton_clicked();
    return QDialog::exec();
}

void DialogPeakFind::changeSelectedFrame(int selected_frame)
{
    Q_UNUSED(selected_frame)

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
        double avgData = std::ceil(frame.sum() / double(nrows*ncols));
        double threshold = _peakFinder->getThresholdValue();
        bool relativeThreshold = _peakFinder->getThresholdType() == 0;
        threshold = relativeThreshold ? threshold*avgData : threshold;

        for (int i = 0; i < nrows; ++i)
            for (int j = 0; j < ncols; ++j)
                convolved_frame(i, j) = convolved_frame(i, j) < threshold ? 0 : max_data-1;
    }

    // clamp the result for the preview window
    double minVal = convolved_frame.minCoeff();
    double maxVal = convolved_frame.maxCoeff();
    convolved_frame.array() -= minVal;
    convolved_frame.array() *= static_cast<double>(max_data)/(maxVal-minVal);

    QRect rect(0, 0, ncols, nrows);

    QImage image = _colormap->matToImage(convolved_frame.cast<double>(), rect, convolved_frame.maxCoeff());

    if (!_pxmapPreview)
        _pxmapPreview = _scene->addPixmap(QPixmap::fromImage(image));
    else
        _pxmapPreview->setPixmap(QPixmap::fromImage(image));
}

void DialogPeakFind::clipPreview(int state) {
    updatePreview();
}

void DialogPeakFind::changeConvolutionFilter(int selected_filter)
{
    Q_UNUSED(selected_filter)

    nsx::sptrConvolutionKernel kernel;

    std::string kernel_name = ui->filterComboBox->currentText().toStdString();

    if (kernel_name.compare("none") == 0)
        kernel.reset();
    else {
        int selected_data = ui->dataList->currentRow();
        auto data = _data[selected_data];

        nsx::KernelFactory kernel_factory;
        kernel = kernel_factory.create(kernel_name,{});
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


void DialogPeakFind::changeThresholdValue(double value)
{
    _peakFinder->setThresholdValue(value);

    updatePreview();
}

void DialogPeakFind::changeBlobConfidenceValue(double value)
{
    _peakFinder->setSearchConfidence(value);

    updatePreview();
}

void DialogPeakFind::changeIntegrationConfidenceValue(double value)
{
    _peakFinder->setIntegrationConfidence(value);

    updatePreview();
}

void DialogPeakFind::changeBlobMinSize(int size)
{
    _peakFinder->setMinComponents(size);

    updatePreview();
}

void DialogPeakFind::changeBlobMaxSize(int size)
{
    _peakFinder->setMaxComponents(size);

    updatePreview();
}

void DialogPeakFind::changeThresholdType(int index)
{
    _peakFinder->setThresholdType(index);

    updatePreview();
}

void DialogPeakFind::changeConvolutionParameters(QStandardItem *item)
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
