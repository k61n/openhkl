#pragma once

#include <QDialog>

namespace Ui {
class DialogCalibrateDistance;
}

class DialogCalibrateDistance : public QDialog {
  Q_OBJECT

public:
  explicit DialogCalibrateDistance(QWidget *parent = 0);
  ~DialogCalibrateDistance();

signals:
  void on_calibrateDistanceSpinBox_valueChanged(double arg1);
  void on_calibrateDistanceButtonOK_accepted();

private slots:

private:
  Ui::DialogCalibrateDistance *ui;
};
