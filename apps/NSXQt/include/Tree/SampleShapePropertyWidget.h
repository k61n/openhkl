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
    void setChemicalFormula(const QString& formula);
    void on_pushButton_LoadMovie_clicked();
    bool setHullProperties();

private:
    SampleShapeItem* _caller;
    Ui::SampleShapePropertyWidget *ui;
};

#endif // SAMPLESHAPEPROPERTYWIDGET_H
