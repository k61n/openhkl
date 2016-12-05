#include "dialogs/PeakFitDialog.h"
#include "ui_PeakFitDialog.h"

PeakFitDialog::PeakFitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PeakFitDialog)
{
    ui->setupUi(this);
}

PeakFitDialog::~PeakFitDialog()
{
    delete ui;
}
