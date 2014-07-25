#ifndef DIALOGPEAKFIND_H
#define DIALOGPEAKFIND_H
#include "ui_dialog_PeakFind.h"
#include <QDialog>

//namespace Ui
//{
// class Ui::DialogPeakFind;
//}

class DialogPeakFind : public QDialog
{
    Q_OBJECT
public:
    explicit DialogPeakFind(QWidget *parent = 0):QDialog(parent),ui(new Ui::DialogPeakFind)
    {
        ui->setupUi(this);
        setModal(true);
    }
    double getConfidence()
    {
        return ui->confidenceSpinBox->value();
    }

    double getThreshold()
    {
        return ui->thresholdSpinBox->value();
    }

    ~DialogPeakFind()
    {}
signals:

public slots:
private slots:


private:
    Ui::DialogPeakFind* ui;

};
#endif // DIALOGPEAKFIND_H
