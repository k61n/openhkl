#include "DialogCalibrateDistance.h"
#include "ui_DialogCalibrateDistance.h"

DialogCalibrateDistance::DialogCalibrateDistance(QWidget *parent)
    : QDialog(parent), ui(new Ui::DialogCalibrateDistance) {
  ui->setupUi(this);
}

DialogCalibrateDistance::~DialogCalibrateDistance() { delete ui; }
