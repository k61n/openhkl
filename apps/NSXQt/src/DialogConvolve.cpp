// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#include "DialogConvolve.h"
#include "ui_DialogConvolve.h"

#include "AnnularKernel.h"
#include "DeltaKernel.h"

#include "ColorMap.h"
#include <QImage>

#include <Eigen/Core>
#include <QDebug>

#include <iostream>

#include "Types.h"

using RealMatrix = SX::Types::RealMatrix;

using std::cout;
using std::endl;

DialogConvolve::DialogConvolve(const Eigen::MatrixXi& currentFrame, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConvolve),
    frame(currentFrame)
{
    ui->setupUi(this);

    //this->setWindowTitle(nsx");

    // disable resizing
    this->setFixedSize(this->size());


    _peakFinder = std::shared_ptr<SX::Data::PeakFinder>(new SX::Data::PeakFinder);
    _convolver = std::shared_ptr<SX::Imaging::Convolver>(new SX::Imaging::Convolver);
    _peakFinder->setConvolver(_convolver);

    //ui->graphicsView->setAcceptDrops();

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    // get pixmap from current frame
    int nrows = frame.rows();
    int ncols = frame.cols();
    int max_intensity = 1000;

    Eigen::Matrix<int,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> rowFrame(frame);
    // jmf debug testing
    pxmapPreview = scene->addPixmap(QPixmap::fromImage(Mat2QImage(rowFrame.data(), nrows, ncols, 0, ncols-1, 0, nrows-1, max_intensity)));

    //scene->addPixmap();


    // default value
    //ui->thresholdSpinBox->setValue(100.0);

    //ui->treeView->setModel(_peakFindModel);
}

DialogConvolve::~DialogConvolve()
{
    delete ui;
    // this should be handled by Qt. check with valgrind?
    // delete _peakFindModel;
}

void DialogConvolve::setPeakFinder(std::shared_ptr<SX::Data::PeakFinder> peakFinder)
{
    _peakFinder = peakFinder;
    std::shared_ptr<SX::Imaging::ConvolutionKernel> kernel = peakFinder->getKernel();

    if ( !_peakFinder)
        return;

    // need to update widgets with appropriate values
    ui->thresholdSpinBox->setValue(_peakFinder->getThresholdValue());
    ui->thresholdComboBox->setCurrentIndex(_peakFinder->getThresholdType());
    ui->confidenceSpinBox->setValue(_peakFinder->getConfidence());
    ui->minCompBox->setValue(_peakFinder->getMinComponents());
    ui->maxCompBox->setValue(_peakFinder->getMaxComponents());
    ui->filterComboBox->setCurrentIndex(_peakFinder->getKernelType());

    if ( kernel ) {
        ui->parameter1->setValue(kernel->getParameters()["r1"]);
        ui->parameter2->setValue(kernel->getParameters()["r2"]);
        ui->parameter3->setValue(kernel->getParameters()["r3"]);
    }
}
/*
double DialogConvolve::getThreshold()
{
    return ui->thresholdSpinBox->value();
}

double DialogConvolve::getConfidence()
{
    return ui->confidenceSpinBox->value();
}

int DialogConvolve::getMinComponents()
{
    return ui->minCompBox->value();
}

int DialogConvolve::getMaxComponents()
{
    return ui->maxCompBox->value();
}

bool DialogConvolve::thesholdIsRelative()
{
    switch (ui->thresholdComboBox->currentIndex()) {
    case 0:
        return true;
    case 1:
        return false;
    default:
        qDebug() << "warning: DialogConvolve: thesholdComboBox has invalid index!";
        return false;
    }
}

std::shared_ptr<SX::Imaging::Convolver> DialogConvolve::getConvolver()
{
    return _convolver;
}

std::shared_ptr<SX::Imaging::ConvolutionKernel> DialogConvolve::getKernel()
{

    return _kernel;
}
*/
void DialogConvolve::on_previewButton_clicked()
{
    // TODO: implement this
    std::cout << "push button clicked" << std::endl;
    RealMatrix data, result;
    Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> clamped_result;

    int nrows, ncols;

    nrows = frame.rows();
    ncols = frame.cols();

    // note that treeWidget retains ownership!
    //ui->treeWidgetOld->retrieveParameters();
    //_kernel = ui->treeWidgetOld->getKernel();
    auto kernel = _peakFinder->getKernel();

    if (!kernel) {
        qDebug() << "null kernel returned!";
        return;
    }

    std::cout << "got kernel" << std::endl;

    // dimensions must match image dimensions
    kernel->getParameters()["rows"] = frame.rows();
    kernel->getParameters()["cols"] = frame.cols();

    std::cout << "set kernel rows and cols: " << frame.rows() << " " << frame.cols() << std::endl;

    // set up convolver
    _convolver->setKernel(kernel->getKernel());

    std::cout << "initialized convolver" << std::endl;

    // compute the convolution!
    data = frame.cast<double>();

    cout << "cast data to double" << endl;

    result = _convolver->apply(data);
    clamped_result.resize(frame.rows(),frame.cols());

    cout << "convolved data successfully" << endl;

    int max_intensity = 1000;

    // apply a simple theshold
    // TODO: incorporate into GUI, or improve in some other way

    double background;

    // threshold relative to background
    if (_peakFinder->getThresholdType() == 0)
        background = data.sum() / ((double)frame.rows()*frame.cols());
    else
        background = 1.0;

    qDebug() << "Generating preview image with background of " << background;

    for ( int i = 0; i < nrows*ncols; ++i)
        clamped_result.data()[i] = result.data()[i] > _peakFinder->getThresholdValue()*background ? max_intensity-1 : 0;

    pxmapPreview->setPixmap(QPixmap::fromImage(Mat2QImage(clamped_result.data(), frame.rows(), frame.cols(), 0, ncols, 0, nrows, max_intensity)));
}

void DialogConvolve::on_filterComboBox_currentIndexChanged(int index)
{
    std::shared_ptr<SX::Imaging::ConvolutionKernel> kernel;

    switch(index)
    {
    // no kernel
    case 0:
        kernel.reset();
    // annular kernel
    case 1:
        kernel = std::shared_ptr<SX::Imaging::ConvolutionKernel>(new SX::Imaging::AnnularKernel());
        kernel->getParameters()["r1"] = ui->parameter1->value();
        kernel->getParameters()["r2"] = ui->parameter2->value();
        kernel->getParameters()["r3"] = ui->parameter3->value();
        break;
    // kronecker delta (debugging)
    case 2:
        kernel = std::shared_ptr<SX::Imaging::ConvolutionKernel>(new SX::Imaging::DeltaKernel());
        break;
    default:
        qDebug() << "Warning: unrecognized kernel selected -- defaulting to NO kernel";
        kernel.reset();
        break;
    }

    if (kernel ) {
        kernel->getParameters()["rows"] = frame.rows();
        kernel->getParameters()["cols"] = frame.cols();
    }

    // propagate changes to peak finder
    _peakFinder->setKernel(kernel);
}

void DialogConvolve::on_parameter1_valueChanged(int arg1)
{
    if ( _peakFinder->getKernel() )
        _peakFinder->getKernel()->getParameters()["r1"] = arg1;
}

void DialogConvolve::on_parameter2_valueChanged(int arg1)
{
    if ( _peakFinder->getKernel() )
        _peakFinder->getKernel()->getParameters()["r2"] = arg1;
}

void DialogConvolve::on_parameter3_valueChanged(int arg1)
{
    if ( _peakFinder->getKernel() )
        _peakFinder->getKernel()->getParameters()["r3"] = arg1;
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
