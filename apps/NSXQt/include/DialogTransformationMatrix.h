#ifndef DIALOGTRANSFORMATIONMATRIX_H
#define DIALOGTRANSFORMATIONMATRIX_H

#include <QDialog>

namespace Ui {
class DialogTransformationmatrix;
}

class DialogTransformationmatrix : public QDialog
{
    Q_OBJECT

public:
    explicit DialogTransformationmatrix(QWidget *parent = 0);
    ~DialogTransformationmatrix();

private:
    Ui::DialogTransformationmatrix *ui;
};

#endif // DIALOGTRANSFORMATIONMATRIX_H
