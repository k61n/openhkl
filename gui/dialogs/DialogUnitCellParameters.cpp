#include <nsxlib/UnitCell.h>
#include <nsxlib/Units.h>

#include "DialogUnitCellParameters.h"
#include "ui_DialogUnitCellParameters.h"

DialogUnitCellParameters::DialogUnitCellParameters(nsx::sptrUnitCell unitCell, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogUnitCellParameters),
    _unitCell(unitCell)
{
    ui->setupUi(this);

    ui->labelAlpha->setText(QString((QChar) 0x03B1));
    ui->labelBeta->setText(QString((QChar) 0x03B2));
    ui->labelGamma->setText(QString((QChar) 0x03B3));

    auto ch = _unitCell->character();

    ui->a->setValue(ch.a);
    ui->b->setValue(ch.b);
    ui->c->setValue(ch.c);
    ui->alpha->setValue(ch.alpha/nsx::deg);
    ui->beta->setValue(ch.beta/nsx::deg);
    ui->gamma->setValue(ch.gamma/nsx::deg);

    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(setUnitCellParameters()));
}

DialogUnitCellParameters::~DialogUnitCellParameters()
{
    delete ui;
}

void DialogUnitCellParameters::setUnitCellParameters()
{

    double a=ui->a->value();
    double b=ui->b->value();
    double c=ui->c->value();
    double alpha=ui->alpha->value()*nsx::deg;
    double beta=ui->beta->value()*nsx::deg;
    double gamma=ui->gamma->value()*nsx::deg;

    _unitCell->setParameters(a,b,c,alpha,beta,gamma);
}
