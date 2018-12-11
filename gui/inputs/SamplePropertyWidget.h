#pragma once

#include <QWidget>
#include "SampleItem.h"

#include "ui_SamplePropertyWidget.h"

class SamplePropertyWidget : public QWidget
{
    Q_OBJECT

public:

    explicit SamplePropertyWidget(SampleItem* caller,QWidget* parent=0);

    ~SamplePropertyWidget();

private:

    Ui::SamplePropertyWidget *_ui;

    SampleItem* _sampleItem;
};
