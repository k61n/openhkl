#include "DialogConvolve.h"
#include "ui_DialogConvolve.h"

#include "ColorMap.h"
#include <QImage>

#include <Eigen/Core>

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


}

DialogConvolve::~DialogConvolve()
{
    delete ui;
}


