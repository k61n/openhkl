#ifndef SOURCEPROPERTYWIDGET_H
#define SOURCEPROPERTYWIDGET_H

#include <QWidget>

namespace Ui {
class SourcePropertyWidget;
}

class SourceItem;

class SourcePropertyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SourcePropertyWidget(SourceItem* caller,QWidget *parent = 0);
    ~SourcePropertyWidget();

private slots:
    void on_doubleSpinBox_Wavelength_valueChanged(double arg1);

    void on_doubleSpinBox_Width_valueChanged(double arg1);

    void on_doubleSpinBox_Height_valueChanged(double arg1);

private:
    Ui::SourcePropertyWidget *ui;
    SourceItem* _caller;
};

#endif // SOURCEPROPERTYWIDGET_H
