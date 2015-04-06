#ifndef UnitCellPropertyWidget_H
#define UnitCellPropertyWidget_H

#include <QWidget>
#include <memory>
#include <Eigen/Dense>

namespace Ui {
class UnitCellPropertyWidget;
}
namespace SX{
    namespace Crystal{
    class UnitCell;
    }
}

class UnitCellItem;

class UnitCellPropertyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UnitCellPropertyWidget(UnitCellItem* caller,QWidget *parent = 0);
    ~UnitCellPropertyWidget();

signals:
    void activateIndexingMode(std::shared_ptr<SX::Crystal::UnitCell>);
    void cellUpdated();
private slots:
    void getLatticeParams();
    void setLatticeParams();
    void on_pushButton_Info_clicked();
    void on_pushButton_Index_clicked();
    void on_pushButton_AutoIndexing_clicked();
    void on_pushButton_Refine_clicked();
    void setChemicalFormula(const QString &formula);
    void on_spinBox_Z_valueChanged(int arg1);
    void setMassDensity() const;
    void on_lineEdit_ChemicalFormula_editingFinished();
    void transform(const Eigen::Matrix3d& P);
    void on_pushButton_Refine_2_clicked();

    void on_pushButton_WeakPeaks_pressed();

public slots:
    void setCell(const SX::Crystal::UnitCell&);    

private:
    UnitCellItem* _unitCellItem;
    Ui::UnitCellPropertyWidget *ui;
};

#endif // UnitCellPropertyWidget_H
