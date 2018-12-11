#pragma once

#include "ui_SampleShapePropertyWidget.h"
#include <QWidget>
#include <core/ConvexHull.h>
#include <core/Experiment.h>

class SampleShapePropertyWidget : public QWidget {
    Q_OBJECT

public:
    explicit SampleShapePropertyWidget(const nsx::sptrExperiment& experiment);
    ~SampleShapePropertyWidget();

private slots:
    void on_pushButton_LoadMovie_clicked();
    void setHullProperties();

private:
    const nsx::sptrExperiment& _experiment;
    const nsx::ConvexHull& _shape;
    Ui::SampleShapePropertyWidget* ui;
};
