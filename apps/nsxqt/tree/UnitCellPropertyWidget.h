#ifndef NSXQT_UNITCELLPROPERTYWIDGET_H
#define NSXQT_UNITCELLPROPERTYWIDGET_H

#include <memory>

#include <QWidget>

#include <nsxlib/CrystalTypes.h>

namespace Ui {
class UnitCellPropertyWidget;
}

class QString;

class UnitCellItem;

class UnitCellPropertyWidget : public QWidget
{
    Q_OBJECT

public:

    explicit UnitCellPropertyWidget(UnitCellItem* caller,QWidget *parent = 0);
    ~UnitCellPropertyWidget();

public slots:

    void updateCellParameters(nsx::sptrUnitCell);

private slots:
    void getLatticeParams();
    void setLatticeParams();
    void setChemicalFormula(const QString& formula);
    void on_spinBox_Z_valueChanged(int arg1);
    void setMassDensity() const;
    void on_lineEdit_ChemicalFormula_editingFinished();
    void onCompleterActivated(const QString&);
    void setIndexingTolerance(double);

private:
    UnitCellItem* _unitCellItem;
    Ui::UnitCellPropertyWidget *ui;
};

#endif // NSXQT_UNITCELLPROPERTYWIDGET_H
