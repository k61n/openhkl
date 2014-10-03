#include "DialogTransformationMatrix.h"
#include "ui_dialogtransformationmatrix.h"

DialogTransformationmatrix::DialogTransformationmatrix(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogTransformationmatrix)
{
    ui->setupUi(this);
}

DialogTransformationmatrix::~DialogTransformationmatrix()
{
    delete ui;
}

Eigen::Matrix3d DialogTransformationmatrix::getTransformation()
{
    Eigen::Matrix3d P;
    P << ui->P00->value(), ui->P01->value(), ui->P02->value(),
         ui->P10->value(), ui->P11->value(), ui->P12->value(),
         ui->P20->value(), ui->P21->value(), ui->P22->value();
    return P;
}
