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
signals:
    void getMatrix(const Eigen::Matrix3d& m);
public:
    explicit DialogTransformationmatrix(QWidget *parent = 0);
    ~DialogTransformationmatrix();
    void getTransformation();
private:
    Ui::DialogTransformationmatrix *ui;
};

#endif // DIALOGTRANSFORMATIONMATRIX_H
