#ifndef DIALOGUNITCELL_H
#define DIALOGUNITCELL_H
#include "ui_dialog_UnitCell.h"
#include <QDialog>
#include <vector>
#include <functional>
#include <memory>
#include <vector>
#include "UnitCell.h"
#include "Peak3D.h"
#include "LatticeFinder.h"
#include <tuple>
#include <UBMinimizer.h>

class DialogUnitCell : public QDialog
{
    Q_OBJECT
public:
    explicit DialogUnitCell(QWidget *parent = 0);
    void setPeaks(const std::vector<std::reference_wrapper<SX::Crystal::Peak3D>>& peaks);
    ~DialogUnitCell();
signals:
    void hasSolutions(const std::vector<SX::Crystal::UnitCell>&);
public slots:
    void getUnitCell();
    void setUpValues();
    void setTransformationMatrix();
    void acceptSolution(int i);
private slots:

private:
    std::vector<std::tuple<SX::Crystal::UnitCell,SX::Crystal::UBSolution,double>> _unitcells;
    SX::Crystal::UnitCell _basis;
    Ui::DialogUnitCell* ui;
    std::vector<std::reference_wrapper<SX::Crystal::Peak3D>> _peaks;
};

#endif // DialogUnitCell_H
