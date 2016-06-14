// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef DIALOGCONVOLVE_H
#define DIALOGCONVOLVE_H

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

#include <Eigen/Core>

#include <map>
#include <string>

#include "Convolver.h"
#include "ConvolutionKernel.h"

namespace Ui {
class DialogConvolve;
}

class DialogConvolve : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConvolve(const Eigen::MatrixXi& currentFrame, QWidget *parent = 0);
    ~DialogConvolve();

    double getThreshold();

private slots:
    void on_pushButton_clicked();

private:
    Ui::DialogConvolve *ui;
    QGraphicsScene* scene;
    QGraphicsPixmapItem* pxmapPreview;

    Eigen::MatrixXi frame;

    SX::Imaging::Convolver _convolver;
    SX::Imaging::ConvolutionKernel* _kernel;

};
#endif // DIALOGCONVOLVE_H
