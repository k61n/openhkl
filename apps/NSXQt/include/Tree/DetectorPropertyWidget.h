#ifndef DETECTORPROPERTYWIDGET_H
#define DETECTORPROPERTYWIDGET_H

#include <QWidget>

namespace Ui {
class DetectorPropertyWidget;
}

class DetectorItem;

class DetectorPropertyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DetectorPropertyWidget(DetectorItem* caller,QWidget *parent = 0);
    ~DetectorPropertyWidget();

private:
    DetectorItem* _detectorItem;
    Ui::DetectorPropertyWidget *ui;
};

#endif // DETECTORPROPERTYWIDGET_H
