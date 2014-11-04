#include "include/ExperimentDialog.h"
#include "ui_ExperimentDialog.h"

ExperimentDialog::ExperimentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExperimentDialog)
{
    ui->setupUi(this);
}

ExperimentDialog::~ExperimentDialog()
{
    delete ui;
}
