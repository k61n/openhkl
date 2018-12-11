#pragma once

#include <QWidget>
#include "ui_NumorPropertyWidget.h"
#include <core/DataTypes.h>

class NumorPropertyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NumorPropertyWidget(const nsx::sptrDataSet& data);
    ~NumorPropertyWidget();

private:
    Ui::NumorPropertyWidget *ui;
};
