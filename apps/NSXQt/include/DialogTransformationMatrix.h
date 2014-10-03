#ifndef DIALOGTRANSFORMATIONMATRIX_H
#define DIALOGTRANSFORMATIONMATRIX_H

#include <QDialog>
#include <Eigen/Dense>

namespace Ui {
class DialogTransformationmatrix;
}

class DialogTransformationmatrix : public QDialog
{
    Q_OBJECT

public:
    explicit DialogTransformationmatrix(QWidget *parent = 0);
    ~DialogTransformationmatrix();
    Eigen::Matrix3d getTransformation();
private:
    Ui::DialogTransformationmatrix *ui;
};

#endif // DIALOGTRANSFORMATIONMATRIX_H
