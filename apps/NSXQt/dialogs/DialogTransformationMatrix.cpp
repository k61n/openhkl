#include "DialogTransformationMatrix.h"
#include "ui_TransformationMatrixDialog.h"

#include "UnitCell.h"

DialogTransformationmatrix::DialogTransformationmatrix(sptrUnitCell unitCell, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogTransformationmatrix),
    _unitCell(unitCell)
{
    ui->setupUi(this);

    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(getTransformation()));

}

DialogTransformationmatrix::~DialogTransformationmatrix()
{
    delete ui;
}

void DialogTransformationmatrix::getTransformation()
{
    Eigen::Matrix3d P;
    P << ui->P00->value(), ui->P01->value(), ui->P02->value(),
         ui->P10->value(), ui->P11->value(), ui->P12->value(),
         ui->P20->value(), ui->P21->value(), ui->P22->value();

    _unitCell->transform(P);
}
