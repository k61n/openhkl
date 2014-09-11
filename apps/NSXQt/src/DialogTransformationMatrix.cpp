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
