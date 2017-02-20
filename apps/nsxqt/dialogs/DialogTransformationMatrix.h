#ifndef DIALOGTRANSFORMATIONMATRIX_H
#define DIALOGTRANSFORMATIONMATRIX_H

#include <QDialog>
#include <Eigen/Dense>

#include <nsxlib/utils/Types.h>

namespace Ui {
class DialogTransformationmatrix;
}

using SX::Crystal::sptrUnitCell;

class DialogTransformationmatrix : public QDialog
{
    Q_OBJECT

signals:
    void getMatrix(const Eigen::Matrix3d& m);

public:
    explicit DialogTransformationmatrix(sptrUnitCell unitCell, QWidget *parent=0);
    ~DialogTransformationmatrix();

public slots:
    void getTransformation();

private:
    Ui::DialogTransformationmatrix *ui;
    sptrUnitCell _unitCell;
};

#endif // DIALOGTRANSFORMATIONMATRIX_H
