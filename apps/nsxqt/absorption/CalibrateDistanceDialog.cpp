#include "CalibrateDistanceDialog.h"
#include "ui_CalibrateDistanceDialog.h"

CalibrateDistanceDialog::CalibrateDistanceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CalibrateDistanceDialog)
{
    ui->setupUi(this);
}

CalibrateDistanceDialog::~CalibrateDistanceDialog()
{
    delete ui;
}


