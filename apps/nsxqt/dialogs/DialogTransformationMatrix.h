#ifndef NSXQT_DIALOGTRANSFORMATIONMATRIX_H
#define NSXQT_DIALOGTRANSFORMATIONMATRIX_H

#include <memory>

#include <Eigen/Dense>

#include <QDialog>

namespace Ui {
class DialogTransformationmatrix;
}

namespace nsx {
class UnitCell;
}

class Widget;

class DialogTransformationmatrix : public QDialog
{
    Q_OBJECT

signals:
    void getMatrix(const Eigen::Matrix3d& m);

public:
    explicit DialogTransformationmatrix(std::shared_ptr<nsx::UnitCell> unitCell, QWidget *parent=0);
    ~DialogTransformationmatrix();

public slots:
    void getTransformation();

private:
    Ui::DialogTransformationmatrix *ui;
    std::shared_ptr<nsx::UnitCell> _unitCell;
};

#endif // NSXQT_DIALOGTRANSFORMATIONMATRIX_H
