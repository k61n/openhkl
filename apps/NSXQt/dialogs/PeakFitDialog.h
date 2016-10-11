#ifndef PEAKFITDIALOG_H
#define PEAKFITDIALOG_H

#include <QDialog>

#include "ExperimentTree.h"

class QGraphicsScene;
class QGraphicsPixmapItem;

class SessionModel;

namespace SX {
namespace Crystal {
class Peak3D;
}
}

namespace Ui {
class PeakFitDialog;
}

class PeakFitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PeakFitDialog(std::shared_ptr<SessionModel> session, QWidget *parent = 0);
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
    std::shared_ptr<SessionModel> _session;
    QGraphicsScene* _scene;
    QGraphicsPixmapItem* _image;
    Eigen::RowVector3i _hkl;

    int _xmin, _xmax, _ymin, _ymax, _zmin, _zmax;

    Eigen::RowVectorXd _fitParams;
    SX::Crystal::Peak3D* _peak;
};

#endif // PEAKFITDIALOG_H
