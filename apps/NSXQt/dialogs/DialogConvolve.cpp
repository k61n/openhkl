// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#include "DialogConvolve.h"
#include "ui_ConvolveDialog.h"

#include "KernelFactory.h"

#include "ColorMap.h"
#include <QImage>
#include <QSortFilterProxyModel>
#include <QTreeView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QList>

#include <Eigen/Core>
#include <QDebug>

#include <iostream>

#include "Types.h"

using RealMatrix = SX::Types::RealMatrix;

using std::cout;
using std::endl;

DialogConvolve::DialogConvolve(const Eigen::MatrixXi& currentFrame, std::shared_ptr<SX::Data::PeakFinder> peakFinder, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConvolve),
    _frame(currentFrame),
    _pxmapPreview(nullptr)
{
    ui->setupUi(this);

    //this->setWindowTitle(nsx");

    // disable resizing
    this->setFixedSize(this->size());

    _peakFinder = peakFinder;

    _scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(_scene);
    // flip image vertically to conform with DetectorScene
    ui->graphicsView->scale(1, -1);

    ui->filterComboBox->clear();
    SX::Imaging::KernelFactory* kernelFactory=SX::Imaging::KernelFactory::Instance();
    for (const auto& k : kernelFactory->list())
        ui->filterComboBox->addItem(QString::fromStdString(k));
    ui->filterComboBox->addItem("none");

    QSortFilterProxyModel* proxy = new QSortFilterProxyModel();
    proxy->setSourceModel(ui->filterComboBox->model());
    ui->filterComboBox->model()->setParent(proxy);
    ui->filterComboBox->setModel(proxy);
    ui->filterComboBox->model()->sort(0);
}

DialogConvolve::~DialogConvolve()
{
    delete ui;
    // this should be handled by Qt. check with valgrind?
    // delete _peakFindModel;
}

void DialogConvolve::buildTree()
{
    // reset tree
    QTreeView* treeView = ui->treeView;
    treeView->reset();
    treeView->header()->hide();

    // no peakfinder set?!
    if (!_peakFinder)
        return;

    // get the selected kernel (if any)
    std::shared_ptr<SX::Imaging::ConvolutionKernel> kernel = _peakFinder->getKernel();

    QStandardItemModel* model = new QStandardItemModel(this);

    // no kernel selected: do nothing
    if (kernel)
    {
        // get parameters
        std::map<std::string, double> parameters = kernel->getParameters();

        // iterate through parameters to build the tree
        for (auto it: parameters) {
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

    connect(model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(parameterChanged(QStandardItem*)));
}

void DialogConvolve::on_previewButton_clicked()
{
    RealMatrix data, result;
    Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> clamped_result;

    int nrows, ncols;

    nrows = _frame.rows();
    ncols = _frame.cols();

    auto kernel = _peakFinder->getKernel();

    int maxData = _frame.maxCoeff();

    if (kernel)
    {
        // set up convolver
        auto convolver = _peakFinder->getConvolver();
        convolver->setKernel(kernel->getKernel());

        // compute the convolution
        data = _frame.cast<double>();
        result = convolver->apply(data);
        clamped_result.resize(_frame.rows(),_frame.cols());
        double minVal = result.minCoeff();
        double maxVal = result.maxCoeff();
        result.array() -= minVal;
        result.array() *= static_cast<double>(maxData)/(maxVal-minVal);
        clamped_result = result.cast<int>();
    }
    else
        clamped_result = _frame;

    QImage image = Mat2QImage(clamped_result.data(), nrows, ncols, 0, ncols-1, 0, nrows-1, maxData);

    if (!_pxmapPreview)
        _pxmapPreview = _scene->addPixmap(QPixmap::fromImage(image));
    else
        _pxmapPreview->setPixmap(QPixmap::fromImage(image));
}

void DialogConvolve::on_filterComboBox_currentIndexChanged(int index)
{
    std::shared_ptr<SX::Imaging::ConvolutionKernel> kernel;

    if (QString::compare(ui->filterComboBox->currentText(),"none") == 0)
        kernel.reset();
    else
    {
        std::string kernelName = ui->filterComboBox->currentText().toStdString();
        SX::Imaging::KernelFactory* kernelFactory = SX::Imaging::KernelFactory::Instance();
        kernel.reset(kernelFactory->create(kernelName,_frame.rows(),_frame.cols()));
    }

    // propagate changes to peak finder
    _peakFinder->setKernel(kernel);

    // need to update widgets with appropriate values
    ui->thresholdSpinBox->setValue(_peakFinder->getThresholdValue());
    ui->thresholdComboBox->setCurrentIndex(_peakFinder->getThresholdType());
    ui->confidenceSpinBox->setValue(_peakFinder->getConfidence());
    ui->minCompBox->setValue(_peakFinder->getMinComponents());
    ui->maxCompBox->setValue(_peakFinder->getMaxComponents());

    // update dialog with list of parameters
    buildTree();
}


void DialogConvolve::on_thresholdSpinBox_valueChanged(double arg1)
{
    _peakFinder->setThresholdValue(arg1);
}

void DialogConvolve::on_confidenceSpinBox_valueChanged(double arg1)
{
    _peakFinder->setConfidence(arg1);
}

void DialogConvolve::on_minCompBox_valueChanged(int arg1)
{
    _peakFinder->setMinComponents(arg1);
}

void DialogConvolve::on_maxCompBox_valueChanged(int arg1)
{
    _peakFinder->setMaxComponents(arg1);
}

void DialogConvolve::on_thresholdComboBox_currentIndexChanged(int index)
{
    _peakFinder->setThresholdType(index);
    qDebug() << "threshold type index is " << index;
}

void DialogConvolve::parameterChanged(QStandardItem *item)
{
    // nothing to do
    if (!item || !_peakFinder)
        return;

    auto kernel = _peakFinder->getKernel();

    // still nothing to do
    if (!kernel)
        return;

    auto& parameters = kernel->getParameters();

    // extract name and value
    auto name = item->data(Qt::UserRole).toString().toStdString();
    auto value = item->data(Qt::EditRole).toDouble();

    // update
    parameters[name] = value;
}
