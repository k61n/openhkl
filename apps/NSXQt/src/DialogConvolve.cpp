// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#include "DialogConvolve.h"
#include "ui_DialogConvolve.h"

#include "AnnularKernel.h"

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

    this->setWindowTitle("Convolution Filter");

    //ui->graphicsView->setAcceptDrops();

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    // get pixmap from current frame
    int nrows = frame.rows();
    int ncols = frame.cols();
    int max_intensity = 1000;

    Eigen::Matrix<int,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> rowFrame(frame);
    pxmapPreview = scene->addPixmap(QPixmap::fromImage(Mat2QImage(rowFrame.data(), nrows, ncols, 0, ncols, 0, nrows, max_intensity)));

    //scene->addPixmap();

    // default value
    ui->thresholdSpinBox->setValue(100.0);

    _convolver = std::shared_ptr<SX::Imaging::Convolver>(new SX::Imaging::Convolver);

    _peakFindModel = new PeakFindModel(this);

    ui->treeView->setModel(_peakFindModel);
}

DialogConvolve::~DialogConvolve()
{
    delete ui;
    delete _peakFindModel;
}

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
    ui->treeWidgetOld->retrieveParameters();
    _kernel = ui->treeWidgetOld->getKernel();

    if (!_kernel) {
        qDebug() << "null kernel returned!";
        return;
    }

    std::cout << "got kernel" << std::endl;

    // dimensions must match image dimensions
    _kernel->getParameters()["rows"] = frame.rows();
    _kernel->getParameters()["cols"] = frame.cols();

    std::cout << "set kernel rows and cols: " << frame.rows() << " " << frame.cols() << std::endl;

    // set up convolver
    _convolver->setKernel(_kernel->getKernel());

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
    for ( int i = 0; i < nrows*ncols; ++i)
        clamped_result.data()[i] = result.data()[i] > getThreshold() ? max_intensity-1 : 0;

    pxmapPreview->setPixmap(QPixmap::fromImage(Mat2QImage(clamped_result.data(), frame.rows(), frame.cols(), 0, ncols, 0, nrows, max_intensity)));
}
