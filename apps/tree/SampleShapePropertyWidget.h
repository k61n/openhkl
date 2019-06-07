#pragma once

#include <QWidget>

namespace Ui {
class SampleShapePropertyWidget;
}

class SampleShapeItem;

class SampleShapePropertyWidget : public QWidget {
    Q_OBJECT

public:
    explicit SampleShapePropertyWidget(SampleShapeItem* caller, QWidget* parent = 0);
    ~SampleShapePropertyWidget();

private slots:
    void on_pushButton_LoadMovie_clicked();
    void setHullProperties();

private:
    SampleShapeItem* _caller;
    Ui::SampleShapePropertyWidget* ui;
};
