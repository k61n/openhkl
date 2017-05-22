#ifndef NSXQT_UNITCELLPROPERTYWIDGET_H
#define NSXQT_UNITCELLPROPERTYWIDGET_H

#include <memory>

#include <QWidget>

#include <nsxlib/utils/Types.h>

namespace Ui {
class UnitCellPropertyWidget;
}

namespace nsx {
class UnitCell;
}

class QString;

class UnitCellItem;

class UnitCellPropertyWidget : public QWidget
{
    Q_OBJECT

public:

    using sptrUnitCell = std::shared_ptr<nsx::UnitCell>;

    explicit UnitCellPropertyWidget(UnitCellItem* caller,QWidget *parent = 0);
    ~UnitCellPropertyWidget();

public slots:

    void updateCellParameters(sptrUnitCell);

private slots:
    void getLatticeParams();
    void setLatticeParams();
    void setChemicalFormula(const QString& formula);
    void on_spinBox_Z_valueChanged(int arg1);
    void setMassDensity() const;
    void on_lineEdit_ChemicalFormula_editingFinished();
    void onCompleterActivated(const QString&);
    void setHKLTolerance(double);

private:
    UnitCellItem* _unitCellItem;
    Ui::UnitCellPropertyWidget *ui;
};

#endif // NSXQT_UNITCELLPROPERTYWIDGET_H
