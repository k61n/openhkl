#pragma once

#include <QWidget>

namespace Ui {
    class DetectorPropertyWidget;
}

class DetectorItem;

class DetectorPropertyWidget : public QWidget {

    Q_OBJECT

public:
    explicit DetectorPropertyWidget(DetectorItem* caller,QWidget *parent = nullptr);

    ~DetectorPropertyWidget();

private:

    void onSampleToDectorDistanceChanged(double distance);

private:
    Ui::DetectorPropertyWidget *_ui;

    DetectorItem* _detectorItem;
};
