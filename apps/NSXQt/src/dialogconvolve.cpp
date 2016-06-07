#include "dialogconvolve.h"
#include "ui_dialogconvolve.h"

DialogConvolve::DialogConvolve(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConvolve)
{
    ui->setupUi(this);

    this->setWindowTitle("Convolution Filter");
}

DialogConvolve::~DialogConvolve()
{
    delete ui;
}


