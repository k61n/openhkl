#pragma once

#include <QWidget>
#include <core/Detector.h>
#include "ui_DetectorPropertyWidget.h"

class DetectorPropertyWidget : public QWidget {

    Q_OBJECT

public:
    explicit DetectorPropertyWidget(nsx::Detector& detector);

    ~DetectorPropertyWidget();

private:
    void onSampleToDectorDistanceChanged(double distance);

private:
    Ui::DetectorPropertyWidget *_ui;

    nsx::Detector& _detector;
};
