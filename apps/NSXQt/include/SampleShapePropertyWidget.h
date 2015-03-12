#ifndef SAMPLESHAPEPROPERTYWIDGET_H
#define SAMPLESHAPEPROPERTYWIDGET_H

#include <QWidget>

namespace Ui {
class SampleShapePropertyWidget;
}

class SampleShapeItem;

class SampleShapePropertyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SampleShapePropertyWidget(SampleShapeItem* caller, QWidget *parent = 0);
    ~SampleShapePropertyWidget();

private slots:
    void on_pushButton_Info_clicked();
    void setChemicalFormula(const QString& formula);
private:
    SampleShapeItem* _caller;
    Ui::SampleShapePropertyWidget *ui;
};

#endif // SAMPLESHAPEPROPERTYWIDGET_H
