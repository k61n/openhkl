#include "ui_NumorPropertyWidget.h"
#include "Tree/NumorItem.h"
#include "Tree/NumorPropertyWidget.h"

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
