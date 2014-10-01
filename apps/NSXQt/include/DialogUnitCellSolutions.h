#ifndef DIALOGUNITCELLSOLUTIONS_H
#define DIALOGUNITCELLSOLUTIONS_H
#include <LatticeFinder.h>
#include <UBMinimizer.h>
#include <tuple>
#include <QDialog>
#include "UnitCell.h"

namespace Ui {
class DialogUnitCellSolutions;
}

class DialogUnitCellSolutions : public QDialog
{
    Q_OBJECT

public:
    explicit DialogUnitCellSolutions(QWidget *parent = 0);
    ~DialogUnitCellSolutions();

private:
    Ui::DialogUnitCellSolutions *ui;
public slots:
    void setSolutions(const std::vector<std::tuple<SX::Crystal::UnitCell,SX::Crystal::UBSolution,double>>& solutions);
    void selectLine(int i);
signals:
    void selectSolution(int i);
};

#endif // DIALOGUNITCELLSOLUTIONS_H
