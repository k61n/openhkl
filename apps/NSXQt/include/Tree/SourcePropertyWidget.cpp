#include "include/Tree/SourcePropertyWidget.h"
#include "ui_SourcePropertyWidget.h"

SourcePropertyWidget::SourcePropertyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SourcePropertyWidget)
{
    ui->setupUi(this);
}

SourcePropertyWidget::~SourcePropertyWidget()
{
    delete ui;
}
