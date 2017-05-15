// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#include "DialogIntegrate.h"
#include "ui_DialogIntegrate.h"

DialogIntegrate::DialogIntegrate(QWidget *parent):
    QDialog(parent),
    _ui(new Ui::DialogIntegrate)
{
    _ui->setupUi(this);

    connect(_ui->okButton, &QPushButton::clicked, [=] {this->done(QDialog::Accepted);});
    connect(_ui->cancelButton, &QPushButton::clicked, [=] {this->done(QDialog::Rejected);});

    // resolve method overload
    auto valueChanged = static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged);

    _peakScale = _ui->shapeScale->value();
    _bkgScale = _ui->bkgScale->value();

    connect(_ui->shapeScale, valueChanged, [=](double v) {this->_peakScale = v;});
    connect(_ui->bkgScale, valueChanged, [=](double v) {this->_bkgScale = v;});
}

DialogIntegrate::~DialogIntegrate()
{
    delete _ui;
}

