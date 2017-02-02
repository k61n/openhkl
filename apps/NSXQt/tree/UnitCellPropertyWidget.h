#ifndef UnitCellPropertyWidget_H
#define UnitCellPropertyWidget_H

#include <QWidget>
#include <memory>
#include <Eigen/Dense>

#include <nsxlib/utils/Types.h>

namespace Ui {
class UnitCellPropertyWidget;
}
namespace SX{
    namespace Crystal{
    class UnitCell;
    }
}

class UnitCellItem;

using SX::Crystal::sptrUnitCell;

class UnitCellPropertyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UnitCellPropertyWidget(UnitCellItem* caller,QWidget *parent = 0);
    ~UnitCellPropertyWidget();

public slots:

    void updateCellParameters(sptrUnitCell);

private slots:
    void getLatticeParams();
    void setLatticeParams();
    void setChemicalFormula(const QString &formula);
    void on_spinBox_Z_valueChanged(int arg1);
    void setMassDensity() const;
    void on_lineEdit_ChemicalFormula_editingFinished();
    void onCompleterActivated(const QString &);
    void setHKLTolerance(double);

private:
    UnitCellItem* _unitCellItem;
    Ui::UnitCellPropertyWidget *ui;
};

#endif // UnitCellPropertyWidget_H
