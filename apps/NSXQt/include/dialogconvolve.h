#ifndef DIALOGCONVOLVE_H
#define DIALOGCONVOLVE_H

#include <QDialog>

namespace Ui {
class DialogConvolve;
}

class DialogConvolve : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConvolve(QWidget *parent = 0);
    ~DialogConvolve();

private:
    Ui::DialogConvolve *ui;

};
#endif // DIALOGCONVOLVE_H
