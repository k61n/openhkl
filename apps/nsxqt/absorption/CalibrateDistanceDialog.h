#pragma once

#include <QDialog>

namespace Ui {
class CalibrateDistanceDialog;
}

class CalibrateDistanceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CalibrateDistanceDialog(QWidget *parent = 0);
    ~CalibrateDistanceDialog();

signals:
    void on_calibrateDistanceSpinBox_valueChanged(double arg1);
    void on_calibrateDistanceButtonOK_accepted();

private slots:


private:
    Ui::CalibrateDistanceDialog *ui;
};
