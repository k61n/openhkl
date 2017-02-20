#include "dialogs/ResolutionCutoffDialog.h"
#include "ui_ResolutionCutoffDialog.h"

ResolutionCutoffDialog::ResolutionCutoffDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResolutionCutoffDialog)
{
    ui->setupUi(this);
}

ResolutionCutoffDialog::~ResolutionCutoffDialog()
{
    delete ui;
}

double ResolutionCutoffDialog::dMin()
{
    return ui->dMinSpinBox->value();
}

double ResolutionCutoffDialog::dMax()
{
    return ui->dMaxSpinBox->value();
}
