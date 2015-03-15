#include "include/Tree/SourcePropertWidget.h"
#include "ui_SourcePropertWidget.h"
#include "include/Tree/SourceItem.h"
#include "Diffractometer.h"
#include "Source.h"

SourcePropertyWidget::SourcePropertyWidget(SourceItem* caller,QWidget *parent) :
    _caller(caller),
    QWidget(parent),
    ui(new Ui::SourcePropertWidget)
{
    ui->setupUi(this);
    auto source=_caller->getExperiment()->getDiffractometer()->getSource();
    ui->lineEdit->setText(QString::number(source->getWavelength()));
}

SourcePropertyWidget::~SourcePropertyWidget()
{
    delete ui;
}
