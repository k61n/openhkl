// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXTOOL_DIALOGCONVOLVE_H_
#define NSXTOOL_DIALOGCONVOLVE_H_

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QStandardItemModel>

#include <Eigen/Core>

#include <map>
#include <string>
#include <memory>

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
    double getConfidence();

    int getMinComponents();
    int getMaxComponents();

    bool thesholdIsRelative();

    std::shared_ptr<SX::Imaging::Convolver> getConvolver();
    std::shared_ptr<SX::Imaging::ConvolutionKernel> getKernel();


private slots:
    void on_previewButton_clicked();

private:
    Ui::DialogConvolve *ui;
    QGraphicsScene* scene;
    QGraphicsPixmapItem* pxmapPreview;

    Eigen::MatrixXi frame;

    std::shared_ptr<SX::Imaging::Convolver> _convolver;
    std::shared_ptr<SX::Imaging::ConvolutionKernel> _kernel;

};
#endif // NSXTOOL_DIALOGCONVOLVE_H_
