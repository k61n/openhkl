#include "ui_EmptyPropertyWidget.h"

#include "EmptyPropertyWidget.h"

EmptyPropertyWidget::EmptyPropertyWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::EmptyPropertyWidget)
{
    ui->setupUi(this);
}

EmptyPropertyWidget::~EmptyPropertyWidget()
{
    delete ui;
}
