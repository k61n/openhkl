#pragma once

#include <QWidget>
#include <core/Sample.h>

#include "ui_SamplePropertyWidget.h"

class SamplePropertyWidget : public QWidget {
    Q_OBJECT

public:
    explicit SamplePropertyWidget(const nsx::Sample& sample);

    ~SamplePropertyWidget();

private:
    Ui::SamplePropertyWidget* _ui;
};
