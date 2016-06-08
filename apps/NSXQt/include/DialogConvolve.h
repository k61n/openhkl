#ifndef DIALOGCONVOLVE_H
#define DIALOGCONVOLVE_H

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

#include <Eigen/Core>

namespace Ui {
class DialogConvolve;
}

class DialogConvolve : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConvolve(const Eigen::MatrixXi& currentFrame, QWidget *parent = 0);
    ~DialogConvolve();

private:
    Ui::DialogConvolve *ui;
    QGraphicsScene* scene;
    QGraphicsPixmapItem* pxmapPreview;

    Eigen::MatrixXi frame;

};
#endif // DIALOGCONVOLVE_H
