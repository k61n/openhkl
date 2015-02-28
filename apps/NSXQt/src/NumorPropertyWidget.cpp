#include "NumorPropertyWidget.h"
#include "ui_NumorPropertyWidget.h"
#include "NumorItem.h"

NumorPropertyWidget::NumorPropertyWidget(NumorItem* caller,QWidget *parent) :
    _numorItem(caller),
    QWidget(parent),
    ui(new Ui::NumorPropertyWidget)
{
    ui->setupUi(this);
}

NumorPropertyWidget::~NumorPropertyWidget()
{
    delete ui;
}
