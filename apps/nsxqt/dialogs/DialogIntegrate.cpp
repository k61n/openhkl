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
}

DialogIntegrate::~DialogIntegrate()
{
    delete _ui;
}

double DialogIntegrate::peakScale() const
{
    return _ui->peakScale->value();
}

double DialogIntegrate::backgroundScale() const
{
    return _ui->bkgScale->value();
}

double DialogIntegrate::dMin() const
{
    return _ui->dMin->value();
}

double DialogIntegrate::dMax() const
{
    return _ui->dMax->value();
}
