#ifndef PEAKFITDIALOG_H
#define PEAKFITDIALOG_H

#include <QDialog>

#include "ExperimentTree.h"

class QGraphicsScene;
class QGraphicsPixmapItem;

namespace Ui {
class PeakFitDialog;
}

class PeakFitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PeakFitDialog(QWidget *parent = 0);
    ~PeakFitDialog();
    void updateView();
    void updatePeak();

public slots:
    void changeFrame(int value);
    void changeH(int value);
    void changeK(int value);
    void changeL(int value);

private:
    Ui::PeakFitDialog *ui;
    ExperimentTree* _tree;
    QGraphicsScene* _scene;
    QGraphicsPixmapItem* _image;
    Eigen::RowVector3i _hkl;

    int _xmin, _xmax, _ymin, _ymax, _zmin, _zmax;
};

#endif // PEAKFITDIALOG_H
