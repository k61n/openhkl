#include "dialogs/DialogCalculatedPeaks.h"
#include "ui_DialogCalculatedPeaks.h"

DialogCalculatedPeaks::DialogCalculatedPeaks(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCalculatedPeaks)
{
    ui->setupUi(this);
}

DialogCalculatedPeaks::~DialogCalculatedPeaks()
{
    delete ui;
}

double DialogCalculatedPeaks::dMin()
{
    return ui->dMinSpinBox->value();
}

double DialogCalculatedPeaks::dMax()
{
    return ui->dMaxSpinBox->value();
}

double DialogCalculatedPeaks::searchRadius()
{
    return ui->radiusSpinBox->value();
}
