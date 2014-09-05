#ifndef DIALOGPROGRESS_H
#define DIALOGPROGRESS_H

#include "ui_dialog_Progress.h"
#include <QDialog>

//namespace Ui
//{
// class Ui::DialogProgress;
//}

class DialogProgress : public QDialog
{
    Q_OBJECT
public:
    explicit DialogProgress(QWidget *parent = 0):QDialog(parent),ui(new Ui::DialogProgress)
    {
        ui->setupUi(this);
        setModal(true);
    }
    void update(int step,int total)
    {
        ui->progressBar->setMaximum(total);
        ui->progressBar->setValue(step);
        ui->progressBar->update();
    }

    ~DialogProgress()
    {}
signals:

public slots:
private slots:


private:
    Ui::DialogProgress* ui;

};
#endif // DIALOGPROGRESS_H
