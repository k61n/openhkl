#include "include/ShapePropertyWidget.h"
#include "ui_ShapePropertyWidget.h"

ShapePropertyWidget::ShapePropertyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShapePropertyWidget)
{
    ui->setupUi(this);
}

ShapePropertyWidget::~ShapePropertyWidget()
{
    delete ui;
}
