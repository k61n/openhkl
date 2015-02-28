#ifndef SHAPEPROPERTYWIDGET_H
#define SHAPEPROPERTYWIDGET_H

#include <QWidget>

namespace Ui {
class ShapePropertyWidget;
}

class ShapePropertyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ShapePropertyWidget(QWidget *parent = 0);
    ~ShapePropertyWidget();

private:
    Ui::ShapePropertyWidget *ui;
};

#endif // SHAPEPROPERTYWIDGET_H
