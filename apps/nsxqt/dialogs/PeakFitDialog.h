#ifndef PEAKFITDIALOG_H
#define PEAKFITDIALOG_H

#include <QDialog>

#include "tree/ExperimentTree.h"

class QGraphicsScene;
class QGraphicsPixmapItem;

class SessionModel;

namespace SX {
namespace Crystal {
class Peak3D;
using sptrPeak3D=std::shared_ptr<Peak3D>;
}
}

namespace Ui {
class PeakFitDialog;
}

using SX::Crystal::sptrPeak3D;

class PeakFitDialog : public QDialog {
    Q_OBJECT

public:
    explicit PeakFitDialog(SessionModel* session, QWidget *parent = 0);
    ~PeakFitDialog();
    void updateView();
    void updatePeak();


public slots:
    void changeFrame(int value);
    void changeH(int value);
    void changeK(int value);
    void changeL(int value);
    void fitPeakShape();
    void checkCollisions();

private:
    Ui::PeakFitDialog *ui;
    SessionModel* _session;
    QGraphicsScene* _scene;
    QGraphicsPixmapItem* _image;
    Eigen::RowVector3i _hkl;

    int _xmin, _xmax, _ymin, _ymax, _zmin, _zmax;

    Eigen::RowVectorXd _fitParams;
    sptrPeak3D _peak;
};

#endif // PEAKFITDIALOG_H
