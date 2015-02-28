#ifndef DETECTORPROPERTYWIDGET_H
#define DETECTORPROPERTYWIDGET_H

#include <QWidget>

namespace Ui {
class DetectorPropertyWidget;
}

class DetectorPropertyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DetectorPropertyWidget(QWidget *parent = 0);
    ~DetectorPropertyWidget();

private:
    Ui::DetectorPropertyWidget *ui;
};

#endif // DETECTORPROPERTYWIDGET_H
