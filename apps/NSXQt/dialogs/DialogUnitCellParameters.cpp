#include "dialogs/DialogUnitCellParameters.h"
#include "ui_DialogUnitCellParameters.h"

#include "Units.h"
#include "UnitCell.h"

DialogUnitCellParameters::DialogUnitCellParameters(sptrUnitCell unitCell, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogUnitCellParameters),
    _unitCell(unitCell)
{
    ui->setupUi(this);

    ui->labelAlpha->setText(QString((QChar) 0x03B1));
    ui->labelBeta->setText(QString((QChar) 0x03B2));
    ui->labelGamma->setText(QString((QChar) 0x03B3));

    ui->a->setValue(_unitCell->getA());
    ui->b->setValue(_unitCell->getB());
    ui->c->setValue(_unitCell->getC());
    ui->alpha->setValue(_unitCell->getAlpha()/SX::Units::deg);
    ui->beta->setValue(_unitCell->getBeta()/SX::Units::deg);
    ui->gamma->setValue(_unitCell->getGamma()/SX::Units::deg);

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
    double alpha=ui->alpha->value()*SX::Units::deg;
    double beta=ui->beta->value()*SX::Units::deg;
    double gamma=ui->gamma->value()*SX::Units::deg;

    _unitCell->setParams(a,b,c,alpha,beta,gamma);
}
